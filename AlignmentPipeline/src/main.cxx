/* Emily Hammond
 * 2014-10-17
 *
 * This code is the main code written to align multi-modal images taken of the same
 * subject on the same day.
 * 
 * steps:
 *	1. Create a set up for the desired inputs and outputs
 *			Inputs: fixed image, moving image, output directory name
 *			Outputs: transformed moving image, final transform parameters, 
 *					 deformation image, jacobian map, histogram images		
 *	2. Read and write functions pulled from RegistrationCode/src/ReadWriteFunctions
 *	3. Obtain the histograms of the images, and the joint histogram (obtain jh from MMI class)
 *	3. Flow of program
 *		- Read in desired images
 *		- (Perform desired preprocessing steps)
 *		- Perform initial alignment
 *			CenteredTransformInitializer by geometry
 *		- Perform rigid registration
 *			Components: VersorRigid3DTransform, VersorRigid3DOptimizer, 
 *						MattesMutualInformation, LinearInterpolator
 *		- Give option for affine registration
 *			Components: AffineTransform, ConjugateGradientLineSearchOptimizerv4, 
 *						MattesMutualInformation, LinearInterpolator
 *		- Give option for bspline registration
 *			Components: BSplineTransform, ConjugateGradientLineSearchOptimizerv4, 
 *						MattesMutualInformation, LinearInterpolator
 *		- Write out all the desired files
 *
 */

/**************************************************************************
 * Header files to include
 **************************************************************************/

// reading/writing files
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"

// initialization
#include "itkCenteredTransformInitializer.h"

// creating histograms
#include "itkImageToHistogramFilter.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkJoinImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

// rigid registration
#include "itkVersorRigid3DTransform.h"
#include "itkMattesMutualInformationImageToImageMetricv4.h"
#include "itkLinearInterpolateImageFunction.h"

// additional C++ libraries
#include <itksys/SystemTools.hxx>
#include <fstream>
#include <string>

// landmark analysis
#include "itkLandmarkAnalysis.h"

/*************************************************************************
 * Write templated functions for reading/writing files to clean up code 
 *************************************************************************/

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
	
	// return output
	std::cout << ImageFilename << " has been read in!" << std::endl;
	return reader->GetOutput();
}


// Write a function to write out images templated over input and output pixel type
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
	
	// return output
	std::cout << ImageFilename << " has been written. " << std::endl;
	return EXIT_SUCCESS;
}

// Write a function to write out a transform
template<typename TransformType>
int WriteOutTransform( const char * transformFilename, typename TransformType::Pointer transform )
{
	typedef itk::TransformFileWriterTemplate< double > TransformWriterType;
	TransformWriterType::Pointer writer = TransformWriterType::New();
	writer->SetInput( transform );
	writer->SetFileName( transformFilename );

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
	
	// return output
	std::cout << transformFilename << " has successfully been created." << std::endl;
	return EXIT_SUCCESS;
}

// Write function to output the histogram
template<typename ImageType>
int CreateHistogram( const char * histogramFilename, typename ImageType::Pointer image, const int bins )
{
	// check to make sure bins in >0
	if (bins < 1)
	{
		std::cout << "Invalid value for bins" << std::endl;
		return EXIT_FAILURE;
	}

	// plot the histogram based on the number of bins
	typedef itk::Statistics::ImageToHistogramFilter< typename ImageType >	HistogramFilterType;
	HistogramFilterType::Pointer histogramFilter = HistogramFilterType::New();

	// determine what the minimum and maximum values of the image are
	typedef itk::MinimumMaximumImageCalculator< ImageType >	MinMaxCalculatorType;
	MinMaxCalculatorType::Pointer minMaxCalc = MinMaxCalculatorType::New();
	minMaxCalc->SetImage( image );
	minMaxCalc->Compute();
	ImageType::PixelType min = minMaxCalc->GetMinimum();
	ImageType::PixelType max = minMaxCalc->GetMaximum();

	// calculate the desired number of bins
	typedef HistogramFilterType::HistogramSizeType SizeType;
	SizeType size(1);
	size[0] = bins;
	histogramFilter->SetHistogramSize( size );
	histogramFilter->SetMarginalScale( 10.0 );

	// set bin min and max values (taken from example)
	typedef HistogramFilterType::HistogramMeasurementVectorType    HistogramMeasurementVectorType;
	HistogramMeasurementVectorType binMinimum(1);
	HistogramMeasurementVectorType binMaximum(1);
	binMinimum[0] = -0.5;
	binMaximum[0] = 255.5;
	histogramFilter->SetHistogramBinMinimum( binMinimum );
	histogramFilter->SetHistogramBinMaximum( binMaximum );

	// create the histogram
	histogramFilter->SetInput( image );
	histogramFilter->Update();

	typedef HistogramFilterType::HistogramType	HistogramType;
	const HistogramType * histogram = histogramFilter->GetOutput();

	// write the values out to a text file
	std::ofstream histogramTextFile;
	histogramTextFile.open( histogramFilename );

	// create iterators
	HistogramType::ConstIterator histItr = histogram->Begin();
	HistogramType::ConstIterator histEnd = histogram->End();

	// define the frequency type (and initialize the max frequency value)
	typedef HistogramType::AbsoluteFrequencyType	AbsoluteFrequencyType;
	AbsoluteFrequencyType maxFrequency = 0;
	
	while( histItr != histEnd )
	{
		const AbsoluteFrequencyType frequency = histItr.GetFrequency();

		HistogramType::IndexType index;
		index = histogram->GetIndex(histItr.GetInstanceIdentifier());
		histogramTextFile << index << "," << frequency << std::endl;
		if( frequency > maxFrequency )
		{		
			maxFrequency = frequency;
		}

		++histItr;
	}

	histogramTextFile.close();

	std::cout << histogramFilename << " has been successfully created and written to file." << std::endl;

	return EXIT_SUCCESS;
}

