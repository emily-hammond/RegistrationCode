/* Emily Hammond
 * 2014-10-17
 *
 * This code is the main code written to align multi-modal images taken of the same
 * subject on the same day.
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
#include "itkMattesMutualInformationImageToImageMetric.h" // v4 does not yet support local-deforming transforms
#include "itkLinearInterpolateImageFunction.h"
#include "itkImageRegistrationMethod.h"
#include "itkRegularStepGradientDescentOptimizer.h"

// different transforms
#include "itkScaleVersor3DTransform.h"
#include "itkVersorRigid3DTransform.h"
#include "itkVersorRigid3DTransformOptimizer.h"

// monitoring
#include "itkCommand.h"
#include "itkTimeProbesCollectorBase.h"
#include "itkMemoryProbesCollectorBase.h"

// applying transform
#include "itkResampleImageFilter.h"

// additional C++ libraries
#include <itksys/SystemTools.hxx>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

// landmark analysis
#include "C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\src\transformFiducials\itkLandmarkAnalysis.h"

// parsing inputs
#include "C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\src\parseInputFile\itkParseInputFile.h"
// creating the output directory (NOT CROSS PLATFORM!!!!)
#include <windows.h>


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
		return EXIT_FAILURE;
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

// Write function to help output things to the out-stream
template< class T >
std::string to_string(T t)
{
	std::ostringstream oss;
	oss << t;
	return oss.str();
}

// Write function to return the FOV of the fixed and moving images (use for metric initialization)
template< typename ImageType >
typename ImageType::PointType GetImageRange( typename ImageType::Pointer image, std::string returnOption )
{
	// extract state variables from the image
	const ImageType::PointType &     imageOrigin = image->GetOrigin();
	const ImageType::SpacingType &   imageSpacing = image->GetSpacing();
	const ImageType::SizeType &      imageSize = image->GetLargestPossibleRegion().GetSize();
	const ImageType::IndexType &     imageIndex = image->GetLargestPossibleRegion().GetIndex();
  
	// What is the name of the last pixel index location?
	ImageType::IndexType lastPixelIndex;
	// obtain information from the size of the largest possible region
	lastPixelIndex[0] = imageSize[0]-1;
	lastPixelIndex[1] = imageSize[1]-1;
	lastPixelIndex[2] = imageSize[2]-1;

	// get physcial location of lastPixelIndex to the lastIndexPhysicalPoint
	ImageType::PointType lastIndexPhysicalPoint;
	// use this function to take in a pixel index and a reference to a point index (output variable) - this is done to avoid the temporary variable in a for loop
	image->TransformIndexToPhysicalPoint( lastPixelIndex, lastIndexPhysicalPoint);

	// What is the image field of view (FOV)?
	// find the upper most corner of the image
	itk::ContinuousIndex<double,3> upperImageCorner;
	// add 0.5 to the last pixel index (previously calculated)
	upperImageCorner[0] = lastPixelIndex[0] + 0.5;
	upperImageCorner[1] = lastPixelIndex[1] + 0.5;
	upperImageCorner[2] = lastPixelIndex[2] + 0.5;

	// convert to physical coordinates
	ImageType::PointType FOVend;
	image->TransformContinuousIndexToPhysicalPoint(upperImageCorner, FOVend);

	// return FOV end if desired
	if( returnOption.compare("end") == 0 )
	{
		return FOVend;
	}

	// find the lower most corner of the image
	itk::ContinuousIndex<double,3> lowerImageCorner;
	// use the current index, subtract the start index and then another half voxel
	lowerImageCorner[0] = imageIndex[0] - 0.0 - 0.5;
	lowerImageCorner[1] = imageIndex[1] - 0.0 - 0.5;
	lowerImageCorner[2] = imageIndex[2] - 0.0 - 0.5;
	// convert to physical coordinates
	ImageType::PointType FOVstart;
	image->TransformContinuousIndexToPhysicalPoint(lowerImageCorner, FOVstart);

	// return FOV start if desired
	if( returnOption.compare("start") == 0 )
	{
		return FOVstart;
	}

	// find the center of physical space (center of the FOV)
	ImageType::PointType FOVcenter;
	// average out the FOV end and start
	FOVcenter[0] = (FOVend[0] + FOVstart[0])*0.5;
	FOVcenter[1] = (FOVend[1] + FOVstart[1])*0.5;
	FOVcenter[2] = (FOVend[2] + FOVstart[2])*0.5;

	// return FOV end if desired
	if( returnOption.compare("center") == 0 )
	{
		return FOVcenter;
	}
}

/*************************************************************************
 * Write functions to monitor the registration process!
 *************************************************************************/
