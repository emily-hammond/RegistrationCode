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
#include "itkMattesMutualInformationImageToImageMetric.h" // v4 does not yet support local-deforming transforms
#include "itkLinearInterpolateImageFunction.h"
#include "itkImageRegistrationMethod.h"
#include "itkVersorRigid3DTransformOptimizer.h"

// additional C++ libraries
#include <itksys/SystemTools.hxx>
#include <fstream>
#include <string>

// landmark analysis
#include "C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\src\transformFiducials\itkLandmarkAnalysis.h"

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
		std::cout << ImageFilename << " has been read in!" << std::endl;
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}
	
	// return output
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
		std::cout << ImageFilename << " has been written. " << std::endl;
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

template< class T >
// Write function to help output things to the out-stream
std::string to_string(T t)
{
	std::ostringstream oss;
	oss << t;
	return oss.str();
}


/*************************************************************************
 * Write functions to monitor the registration process!
 *************************************************************************/
// still need to implement

/*************************************************************************
 * Main function to perform/test functionality
 *************************************************************************/
int main(int argc, char * argv[])
{
	if( argc < 6 )
	{
		std::cerr << "Incorrect number of inputs: " << std::endl;
		std::cerr << "	main.exe fixedImage movingImage outputDirectory movingBins fixedBins" << std::endl;
	}
	
	//*********************** INPUTS *******************************
	// images
	std::string fixedImageFilename = argv[1];
	std::string movingImageFilename = argv[2];
	// mask files (add in later)
	// output directory/file format
	std::string outputDirectory = argv[3];
	std::string outputFileFormat = ".mhd";
	// breakdown files to get baseFilename
	// moving image
	std::string baseMovingFilename = movingImageFilename.substr( movingImageFilename.find_last_of("/\\") + 1 );
	baseMovingFilename = baseMovingFilename.substr(0, baseMovingFilename.find_last_of('.'));
	// fixed image
	std::string baseFixedFilename = fixedImageFilename.substr( fixedImageFilename.find_last_of("/\\") + 1 );
	baseFixedFilename = baseFixedFilename.substr(0, baseFixedFilename.find_last_of('.'));
	// joint histogram bins (curiosity)
	const int movingBins = atoi(argv[4]);
	const int fixedBins = atoi(argv[5]);
	// landmark filenames (validation)
	//const char * fixedFiducialList = argv[3];
	//const char * movingFiducialList = argv[4];

	//*********************** OUTPUTS *******************************
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

	// read in fixed and moving images
	FixedImageType::Pointer fixedImage = ReadInImage<FixedImageType>(fixedImageFilename.c_str());
	MovingImageType::Pointer movingImage = ReadInImage<MovingImageType>(movingImageFilename.c_str());

	// insert preprocessing steps
	//	- inhomogeneity correction
	//  - generation of mask files
	//  - etc.

	// ************************* TRANSFORM *******************************
	// set up rigid transform with initialization
	typedef itk::VersorRigid3DTransform< double >	RigidTransformType;
	RigidTransformType::Pointer rigidTransform = RigidTransformType::New();

	// ********************** INITIALIZATION *****************************
	typedef itk::CenteredTransformInitializer< RigidTransformType, FixedImageType, MovingImageType >	InitializerType;
	InitializerType::Pointer initializer = InitializerType::New();
	
	// input images
	initializer->SetFixedImage( fixedImage );
	initializer->SetMovingImage( movingImage );

	// initialize by geometry
	initializer->SetTransform( rigidTransform );
	initializer->GeometryOn();
	initializer->InitializeTransform();

	// write out transform after initialization
	std::string rigidInitGeomFilename = outputDirectory + "\\" + baseMovingFilename + "_rigidInitGeom.tfm";
	WriteOutTransform< RigidTransformType >( rigidInitGeomFilename.c_str() , rigidTransform );

	// ************************ HISTOGRAMS *******************************
	// moving image
	std::string movingHistogramFilename = outputDirectory + "\\" + baseMovingFilename + "_" + to_string( movingBins ) + "Histogram.txt";
	CreateHistogram< MovingImageType >( movingHistogramFilename.c_str(), movingImage, movingBins );
	// fixed image
	std::string fixedHistogramFilename = outputDirectory + "\\" + baseFixedFilename + "_" + to_string( fixedBins ) + "Histogram.txt";
	CreateHistogram< FixedImageType >( fixedHistogramFilename.c_str(), fixedImage, fixedBins );

	// *********************** INTERPOLATOR ******************************
	typedef itk::LinearInterpolateImageFunction< MovingImageType, double >	InterpolatorType;
	InterpolatorType::Pointer interpolator = InterpolatorType::New();

	// *************************** METRIC ********************************
	typedef itk::MattesMutualInformationImageToImageMetric< FixedImageType, MovingImageType > MetricType;
	MetricType::Pointer metric = MetricType::New();

	// ************************ OPTIMIZER ********************************
	typedef itk::VersorRigid3DTransformOptimizer	RigidOptimizerType;
	RigidOptimizerType::Pointer rigidOptimizer = RigidOptimizerType::New();
	// set parameters
	rigidOptimizer->SetMinimumStepLength( 0.001 );
	rigidOptimizer->SetMaximumStepLength( 0.1 );
	rigidOptimizer->SetNumberOfIterations( 1000 );

	// ******************* REGISTRATION METHOD ***************************
	typedef itk::ImageRegistrationMethod< FixedImageType, MovingImageType >		RegistrationType;
	RegistrationType::Pointer registration = RegistrationType::New();
	// set components for rigid registration
	registration->SetMetric( metric );
	registration->SetOptimizer( rigidOptimizer );
	registration->SetTransform( rigidTransform );
	registration->SetInterpolator( interpolator );
	// set images
	registration->SetFixedImage( fixedImage );
	registration->SetMovingImage( movingImage );
	// perform registration
	std::cout << std::endl;
	std::cout << "Beginning Registration" << std::endl;
	try
	{
		registration->Update();
		std::cout << "OptimizerStopCondition: " << registration->GetOptimizer()->GetStopConditionDescription() << std::endl;
	}
	catch( itk::ExceptionObject & err )
	{
		std::cerr << "ExceptionObjectCaught!" << std::endl;
		std::cerr << err << std::endl;
		return EXIT_FAILURE;
	}

	// print results to the screen
	std::cout << std::endl;
	std::cout << " Iterations      : " << rigidOptimizer->GetCurrentIteration() << std::endl;
	std::cout << " Metric value    : " << rigidOptimizer->GetValue() << std::endl;
	std::cout << " #histogram bins : " << metric->GetNumberOfHistogramBins() << std::endl;
	std::cout << " #spatial samples: " << registration->GetMetric()->GetNumberOfSpatialSamples() << std::endl;
	std::cout << " #pixels counted : " << registration->GetMetric()->GetNumberOfPixelsCounted() << std::endl;
	std::cout << " #moving samples : " << registration->GetMetric()->GetNumberOfMovingImageSamples() << std::endl;

	// obtain joint histogram and rescale
	typedef itk::Image< MetricType::PDFValueType, 2 >	JPDFImageType;
	typedef itk::Image< CharPixelType, 2 >				JointHistogramImageType;
	typedef itk::RescaleIntensityImageFilter< JPDFImageType, JointHistogramImageType >	RescaleFilterType;
	RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
	rescaler->SetInput( metric->GetJointPDF() );
	rescaler->SetOutputMinimum( 0 );
	rescaler->SetOutputMaximum( 255 );
	JointHistogramImageType::Pointer image = JointHistogramImageType::New();

	// write out jpdf to file
	std::string jointHistogramFilename = outputDirectory + "\\" + baseMovingFilename + "_" + baseFixedFilename + "_jointHistogram.tif";
	WriteOutImage< JointHistogramImageType, JointHistogramImageType >( jointHistogramFilename.c_str(), image );

	// write final rigid transform out to file
	std::string finalRigidTransformFilename = outputDirectory + "\\" + baseMovingFilename + "_rigidTransform.txt";
	rigidTransform->SetParameters( registration->GetLastTransformParameters() );
	WriteOutTransform< RigidTransformType >( finalRigidTransformFilename.c_str(), rigidTransform );

	return EXIT_SUCCESS;
}