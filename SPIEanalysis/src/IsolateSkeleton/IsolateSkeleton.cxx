/*
Author: Emily Hammond
Date: 2014.11.07

The purpose of this code is to perform automatic isolation of the skeleton post registration of the SPIE data.

Steps:
	1. Threshold mhd data -> isolate > 1224 (scaled up from HU CT data to have a min of 0)
	2. Dilate the label map
	3. Isolate each connected area (> 1000 pixels in size)
	4. Combine the first four/five regions
	5. Write result out to file
*/

// read/write image functions
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"

// threshold image/combine labels
#include "itkMinimumMaximumImageCalculator.h"
#include "itkBinaryThresholdImageFilter.h"

// dilate the image
#include "itkBinaryBallStructuringElement.h"
#include "itkBinaryDilateImageFilter.h"

// isolate labels
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"

// Write a function to read in images templated over dimension and pixel type
template<typename ImageType>
typename ImageType::Pointer ReadInImage( const char * ImageFilename )
{
	typedef itk::ImageFileReader<ImageType>		ReaderType;	
	typename ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName( ImageFilename );
	
	// update reader
	try
	{
		reader->Update();
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}

	std::cout << std::endl;
	std::cout << ImageFilename << " has been read in!" << std::endl;
	
	// return output
	return reader->GetOutput();
}

// Write a function to write out images
template<typename inputImageType, typename outputImageType>
int WriteOutImage( const char * ImageFilename, typename inputImageType::Pointer image )
{
	typedef itk::CastImageFilter<inputImageType, outputImageType> CastFilterType;
	typename CastFilterType::Pointer caster = CastFilterType::New();
	caster->SetInput( image );

	typedef itk::ImageFileWriter<outputImageType> WriterType;
	typename WriterType::Pointer writer = WriterType::New();
	writer->SetFileName( ImageFilename );
	writer->SetInput( caster->GetOutput() );

	// update the writer
	try
	{
		writer->Update();
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}

	std::cout << std::endl;
	std::cout << ImageFilename << " has been written to file!" << std::endl;
	
	// return output
	return EXIT_SUCCESS;
}

