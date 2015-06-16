/*
Author: Emily Hammond
Date: 2014.11.12

Goal: The purpose of this program is to rigidly registration two CT images together

Pipeline:
	1. read in images
	2. set up components: MMI, linear interp., versor with scaling, versor optimizer
	3. set up multi-resolution
	4. perform registration

*/

// read/write image functions
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"

// registration components
#include "itkCenteredTransformInitializer.h"
#include "itkScaleVersor3DTransform.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
//#include "itkConjugateGradientOptimizer.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkMultiResolutionImageRegistrationMethod.h"
#include "itkMultiResolutionPyramidImageFilter.h"

// apply final transform
#include "itkResampleImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

// monitoring process
#include "itkCommand.h"

// implement a command observer to monitor the registration process
template<typename Registration>
class RegistrationInterfaceCommand : public itk::Command
{
public:
	typedef RegistrationInterfaceCommand	Self;
	typedef itk::Command					Superclass;
	typedef itk::SmartPointer<Self>			Pointer;
	itkNewMacro( Self );

protected:
	RegistrationInterfaceCommand() {};

public:
	typedef	Registration					RegistrationType;
	typedef RegistrationType *				RegistrationPointer;
	typedef itk::RegularStepGradientDescentOptimizer	OptimizerType;
	typedef OptimizerType *					OptimizerPointer;
	//typedef MetricType *					MetricPointer;
	//typedef TransformType *					TransformPointer;

	// implement the real Execute function
	// output the class name and object name
	void Execute( itk::Object * object, const itk::EventObject & event )
	{
		if( !(itk::IterationEvent().CheckEvent( &event )))
		{
			return;
		}

		// cast the object to a const object
		RegistrationPointer registration = dynamic_cast< RegistrationPointer >( object );

		// set up optimizer
		OptimizerPointer optimizer = dynamic_cast< OptimizerPointer >( registration->GetModifiableOptimizer() );

		// set up metric and transform
		//MetricPointer metric = dynamic_cast< MetricPointer >( registration->GetMetric() );
		//TransformPointer transform = dynamic_cast< TransformPointer >( registration->GetTransform() );
		
		if( registration->GetCurrentLevel() == 0 )
		{
			optimizer->SetMaximumStepLength( 1.0 );
			optimizer->SetMinimumStepLength( 0.001 );
		}
		else
		{
			optimizer->SetMaximumStepLength( optimizer->GetMaximumStepLength() * 0.5 );
			optimizer->SetMinimumStepLength( optimizer->GetMinimumStepLength() * 0.1 );
		}

		// print info out to screen
		std::cout << "==========================" << std::endl;
		std::cout << "Multiresolution Level: " << registration->GetCurrentLevel() << std::endl;
		std::cout << " Step size: [" << optimizer->GetMinimumStepLength() << ", " << optimizer->GetMaximumStepLength() << "]" << std::endl;
		std::cout << "start" << std::endl;
	}
	void Execute( const itk::Object *, const itk::EventObject & )
	{
		return;
	}
};

// implement a command observer to monitor the registration process
class CommandIterationUpdate : public itk::Command
{
public:
	typedef CommandIterationUpdate		Self;
	typedef itk::Command				Superclass;
	typedef itk::SmartPointer< Self >	Pointer;
	itkNewMacro( Self );

protected:
	CommandIterationUpdate() {};

public:
	typedef itk::RegularStepGradientDescentOptimizer	OptimizerType;
	typedef const OptimizerType *			OptimizerPointer;

	// write functions to output the details of the optimizer
	void Execute( itk::Object * object, const itk::EventObject & event )
	{
		Execute( (const itk::Object *) object, event );
	}