class RigidCommandIterationUpdate: public itk::Command
{
public:
	typedef RigidCommandIterationUpdate	Self;
	typedef itk::Command				Superclass;
	typedef itk::SmartPointer< Self >	Pointer;
	itkNewMacro( Self );
protected:
	RigidCommandIterationUpdate() {};
public:
	//typedef itk::RegularStepGradientDescentOptimizer	OptimizerType;
	typedef itk::VersorRigid3DTransformOptimizer		OptimizerType;
	typedef const OptimizerType *						OptimizerPointer;
	void Execute( itk::Object *caller, const itk::EventObject &event)
	{
		Execute( (const itk::Object *)caller, event);
	}
	void Execute( const itk::Object * object, const itk::EventObject &event )
	{
		OptimizerPointer	optimizer = static_cast< OptimizerPointer >( object );
		if( ! itk::IterationEvent().CheckEvent( &event ) )
		{
			return;
		}
		


		std::cout << optimizer->GetCurrentIteration() << " " << optimizer->GetCurrentStepLength() << " " << optimizer->GetGradientMagnitude();
		std::cout << " " << optimizer->GetValue() << " " << optimizer->GetCurrentPosition() << std::endl;
	}
};

/*************************************************************************
 * Main function to perform/test functionality
 *************************************************************************/