int main(int argc, char * argv[])
{
	std::string inputFilename = argv[1];
	std::string outputDirectory = argv[2];
	unsigned int radius = atoi( argv[3] );

	std::string thresholdImageFilename = outputDirectory + "\\thresholdedImage.mhd";
	std::string dilatedImageFilename = outputDirectory + "\\dilatedImage.mhd";
	std::string connectedImageFilename = outputDirectory + "\\connectedImage.mhd";
	std::string relabeledImageFilename = outputDirectory + "\\relabeledImage.mhd";
	std::string combinedImageFilename = outputDirectory + "\\combinedImage.mhd";
	std::string labelMapImageFilename = outputDirectory + "\\lableMap.mhd";

	// determine which images are going to be needed
	const unsigned int Dimension = 3;
	typedef float	FloatPixelType;
	typedef unsigned char	CharPixelType;

	typedef itk::Image< FloatPixelType, Dimension >		FloatImageType;
	typedef itk::Image< CharPixelType, Dimension >		CharImageType;

	// read in image
	FloatImageType::Pointer image = ReadInImage< FloatImageType >( inputFilename.c_str() );

	// determine min/max of image
	typedef itk::MinimumMaximumImageCalculator< FloatImageType >	MinMaxCalculatorType;
	MinMaxCalculatorType::Pointer minMaxCalc = MinMaxCalculatorType::New();
	minMaxCalc->SetImage( image );
	minMaxCalc->Compute();
	FloatImageType::PixelType min = minMaxCalc->GetMinimum();
	FloatImageType::PixelType max = minMaxCalc->GetMaximum();

	// threshold the image
	typedef itk::BinaryThresholdImageFilter< FloatImageType, CharImageType >	BinaryThresholdType;
	BinaryThresholdType::Pointer thresholdFilter = BinaryThresholdType::New();

	thresholdFilter->SetInput( image );
	thresholdFilter->SetLowerThreshold( min + 1200 );
	thresholdFilter->SetUpperThreshold( max );
	thresholdFilter->SetInsideValue( 1 );
	thresholdFilter->SetOutsideValue( 0 );

	thresholdFilter->Update();

	// define the structuring element
	typedef itk::BinaryBallStructuringElement< CharImageType::PixelType, 3 >	StructuringElementType;
	StructuringElementType	structuringElement;
	structuringElement.SetRadius( radius );
	structuringElement.CreateStructuringElement();

	// dilate the image
	typedef itk::BinaryDilateImageFilter< CharImageType, CharImageType, StructuringElementType>		DilateFilterType;
	DilateFilterType::Pointer dilateFilter = DilateFilterType::New();
	dilateFilter->SetInput( thresholdFilter->GetOutput() );
	dilateFilter->SetKernel( structuringElement );

	// set the dilate value
	CharImageType::PixelType dilateValue = 1;
	dilateFilter->SetDilateValue( dilateValue );
	dilateFilter->Update();

	// isolate labels
	typedef itk::ConnectedComponentImageFilter< CharImageType, CharImageType >	ConnectedComponentType;
	ConnectedComponentType::Pointer connectedFilter = ConnectedComponentType::New();

	connectedFilter->SetInput( dilateFilter->GetOutput() );
	connectedFilter->FullyConnectedOff();
	connectedFilter->Update();

	// output number of objects identified
	std::cout << std::endl;
	std::cout << "Number of objects found: " << connectedFilter->GetObjectCount() << std::endl;

	// relabel the output to be in consecutive size and order
	typedef itk::RelabelComponentImageFilter< CharImageType, CharImageType >	RelabelComponentType;
	RelabelComponentType::Pointer relabelFilter = RelabelComponentType::New();

	relabelFilter->SetInput( connectedFilter->GetOutput() );
	relabelFilter->SetMinimumObjectSize( 20000 );
	//relabelFilter->SetSortByObjectSize( true );
	relabelFilter->Update();

	// raster through the labels and print out the label with the corresponding size
	RelabelComponentType::LabelType numOfObjects = relabelFilter->GetNumberOfObjects();
	RelabelComponentType::LabelType objectLabel = 1;
	std::cout << std::endl;
	std::cout << "Remaining labels with pixel size: " << std::endl;

	// detemine how many labels to include
	RelabelComponentType::LabelType lowerThreshold = 1;
	RelabelComponentType::LabelType upperThreshold = 1;
	int stopFlag = 0;
	
	for( objectLabel; objectLabel <= numOfObjects; ++objectLabel )
	{
		RelabelComponentType::ObjectSizeType objectSize = relabelFilter->GetSizeOfObjectInPixels( objectLabel );
		RelabelComponentType::ObjectSizeType objectSizePrev = relabelFilter->GetSizeOfObjectInPixels( objectLabel-1 );

		// print information out to file
		std::cout << "[" << objectLabel << "]: " << objectSize << std::endl;

		// check to make sure the ratio between volumes is small enough, once not, stop at that label
		if( (objectLabel > 2) &&  ((float)(objectSizePrev - objectSize)/objectSizePrev > 0.3) && (stopFlag == 0) )
		{
			// change upper label value and stop flag
			upperThreshold = objectLabel - 1;
			stopFlag = 0;
		}
	}

	// print out which labels are being combined
	std::cout << std::endl;
	std::cout << "Combining labels 1 through " << upperThreshold << std::endl;

	// threshold the image to combine the labels
	typedef itk::BinaryThresholdImageFilter< CharImageType, CharImageType >		ThresholdImageType;
	ThresholdImageType::Pointer combineLabelFilter = ThresholdImageType::New();

	combineLabelFilter->SetInput( relabelFilter->GetOutput() );
	combineLabelFilter->SetLowerThreshold( lowerThreshold );
	combineLabelFilter->SetUpperThreshold( upperThreshold );
	combineLabelFilter->SetInsideValue( 1 );
	combineLabelFilter->SetOutsideValue( 0 );
	combineLabelFilter->Update();

	// write out the images
	//WriteOutImage< CharImageType, CharImageType >( thresholdImageFilename.c_str(), thresholdFilter->GetOutput() );
	//WriteOutImage< CharImageType, CharImageType >( dilatedImageFilename.c_str(), dilateFilter->GetOutput() );
	//WriteOutImage< CharImageType, CharImageType >( connectedImageFilename.c_str(), connectedFilter->GetOutput() );
	WriteOutImage< CharImageType, CharImageType >( relabeledImageFilename.c_str(), relabelFilter->GetOutput() );
	WriteOutImage< CharImageType, CharImageType >( combinedImageFilename.c_str(), combineLabelFilter->GetOutput() );

	return EXIT_SUCCESS;
}