/*************************************************************************
 * Main function to perform/test functionality
 *************************************************************************/
int main(int argc, char * argv[])
{
	if( argc < 4 )
	{
		std::cerr << "Incorrect number of inputs: " << std::endl;
		std::cerr << "	main.exe fixedImage movingImage outputDirectory" << std::endl;
	}
	
	// list desired inputs
	const char * fixedImageFilename = argv[1];
	const char * movingImageFilename = argv[2];
	std::string outputDirectory = argv[3];
	std::string outputFileFormat = ".mhd";
	
	// use these if desiring the joint histograms
	//const int movingBins = atoi(argv[4]);
	//const int fixedBins = atoi(argv[5]);

	// use these if desiring to validate the registrations
	//const char * fixedFiducialList = argv[3];
	//const char * movingFiducialList = argv[4];

	// list desired outputs
	/*	
	std::string rigidResultFilename = outputDirectory + "\\rigidResult." + outputFileFormat;
	std::string rigidTransformFilename = outputDirectory + "\\rigidTransformParameters.txt";
	std::string deformableResultFilename = outputDirectory + "\\deformableResult." + outputFileFormat;
	std::string deformableTransformFilename = outputDirectory + "\\deformableTransformParameters.txt";
	std::string deformationFilename = outputDirectory + "\\deformation." + outputFileFormat;
	std::string jacobianMapFilename = outputDirectory + "\\jacobianMap." + outputFileFormat;
	std::string movingHistogramFilename = outputDirectory + "\\movingHistogram.txt";
	std::string fixedHistogramFilename = outputDirectory + "\\fixedHistogram.txt";
	std::string jointHistogramFilename = outputDirectory + "\\jointHistogram.tif";
	*/

	// read in necessary files
	const unsigned int		Dimension = 3;
	typedef float			PixelType;
	typedef unsigned char	CharPixelType;

	typedef itk::Image< PixelType, Dimension >	FixedImageType;
	typedef itk::Image< PixelType, Dimension >	MovingImageType;
	typedef itk::Image< CharPixelType, 2 >		JointHistogramImageType;

	// read in fixed and moving images
	FixedImageType::Pointer fixedImage = ReadInImage<FixedImageType>(fixedImageFilename);
	MovingImageType::Pointer movingImage = ReadInImage<MovingImageType>(movingImageFilename);

	// insert preprocessing steps
	//	- inhomogeneity correction
	//  - generation of mask files
	//  - etc.

	// set up rigid transform with initialization
	typedef itk::VersorRigid3DTransform< double >	RigidTransformType;
	RigidTransformType::Pointer rigidTransformGeom = RigidTransformType::New();
	RigidTransformType::Pointer rigidTransformMom = RigidTransformType::New();

	typedef itk::CenteredTransformInitializer< RigidTransformType, FixedImageType, MovingImageType >	InitializerType;
	InitializerType::Pointer initializer = InitializerType::New();
	
	// input images
	initializer->SetFixedImage( fixedImage );
	initializer->SetMovingImage( movingImage );

	// initialize by geometry
	initializer->SetTransform( rigidTransformGeom );
	initializer->GeometryOn();
	initializer->InitializeTransform();

	// write out transform after initialization
	std::string rigidInitGeomFilename = outputDirectory + "\\rigidInitGeom.tfm";
	WriteOutTransform< RigidTransformType >( rigidInitGeomFilename.c_str() , rigidTransformGeom );

	// initalize by moments
	initializer->SetTransform( rigidTransformMom );
	initializer->MomentsOn();
	initializer->InitializeTransform();

	// write out transform after initialization
	std::string rigidInitMomFilename = outputDirectory + "\\rigidInitMom.tfm";
	WriteOutTransform< RigidTransformType >( rigidInitMomFilename.c_str() , rigidTransformMom );

	/*
	// create the histograms
	CreateHistogram< MovingImageType >( movingHistogramFilename.c_str(), movingImage, movingBins );
	CreateHistogram< FixedImageType >( fixedHistogramFilename.c_str(), fixedImage, fixedBins );

	// set up interpolator
	typedef itk::LinearInterpolateImageFunction< MovingImageType, double >	InterpolatorType;
	InterpolatorType::Pointer interpolator = InterpolatorType::New();

	// set up the metric
	typedef itk::MattesMutualInformationImageToImageMetricv4< FixedImageType, MovingImageType > MetricType;
	MetricType::Pointer metric = MetricType::New();
	
	metric->SetFixedImage( fixedImage );
	metric->SetMovingImage( movingImage );
	metric->SetTransform( rigidTransform );
	metric->SetMovingInterpolator( interpolator );

	metric->SetNumberOfHistogramBins( 100 );
	metric->Initialize();
	std::cout << metric->GetValue() << std::endl;

	// obtain joint histogram and rescale
	typedef itk::Image< MetricType::PDFValueType, 2 >	JPDFImageType;
	typedef itk::RescaleIntensityImageFilter< JPDFImageType, JointHistogramImageType >	RescaleImageType;
	RescaleImageType::Pointer rescaler = RescaleImageType::New();
	rescaler->SetInput( metric->GetJointPDF() );
	rescaler->SetOutputMinimum( 0 );
	rescaler->SetOutputMaximum( 255 );
	JointHistogramImageType::Pointer jpdfCharImage = rescaler->GetOutput();

	// write out jpdf to file
	WriteOutImage< JointHistogramImageType, JointHistogramImageType >( jointHistogramFilename.c_str(), jpdfCharImage );
	*/

	return EXIT_SUCCESS;
}