int main(int argc, char * argv[])
{
	// set up accounting for time and memory in the program
	itk::TimeProbesCollectorBase chronometer;
	itk::MemoryProbesCollectorBase memorymeter;

	std::cout << argc << std::endl;

	memorymeter.Start( "Full program" );
	chronometer.Start( "Full program" );

	if( argc < 2 )
	{
		std::cerr << "Incorrect number of inputs: " << std::endl;
		std::cerr << "	main.exe inputFilename " << std::endl;
		return EXIT_FAILURE;
	}
	
	//*********************** INPUTS *******************************
	memorymeter.Start( "Inputs complete" );
	chronometer.Start( "Inputs complete" );

	// parse input file
	std::string inputFilename = argv[1];
	typedef itk::ParseInputFile		ParsingType;
	ParsingType::Pointer inputs = ParsingType::New();
	inputs->SetFilename( inputFilename.c_str() );
/*	if( !inputs->IsOpen() )
	{
		std::cout << "File is not open." << std::endl;
		return EXIT_FAILURE;
	}*/
	inputs->Print();

	// create output directory
	if( !CreateDirectory( inputs->OutputDirectory().c_str(), NULL ) )
	{
		std::cout << "Output Directory already exists." << std::endl;
	}

	// ******************* DEFINE/READ IN IMAGES *************************
	// define image types
	const unsigned int		Dimension = 3;
	typedef float			FloatPixelType;
	typedef unsigned char	CharPixelType;

	typedef itk::Image< FloatPixelType, Dimension >	FloatImageType;
	typedef itk::Image< CharPixelType, Dimension >  CharImageType;

	// read in fixed and moving images
	FloatImageType::Pointer fixedImage = ReadInImage<FloatImageType>(inputs->FixedImageFilename().c_str());
	FloatImageType::Pointer movingImage = ReadInImage<FloatImageType>(inputs->MovingImageFilename().c_str());

	memorymeter.Stop( "Inputs complete" );
	chronometer.Stop( "Inputs complete" );
	// ************************ HISTOGRAMS *******************************
	memorymeter.Start( "Generating histograms" );
	chronometer.Start( "Generating histograms" );

	// write out histograms for the moving image
	CreateHistogram< FloatImageType >( inputs->MovingHistogramFilename().c_str(), movingImage, inputs->NumberOfHistogramBins() );
	// fixed image
	CreateHistogram< FloatImageType >( inputs->FixedHistogramFilename().c_str(), fixedImage, inputs->NumberOfHistogramBins() );

	memorymeter.Stop( "Generating histograms" );
	chronometer.Stop( "Generating histograms" );
	// ************************* TRANSFORM *******************************
	// set up rigid transform types
	//typedef itk::ScaleVersor3DTransform< double >	RigidTransformType;
	typedef itk::VersorRigid3DTransform< double >	RigidTransformType;

	// instantiate transforms
	RigidTransformType::Pointer rigidTransform = RigidTransformType::New();

	// ***************** GEOMETRICAL INITIALIZATION **********************
	memorymeter.Start( "Initialization" );
	chronometer.Start( "Initialization" );

	// instantiate initializer (align geometrical center of images)
	typedef itk::CenteredTransformInitializer< RigidTransformType, FloatImageType, FloatImageType >	InitializerType;
	InitializerType::Pointer initializer = InitializerType::New();
	
	// input images
	initializer->SetFixedImage( fixedImage );
	initializer->SetMovingImage( movingImage );

	// initialize by geometry
	initializer->SetTransform( rigidTransform );
	initializer->GeometryOn();
	initializer->InitializeTransform();

	// write out transform after initialization
	WriteOutTransform< RigidTransformType >( inputs->InitGeomFilename().c_str() , rigidTransform );

	// *********************** INTERPOLATOR ******************************
	typedef itk::LinearInterpolateImageFunction< FloatImageType, double >	InterpolatorType;
	InterpolatorType::Pointer interpolator = InterpolatorType::New();

	// *************************** METRIC ********************************
	// the v3 metric does not have the function GetJointPDF!!!
	typedef itk::MattesMutualInformationImageToImageMetric< FloatImageType, FloatImageType > MetricType;
	MetricType::Pointer metric = MetricType::New();

	// determine number of samples to use
	FloatImageType::SizeType size = fixedImage->GetLargestPossibleRegion().GetSize();
	int numOfPixels = size[0]*size[1]*size[2];
	std::cout << "Number of pixels in fixed image: " << numOfPixels << std::endl;
	// use 1% of the fixed image samples
	if( inputs->PercentageOfSamples() > 0 )
	{
		metric->SetNumberOfSpatialSamples( inputs->PercentageOfSamples()*numOfPixels );
	}
	if( inputs->NumberOfHistogramBins() > 0 )
	{
		metric->SetNumberOfHistogramBins( inputs->NumberOfHistogramBins() );
	}

	// ******************* METRIC INITIALIZATION *************************
	std::cout << "\nPerform metric initialization" << std::endl;
	// iterate through the transform in the z direction and calculate metric
	// for an additional initialization
	metric->SetFixedImage( fixedImage );
	metric->SetMovingImage( movingImage );
	metric->SetFixedImageRegion( fixedImage->GetLargestPossibleRegion() );
	metric->SetTransform( rigidTransform );
	metric->SetInterpolator( interpolator );
	
	// initialize
	metric->Initialize();

	if( inputs->MetricInitialization() )
	{
		// identify FOVs of both images
		FloatImageType::PointType movingEnd = GetImageRange< FloatImageType >( movingImage, "end" );
		FloatImageType::PointType movingStart = GetImageRange< FloatImageType >( movingImage, "start" );
		FloatImageType::PointType fixedStart = GetImageRange< FloatImageType >( fixedImage, "start" );
		FloatImageType::PointType fixedCenter = GetImageRange< FloatImageType >( fixedImage, "center" );

		std::cout << "MovingEnd: " << movingEnd << std::endl;
		std::cout << "MovingStart: " << movingStart << std::endl;
		std::cout << "FixedStart: " << fixedStart << std::endl;
		std::cout << "FixedCenter: " << fixedCenter << std::endl;

		// obtain the parameters from the transform
		RigidTransformType::ParametersType parameters = rigidTransform->GetParameters();	

		// This portion of code identifies where to perform the gross translation in the z direction.
		// The range is defined as the distance from the center of the fixed image to the location where
		// the moving image center would occur if the two images were bottom aligned. So in math terms
		// fixedCenter - fixedStart - movingRange/2 (all only for the z direction).
		
		int zCor = 5; // location in the transform corresponding to the proper translation
		int ySag = 4;
		
		// find range to move over in the coronal direction
		float rangeCor = abs( fixedCenter[2] - fixedStart[2] - abs( movingEnd[2] - movingStart[2] )/2.0 );
		std::cout << "Coronal Range: " << rangeCor << std::endl;
		float origParam5 = parameters[zCor];

		// find range to move over in the sagittal direction
		float rangeSag = abs( fixedCenter[1] - fixedStart[1] - abs( movingEnd[1] - movingStart[1] )/2.0 );
		std::cout << "Sagittal Range: " << rangeSag << std::endl;
		float origParam4 = parameters[ySag];

		// initialize parameters
		float minMetric = 10000000.0;
		RigidTransformType::ParametersType minParameters;

		// change the parameter corresponding to the z translation and obtain the metric
		std::cout << "\nMETRIC PARAMETERS" << std::endl;
		for( float zTrans = origParam5 - rangeCor - 10.0; zTrans < origParam5 + rangeCor + 10.0; zTrans = zTrans + rangeCor/20.0 )
		{
			// change z parameter
			parameters[zCor] = zTrans;
			// store parameters and metric value into array
			if( metric->GetValue(parameters) < minMetric )
			{
				minMetric = metric->GetValue(parameters);
				minParameters = parameters;
			}
			std::cout << metric->GetValue( parameters ) << " " << parameters << std::endl;
		}

		/*
		// reset everything
		minMetric = 10000000.0;
		rigidTransform->SetParameters( minParameters );
		parameters = rigidTransform->GetParameters();
		std::string filenameTran = "H:\\Results\\2015.04.01_UsingTextFileInput\\3DSpace_Ax\\FirstMetricInitTransform.tfm";
		WriteOutTransform< RigidTransformType >( filenameTran.c_str() , rigidTransform );

		// change parameter corresponding to y translation
		for( float yTrans = origParam4 - rangeSag; yTrans < origParam4 + rangeSag; yTrans = yTrans + rangeSag/5.0 )
		{
			// change y parameter
			parameters[ySag] = yTrans;
			// store parameters and metric value into array
			if( metric->GetValue(parameters) < minMetric )
			{
				minMetric = metric->GetValue(parameters);
				minParameters = parameters;
			}
			std::cout << metric->GetValue(parameters) << " " << parameters << std::endl;
		}*/

		// insert the new initialization parameters into the transform and print out
		rigidTransform->SetParameters( minParameters );
		WriteOutTransform< RigidTransformType >( inputs->InitMetricFilename().c_str() , rigidTransform );
	}

	memorymeter.Stop( "Initialization" );
	chronometer.Stop( "Initialization" );

	// ************************ OPTIMIZER ********************************
	//typedef itk::RegularStepGradientDescentOptimizer	RigidOptimizerType;
	typedef itk::VersorRigid3DTransformOptimizer		RigidOptimizerType;
	RigidOptimizerType::Pointer rigidOptimizer = RigidOptimizerType::New();
	// set parameters
	rigidOptimizer->SetMinimumStepLength( 0.001 );
	if( inputs->MaximumStepLength() > 0 )
	{
		rigidOptimizer->SetMaximumStepLength( inputs->MaximumStepLength() );
	}
	if( inputs->NumberOfIterations() > 0 )
	{
		rigidOptimizer->SetNumberOfIterations( inputs->NumberOfIterations() );
	}
	if( inputs->RelaxationFactor() > 0 )
	{
		rigidOptimizer->SetRelaxationFactor( inputs->RelaxationFactor() );
	}
	if( inputs->GradientMagnitudeTolerance() > 0 )
	{
		rigidOptimizer->SetGradientMagnitudeTolerance( inputs->GradientMagnitudeTolerance() );
	}
	rigidOptimizer->MinimizeOn();

	if( inputs->RotationScale() > 0 || inputs->TranslationScale() > 0 )//|| inputs->ScalingScale() > 0 )
	{
		// set optimizer scales
		RigidOptimizerType::ScalesType rigidOptScales( rigidTransform->GetNumberOfParameters() );

		// rotation
		rigidOptScales[0] = 1.0/inputs->RotationScale();
		rigidOptScales[1] = 1.0/inputs->RotationScale();
		rigidOptScales[2] = 1.0/inputs->RotationScale();
		// translation
		rigidOptScales[3] = 1.0/inputs->TranslationScale();
		rigidOptScales[4] = 1.0/inputs->TranslationScale();
		rigidOptScales[5] = 1.0/inputs->TranslationScale();
		// scaling
		//rigidOptScales[6] = 1.0/inputs->ScalingScale();
		//rigidOptScales[7] = 1.0/inputs->ScalingScale();
		//rigidOptScales[8] = 1.0/inputs->ScalingScale();

		// set the scales
		rigidOptimizer->SetScales( rigidOptScales );
	}

	// register the optimizer with the command class
	RigidCommandIterationUpdate::Pointer rigidObserver = RigidCommandIterationUpdate::New();
	rigidOptimizer->AddObserver( itk::IterationEvent(), rigidObserver );

	// ********************** REGISTRATION METHOD ***************************
	typedef itk::ImageRegistrationMethod< FloatImageType, FloatImageType >		RegistrationType;
	RegistrationType::Pointer registration = RegistrationType::New();
	// set components for rigid registration
	registration->SetMetric( metric );
	registration->SetOptimizer( rigidOptimizer );
	registration->SetTransform( rigidTransform );
	registration->SetInitialTransformParameters( rigidTransform->GetParameters() );
	registration->SetInterpolator( interpolator );
	// set images
	registration->SetFixedImage( fixedImage );
	registration->SetMovingImage( movingImage );
	registration->SetFixedImageRegion( fixedImage->GetBufferedRegion() );
	// perform registration
	std::cout << std::endl;
	std::cout << "Beginning Registration" << std::endl;
	std::cout << "Itr# StepSize MetricValue TransformParameters" << std::endl;
	try
	{
		memorymeter.Start( "Rigid registration" );
		chronometer.Start( "Rigid registration" );

		registration->Update();
		std::cout << "OptimizerStopCondition: " << registration->GetOptimizer()->GetStopConditionDescription() << std::endl;

		memorymeter.Stop( "Rigid registration" );
		chronometer.Stop( "Rigid registration" );
	}
	catch( itk::ExceptionObject & err )
	{
		std::cerr << "ExceptionObjectCaught!" << std::endl;
		std::cerr << err << std::endl;
		return EXIT_FAILURE;
	}

	// ********************** OUTPUTS ***************************
	memorymeter.Start( "Outputs" );
	chronometer.Start( "Outputs" );

	// print results to the screen
	/*
	std::cout << std::endl;
	std::cout << "\n***** OPTIMIZER PARAMETERS *****" << std::endl;
	std::cout << " Iterations      : " << rigidOptimizer->GetCurrentIteration() << std::endl;
	std::cout << " Metric value    : " << rigidOptimizer->GetValue() << std::endl;
	std::cout << " #histogram bins : " << metric->GetNumberOfHistogramBins() << std::endl;
	std::cout << " #spatial samples: " << registration->GetMetric()->GetNumberOfSpatialSamples() << std::endl;
	std::cout << " #fixed samples  : " << registration->GetMetric()->GetNumberOfFixedImageSamples() << std::endl;
	std::cout << " #moving samples : " << registration->GetMetric()->GetNumberOfMovingImageSamples() << std::endl;
	*/

	std::cout << std::endl;

	// write final rigid transform out to file
	// instantiate new transform and insert parameters
	RigidTransformType::Pointer finalRigidTransform = RigidTransformType::New();
	finalRigidTransform->SetParameters( registration->GetLastTransformParameters() );
	finalRigidTransform->SetFixedParameters( rigidTransform->GetFixedParameters() );
	WriteOutTransform< RigidTransformType >( inputs->RigidTransformFilename().c_str(), finalRigidTransform );

	std::cout << finalRigidTransform << std::endl;

	// obtain joint histogram
	typedef itk::Image< MetricType::PDFValueType, 2 >	JPDFImageType;
	typedef itk::RescaleIntensityImageFilter< JPDFImageType, CharImageType >	RescaleFilterType;
	RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
	rescaler->SetInput( metric->GetJointPDF() );
	rescaler->SetOutputMinimum( 0 );
	rescaler->SetOutputMaximum( 255 );
	
	// write out joint histogram
	WriteOutImage< CharImageType, CharImageType >( inputs->JointHistogramFilename().c_str(), rescaler->GetOutput() );

	std::cout << "\n***** RIGID TRANSFORM PARAMETERS *****" << std::endl;
	std::cout << " Raw Parameters: " << rigidTransform->GetParameters() << std::endl;
	std::cout << " Rotation: " << rigidTransform->GetVersor() << std::endl;
	std::cout << " Angle: " << rigidTransform->GetVersor().GetAngle() << std::endl;
	std::cout << " Translation: " << rigidTransform->GetTranslation() << std::endl;

	std::cout << std::endl;
	std::cout << rigidOptimizer << std::endl;

	// ********************** APPLY TRANSFORM ***************************
	if( inputs->WriteImage() )
	{
		// apply transform to image and write out result to file
		typedef itk::ResampleImageFilter< FloatImageType, FloatImageType >	ResampleFilterType;
		ResampleFilterType::Pointer resampler = ResampleFilterType::New();
		// initialize with fixed image parameters
		resampler->SetSize( fixedImage->GetLargestPossibleRegion().GetSize() );
		resampler->SetOutputOrigin( fixedImage->GetOrigin() );
		resampler->SetOutputSpacing( fixedImage->GetSpacing() );
		resampler->SetOutputDirection( fixedImage->GetDirection() );
		resampler->SetDefaultPixelValue( inputs->DefaultPixelValue() );
		// send inputs
		resampler->SetInput( movingImage );
		resampler->SetTransform( finalRigidTransform );

		// write out image
		WriteOutImage< FloatImageType, FloatImageType >( inputs->TransformedImageFilename().c_str(), resampler->GetOutput() );
	}

	memorymeter.Stop( "Outputs" );
	chronometer.Stop( "Outputs" );

	memorymeter.Stop( "Full program" );
	chronometer.Stop( "Full program" );

	std::cout << "\n***** TIME/MEMORY MONITORING *****" << std::endl;
	chronometer.Report( std::cout );
	memorymeter.Report( std::cout );

	return EXIT_SUCCESS;
}