/*
 * Emily Hammond
 * 10/5/2015
 *
 * The goal of this code is to read in a previously determined transform
 * and label maps from the fixed image and moving image for that transform.
 * This code then determines the overlap measures and outputs them to a
 * csv file.
 * 
 */
 
// reading/writing files
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"
 
#include "itkResampleImageFilter.h"
#include "itkScaleVersor3DTransform.h"
#include <fstream>
 
// overlap analysis
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkLabelOverlapMeasuresImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkHausdorffDistanceImageFilter.h"

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
		std::cout << ImageFilename << std::endl;
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}
	
	// return output
	return reader->GetOutput();
}

// Write a function to write out images templated over input and output pixel type
template<typename inputImageType, typename outputImageType>
int WriteOutImage( const char * ImageFilename, typename inputImageType::Pointer image)
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
		return EXIT_FAILURE;
	}
	
	// return output
	return EXIT_SUCCESS;
}

// Write function to perform overlap statistics
template< typename ImageType >
int LabelOverlapMeasures( typename ImageType::Pointer source, typename ImageType::Pointer target, std::ofstream & file )
{
	// instantiate filter and insert images
	typedef itk::LabelOverlapMeasuresImageFilter< ImageType >  FilterType;
	FilterType::Pointer filter = FilterType::New();
	filter->SetSourceImage( source );
	filter->SetTargetImage( target );

	// update filter
	try
	{
		filter->Update();
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}

	// write out results to the screen
	// for all labels
	file << " ** All Labels ** " << std::endl;
	file << ",Total,Union (jaccard),Mean (dice),Volume sim.,False negative,False positive\n";
	file << "," << filter->GetTotalOverlap();
	file << "," << filter->GetUnionOverlap();
	file << "," << filter->GetMeanOverlap();
	file << "," << filter->GetVolumeSimilarity();
	file << "," << filter->GetFalseNegativeError();
	file << "," << filter->GetFalsePositiveError();
	file << std::endl;

	file << " ** Individual Labels ** " << std::endl;
	file << "Label,Target,Union (jaccard),Mean (dice),Volume sim.,False negative,False positive,Hausdorff distance,Average distance\n";

	// for each individual labels
	FilterType::MapType labelMap = filter->GetLabelSetMeasures();
	FilterType::MapType::const_iterator it;
	for( it = labelMap.begin(); it != labelMap.end(); ++it )
    {		
		// ignore label 0 (background)
		if( (*it).first == 0 )
		{
			continue;
		}

		// identify label
		int label = (*it).first;
		std::cout << "LABEL: " << label << std::endl;

		// write out to file
		file << label;
		file << "," << filter->GetTargetOverlap( label );
		file << "," << filter->GetUnionOverlap( label );
		file << "," << filter->GetMeanOverlap( label );
		file << "," << filter->GetVolumeSimilarity( label );
		file << "," << filter->GetFalseNegativeError( label );
		file << "," << filter->GetFalsePositiveError( label );

		// isolate label in source image
		typedef itk::BinaryThresholdImageFilter< ImageType, ImageType > ThresholdType;
		typename ThresholdType::Pointer disSource = ThresholdType::New();
		disSource->SetInput( filter->GetSourceImage() );
		disSource->SetLowerThreshold( label );
		disSource->SetUpperThreshold( label );
		disSource->SetInsideValue( static_cast< ImageType::PixelType >( 1 ) );
		disSource->SetOutsideValue( static_cast< ImageType::PixelType >( 0 ) );
		disSource->Update();
		std::cout << "Source image thresholded" << std::endl;
		if(label == 1)
		{
			std::string filename = "SourceFile.mhd";
			WriteOutImage< ImageType, ImageType >( filename.c_str(), disSource->GetOutput() );
		}

		// isolate label in target image
		typename ThresholdType::Pointer disTarget = ThresholdType::New();
		disTarget->SetInput( filter->GetTargetImage() );
		disTarget->SetLowerThreshold( label );
		disTarget->SetUpperThreshold( label );
		disTarget->SetInsideValue( static_cast< ImageType::PixelType >( 1 ) );
		disTarget->SetOutsideValue( static_cast< ImageType::PixelType >( 0 ) );
		disTarget->Update();
		std::cout << "Target image thresholded" << std::endl;
		if(label == 1)
		{
			std::string filename = "TargetFile.mhd";
			WriteOutImage< ImageType, ImageType >( filename.c_str(), disSource->GetOutput() );
		}

		// calculate Hausdorff distance
		typedef itk::HausdorffDistanceImageFilter< ImageType, ImageType >	DistanceType;
		typename DistanceType::Pointer distance = DistanceType::New();
		distance->SetInput1( disSource->GetOutput() );
		distance->SetInput2( disTarget->GetOutput() );
		if( filter->GetVolumeSimilarity( label ) > -2 )
		{
			std::cout << "Calculating distances." << std::endl;
			distance->Update();
		
			//distance->Print( std::cout );
			file << "," << distance->GetHausdorffDistance();
			file << "," << distance->GetAverageHausdorffDistance();
		}
		file << std::endl;
	}

	return EXIT_SUCCESS;
}