	void Execute( const itk::Object * object, const itk::EventObject & event )
	{
		OptimizerPointer optimizer = dynamic_cast< OptimizerPointer >( object );
		/*if( !(itk::IterationEvent().CheckEvent( &event )) );
		{
			return;
		}*/
		std::cout << optimizer->GetCurrentIteration() << " ";
		std::cout << optimizer->GetValue() << " ";
		std::cout << optimizer->GetCurrentPosition() << std::endl;
	}
};

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

		return EXIT_FAILURE;
	}

	std::cout << std::endl;
	std::cout << ImageFilename << " has been written to file!" << std::endl;
	
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

int main(int argc, char * argv[])
{
	// determine inputs
	std::string animalTag = argv[1];
	std::string movingTP = argv[2];
	std::string fixedTP = "1";

	// input files
	std::string basePath = "C:\\Experiments\\SPIEMhdFiles\\";
	std::string fixedImageFilename = basePath + animalTag + "\\T" + fixedTP + "\\T" + fixedTP + "_" + animalTag + ".mhd";;
	std::string movingImageFilename = basePath + animalTag + "\\T" + movingTP + "\\T" + movingTP + "_" + animalTag + ".mhd";;

	std::cout << "Fixed Image: " << fixedImageFilename << std::endl;
	std::cout << "Moving Image: " << movingImageFilename << std::endl;

	// outputs
	std::string outputDirectory = "C:\\Experiments\\SPIEMhdFiles\\" + animalTag + "\\Results";
	std::string finalImageFilename = outputDirectory + "\\TP" + movingTP + "result.mhd";
	std::string initialTransformFilename = outputDirectory + "\\TP" + movingTP + "initalTransform.mhd";
	std::string finalTransformFilename = outputDirectory + "\\TP" + movingTP + "finalTransform.mhd";
	std::string jointPDFFilename = outputDirectory + "\\TP" + movingTP + "jointPDF.tif";
	std::string fixedImageOutput = outputDirectory + "\\TP" + fixedTP + "result.mhd";
	//std::string movingImageOutput = outputDirectory + "\\movingImage.mhd";
	std::string initializedImageFilename = outputDirectory + "\\TP" + movingTP + "initializedImage.mhd";

	// set up image types to be used
	typedef short		PixelType;
	const unsigned int	Dimension = 3;
	
	// define images
	typedef itk::Image< PixelType, Dimension >	FixedImageType;
	typedef itk::Image< PixelType, Dimension >	MovingImageType;

	// define components of registration
	typedef itk::ScaleVersor3DTransform< double >	ScaleVersorTransformType;
	typedef itk::LinearInterpolateImageFunction< MovingImageType, double >	InterpolateType;
	typedef itk::RegularStepGradientDescentOptimizer			GDOptimizerType;
	typedef itk::MattesMutualInformationImageToImageMetric< FixedImageType, MovingImageType >	MMIMetricType;
	typedef itk::MultiResolutionImageRegistrationMethod< FixedImageType, MovingImageType >	RegistrationType;
	
	// initializer for the transform
	typedef itk::CenteredTransformInitializer< ScaleVersorTransformType, FixedImageType, MovingImageType >	InitializerType;
	
	// define the smoothing pyramids
	typedef itk::MultiResolutionPyramidImageFilter< FixedImageType, FixedImageType >	FixedImagePyramidType;
	typedef itk::MultiResolutionPyramidImageFilter< MovingImageType, MovingImageType >	MovingImagePyramidType;
	
	// instantiate registration methods
	ScaleVersorTransformType::Pointer scaleVersorTransform = ScaleVersorTransformType::New();
	GDOptimizerType::Pointer gdOptimizer = GDOptimizerType::New();
	InterpolateType::Pointer interpolator = InterpolateType::New();
	RegistrationType::Pointer registration = RegistrationType::New();
	MMIMetricType::Pointer mmiMetric = MMIMetricType::New();

	// instantiate initializer
	InitializerType::Pointer initializer = InitializerType::New();

	// instantiate pyramids
	FixedImagePyramidType::Pointer fixedPyramid = FixedImagePyramidType::New();
	MovingImagePyramidType::Pointer movingPyramid = MovingImagePyramidType::New();

	// define components of registration
	registration->SetOptimizer( gdOptimizer );
	registration->SetTransform( scaleVersorTransform );
	registration->SetInterpolator( interpolator );
	registration->SetMetric( mmiMetric );
	registration->SetFixedImagePyramid( fixedPyramid );
	registration->SetMovingImagePyramid( movingPyramid );

	// read in images
	FixedImageType::Pointer fixedImage = ReadInImage< FixedImageType >( fixedImageFilename.c_str() );
	MovingImageType::Pointer movingImage = ReadInImage< MovingImageType >( movingImageFilename.c_str() );

	WriteOutImage< FixedImageType, FixedImageType >( fixedImageOutput.c_str(), fixedImage );
	//WriteOutImage< MovingImageType, MovingImageType >( movingImageOutput.c_str(), movingImage );

	// put images into registration
	registration->SetFixedImage( fixedImage );
	registration->SetMovingImage( movingImage );
	registration->SetFixedImageRegion( fixedImage->GetBufferedRegion() );

	// set initial parameters
	initializer->SetTransform( scaleVersorTransform );
	initializer->SetFixedImage( fixedImage );
	initializer->SetMovingImage( movingImage );
	initializer->GeometryOn();
	initializer->InitializeTransform();

	// input result into registration
	registration->SetInitialTransformParameters( scaleVersorTransform->GetParameters() );

	// write out initial parameters
	std::cout << std::endl;
	std::cout << "scaleVersorTransform information: " << std::endl;
	std::cout << "Rotation: " << scaleVersorTransform->GetVersor() << std::endl;
	std::cout << "Translation: " << scaleVersorTransform->GetTranslation() << std::endl;
	std::cout << "Scale: " << scaleVersorTransform->GetScale() << std::endl;
	std::cout << std::endl;

	// apply initial transform to image
	typedef itk::ResampleImageFilter< MovingImageType, MovingImageType >	ResampleInitialImageType;
	ResampleInitialImageType::Pointer initialResampler = ResampleInitialImageType::New();

	initialResampler->SetTransform( scaleVersorTransform );
	initialResampler->SetInput( movingImage );
	initialResampler->SetSize( fixedImage->GetLargestPossibleRegion().GetSize() );
	initialResampler->SetOutputOrigin( fixedImage->GetOrigin() );
	initialResampler->SetOutputSpacing( fixedImage->GetSpacing() );
	initialResampler->SetOutputDirection( fixedImage->GetDirection() );
	initialResampler->Update();

	//WriteOutImage< MovingImageType, MovingImageType >( initializedImageFilename.c_str(), initialResampler->GetOutput() );

	//set up metric
	mmiMetric->SetNumberOfSpatialSamples( 100000 );
	mmiMetric->SetNumberOfHistogramBins( 128 );
	mmiMetric->ReinitializeSeed( 19900802 );

	// set up optimizer
	gdOptimizer->SetNumberOfIterations( 1000 );
	gdOptimizer->SetRelaxationFactor( 0.9 );
	gdOptimizer->SetMinimumStepLength( 0.001 );
	gdOptimizer->SetMaximumStepLength( 5.0 );
	gdOptimizer->SetGradientMagnitudeTolerance( 0.001 );

	// initialize optimizer scales
	typedef GDOptimizerType::ScalesType	OptimizerScalesType;
	OptimizerScalesType optimizerScales( scaleVersorTransform->GetNumberOfParameters() );
	// rotation
	const double rotationScale = 1.0/0.01;
	optimizerScales[0] = rotationScale;
	optimizerScales[1] = rotationScale;
	optimizerScales[2] = rotationScale;
	// translation
	const double translationScale = 1.0/50.0;
	optimizerScales[3] = translationScale;
	optimizerScales[4] = translationScale;
	optimizerScales[5] = translationScale;
	// scaling
	const double scalingScale = 1.0/0.01;
	optimizerScales[6] = scalingScale;
	optimizerScales[7] = scalingScale;
	optimizerScales[8] = scalingScale;
	// set scales
	gdOptimizer->SetScales( optimizerScales );


	// register command observer with optimizer
	CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
	gdOptimizer->AddObserver( itk::IterationEvent(), observer );

	// create an instance of the command and connect with registration object
	typedef RegistrationInterfaceCommand< RegistrationType > CommandType;
	CommandType::Pointer command = CommandType::New();
	registration->AddObserver( itk::IterationEvent(), command );

	// set the number of levels
	registration->SetNumberOfLevels( 1 );
	unsigned int shrinkFactors[3];
	shrinkFactors[0] = 1;
	shrinkFactors[1] = 1;
	shrinkFactors[2] = 1;
	fixedPyramid->SetStartingShrinkFactors( shrinkFactors );
	movingPyramid->SetStartingShrinkFactors( shrinkFactors );
	fixedPyramid->SetNumberOfLevels( 1 );
	movingPyramid->SetNumberOfLevels( 1 );

	std::cout << std::endl;
	std::cout << "Fixed pyramid : " << fixedPyramid->GetSchedule() << std::endl;
	std::cout << "Moving pyramid: " << movingPyramid->GetSchedule() << std::endl;
	std::cout << "# of levels: " << registration->GetNumberOfLevels() << std::endl;
	std::cout << "  fixed: " << fixedPyramid->GetNumberOfLevels() << std::endl;
	std::cout << "  moving: " << movingPyramid->GetNumberOfLevels() << std::endl;
	std::cout << std::endl;

	std::cout << "Starting Registration! " << std::endl;
	try
	{
		registration->Update();
		std::cout << "Optimizer stop condition: ";
		std::cout << registration->GetOptimizer()->GetStopConditionDescription() << std::endl;
	}
	catch( itk::ExceptionObject & err )
	{
		std::cout << "ExceptionObject caught !" << std::endl;
		std::cerr << err << std::endl;
		return EXIT_FAILURE;
	}

	// apply final transform to image
	typedef itk::ResampleImageFilter< MovingImageType, MovingImageType >	ResampleImageType;
	ResampleImageType::Pointer resampler = ResampleImageType::New();

	resampler->SetTransform( scaleVersorTransform );
	resampler->SetInput( movingImage );
	resampler->SetSize( fixedImage->GetLargestPossibleRegion().GetSize() );
	resampler->SetOutputOrigin( fixedImage->GetOrigin() );
	resampler->SetOutputSpacing( fixedImage->GetSpacing() );
	resampler->SetOutputDirection( fixedImage->GetDirection() );
	resampler->Update();
	
	WriteOutImage< MovingImageType, MovingImageType >( finalImageFilename.c_str(), resampler->GetOutput() );
	//WriteOutImage< MMIMetricType::JointPDFType, MovingImageType >( jointPDFFilename.c_str(), mmiMetric->GetJointPDF() );

	// output final parameters
	std::cout << std::endl;
	std::cout << "scaleVersorTransform information: " << std::endl;
	std::cout << "   Raw Parameters: " << scaleVersorTransform->GetParameters() << std::endl;
	std::cout << "   Rotation: " << scaleVersorTransform->GetVersor() << std::endl;
	std::cout << "   Angle: " << scaleVersorTransform->GetVersor().GetAngle() << std::endl;
	std::cout << "   Translation: " << scaleVersorTransform->GetTranslation() << std::endl;
	std::cout << "   Scale: " << scaleVersorTransform->GetScale() << std::endl;
	std::cout << "gradient descent optimizer: " << std::endl;
	std::cout << "   Iterations: " << gdOptimizer->GetCurrentIteration() << std::endl;
	std::cout << "   Metric value: " << gdOptimizer->GetValue();
	std::cout << std::endl;

	return EXIT_SUCCESS;
}
