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
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkVersorRigid3DTransformOptimizer.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkMultiResolutionImageRegistrationMethod.h"
#include "itkMultiResolutionPyramidImageFilter.h"

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

int main(int argc, char * argv[])
{
	// determine inputs
	const char * fixedImageFilename = argv[1];
	const char * movingImageFilename = argv[2];
	std::string outputDirectory = argv[3];

	// outputs
	std::string finalImageFilename = outputDirectory + "\\result.mhd";
	std::string finalTransformFilename = outputDirectory + "\\transform.mhd";

	// set up image types to be used
	typedef short	PixelType;
	const unsigned int Dimension = 3;

	typedef itk::Image< PixelType, Dimension >	FixedImageType;
	typedef itk::Image< PixelType, Dimension >	MovingImageType;

	// read in images
	FixedImageType::Pointer fixedImage = ReadInImage< FixedImageType >( fixedImageFilename );
	MovingImageType::Pointer movingImage = ReadInImage< MovingImageType >( movingImageFilename );

	// set up transform
	typedef itk::ScaleVersor3DTransform< double >	VersorTransformType;
	VersorTransformType::Pointer versorTransform = VersorTransformType::New();

	// set initial parameters
	typedef itk::CenteredTransformInitializer< VersorTransformType, FixedImageType, MovingImageType >	InitializerType;
	InitializerType::Pointer initializer = InitializerType::New();
	initializer->SetTransform( versorTransform );
	initializer->SetFixedImage( fixedImage );
	initializer->SetMovingImage( movingImage );
	initializer->GeometryOn();
	initializer->InitializeTransform();

	/*typedef VersorTransformType::VersorType	VersorType;
	typedef VersorTransformType::VectorType VectorType;
	VersorType	rotation;
	VectorType	axis;
	axis[0] = 0.0;
	axis[1] = 0.0;
	axis[2] = 0.0;
	rotation[0] = 0.0;
	rotation[1] = 0.0;
	rotation[2] = 0.0;
	versorTransform->SetRotation( rotation );
	versorTransform->SetTranslation( axis );*/

	//WriteOutTransform< VersorTransformType >( finalTransformFilename.c_str(), versorTransform );
	std::cout << "VersorTransform information: " << std::endl;
	std::cout << versorTransform->GetParameters() << std::endl;
	std::cout << "Rotation: " << versorTransform->GetVersor() << std::endl;
	std::cout << "Translation: " << versorTransform->GetTranslation() << std::endl;
	std::cout << "Scale: " << versorTransform->GetScale() << std::endl;
	std::cout << std::endl;

	//set up metric
	typedef itk::MeanSquaresImageToImageMetric< FixedImageType, MovingImageType >	MeanSquaresMetricType;
	MeanSquaresMetricType::Pointer meanSquaresMetric = MeanSquaresMetricType::New();
	meanSquaresMetric->SetNumberOfSpatialSamples( 10000 );

	// set up interpolator
	typedef itk::LinearInterpolateImageFunction< MovingImageType, double >	InterpolateType;
	InterpolateType::Pointer interpolator = InterpolateType::New();

	// set up optimizer
	typedef itk::VersorRigid3DTransformOptimizer	VersorOptimizerType;
	VersorOptimizerType::Pointer versorOptimizer = VersorOptimizerType::New();
	
	typedef VersorOptimizerType::ScalesType	OptimizerScalesType;
	OptimizerScalesType optimizerScales( versorTransform->GetNumberOfParameters() );
	const double translationScale = 1.0/1000.0;
	const double scalingScale = 1.0/100.0;
	optimizerScales[0] = 1.0;
	optimizerScales[1] = 1.0;
	optimizerScales[2] = 1.0;
	optimizerScales[3] = translationScale;
	optimizerScales[4] = translationScale;
	optimizerScales[5] = translationScale;
	optimizerScales[6] = scalingScale;
	optimizerScales[7] = scalingScale;
	optimizerScales[8] = scalingScale;
	versorOptimizer->SetScales( optimizerScales );
	versorOptimizer->SetMaximumStepLength( 0.2 );
	versorOptimizer->SetMinimumStepLength( 0.0001 );
	versorOptimizer->SetNumberOfIterations( 200 );

	// set up multiresolution scheme
	typedef itk::MultiResolutionPyramidImageFilter< FixedImageType, FixedImageType >	FixedImagePyramidType;
	FixedImagePyramidType::Pointer fixedPyramid = FixedImagePyramidType::New();
	fixedPyramid->SetStartingShrinkFactors( 8.0 );
	
	typedef itk::MultiResolutionPyramidImageFilter< MovingImageType, MovingImageType >	MovingImagePyramidType;
	MovingImagePyramidType::Pointer movingPyramid = MovingImagePyramidType::New();
	movingPyramid->SetStartingShrinkFactors( 8.0 );

	// plug into registration
	typedef itk::MultiResolutionImageRegistrationMethod< FixedImageType, MovingImageType >	RegistrationType;
	RegistrationType::Pointer registration = RegistrationType::New();

	registration->SetFixedImage( fixedImage );
	registration->SetFixedImagePyramid( fixedPyramid );
	registration->SetMovingImage( movingImage );
	registration->SetMovingImagePyramid( movingPyramid );

	registration->SetMetric( meanSquaresMetric );
	registration->SetOptimizer( versorOptimizer );
	registration->SetInterpolator( interpolator );
	registration->SetTransform( versorTransform );
	registration->SetInitialTransformParameters( versorTransform->GetParameters() );

	registration->SetNumberOfLevels( 1 );

	std::cout << std::endl;
	std::cout << "Fixed pyramid : " << fixedPyramid->GetSchedule() << std::endl;
	std::cout << "Moving pyramid: " << movingPyramid->GetSchedule() << std::endl;
	std::cout << "# of levels: " << registration->GetNumberOfLevels() << std::endl;
	std::cout << "	fixed: " << fixedPyramid->GetNumberOfLevels() << std::endl;
	std::cout << "  moving: " << movingPyramid->GetNumberOfLevels() << std::endl;
	std::cout << std::endl;

	std::cout << "Starting Registration! " << std::endl;
	try
	{
		registration->Update();
		std::cout << "Optimizer stop condition: " << registration->GetOptimizer()->GetStopConditionDescription() << std::endl;
	}
	catch( itk::ExceptionObject & err )
	{
		std::cerr << err << std::endl;
	}

	std::cout << registration->GetLastTransformParameters() << std::endl;

	return EXIT_SUCCESS;
}