int main(int argc, char * argv[])
{
	// inputs
	if( argc < 4 )
	{
		std::cout << "Not enough input arguments." << std::endl;
		std::cout << "    determineOverlap.exe fixedLMfilename movingLMfilename transformFilename" << std::endl;
	}
	
	const char * FixedLabelMapFilename = argv[1];
	const char * MovingLabelMapFilename = argv[2];
	const char * TransformFilename = argv[3];
	std::string LabelMapMeasuresFilename = "LabelMapMeasures.csv";
	
	// read in label maps and transform
	typedef unsigned int							LabelMapPixelType;
	typedef itk::Image< LabelMapPixelType, 3 >		LabelMapImageType;
	
	LabelMapImageType::Pointer fixedLM = ReadInImage< LabelMapImageType >( FixedLabelMapFilename );
	LabelMapImageType::Pointer movingLM = ReadInImage< LabelMapImageType >( MovingLabelMapFilename );
	
	// define transform
	typedef itk::ScaleVersor3DTransform< double >	RigidTransformType;
	RigidTransformType::Pointer rigidTransform = RigidTransformType::New();
	
	// read in transform
	typedef itk::TransformFileReader	TransformReaderType;
	TransformReaderType::Pointer transformReader = TransformReaderType::New();
	
	transformReader->SetFileName( TransformFilename );
	try
	{
		transformReader->Update();
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}

	// check to make sure there is only one transform here
	TransformReaderType::TransformListType * transforms = transformReader->GetTransformList();
	if( transforms->size() != 1 )
	{
		std::cerr << "There is more than 1 transform in initial transform file" << std::endl;
		return EXIT_FAILURE;
	}

	// store initial transform into rigid transform
	TransformReaderType::TransformListType::const_iterator it = transforms->begin();
	rigidTransform = static_cast< RigidTransformType * >( (*it).GetPointer() );
	
	// resample moving label mask with the transform
	typedef itk::ResampleImageFilter< LabelMapImageType, LabelMapImageType >	LMResampleFilterType;
	LMResampleFilterType::Pointer lmresampler = LMResampleFilterType::New();
	
	// initialize with fixed image parameters
	lmresampler->SetSize( fixedLM->GetLargestPossibleRegion().GetSize() );
	lmresampler->SetOutputOrigin( fixedLM->GetOrigin() );
	lmresampler->SetOutputSpacing( fixedLM->GetSpacing() );
	lmresampler->SetOutputDirection( fixedLM->GetDirection() );
	lmresampler->SetDefaultPixelValue( 0.0 );
	
	// perform resampling
	lmresampler->SetInput( movingLM );
	lmresampler->SetTransform( rigidTransform );
	
	// obtain overlap measures
	std::ofstream file;
	file.open( LabelMapMeasuresFilename.c_str() );
	file << "Source: " << FixedLabelMapFilename << std::endl;
	file << "Target: " << MovingLabelMapFilename << std::endl;
	LabelOverlapMeasures< LabelMapImageType >( fixedLM, lmresampler->GetOutput(), file );
	file << std::endl;
	file.close();
	
	return EXIT_SUCCESS;
}
	