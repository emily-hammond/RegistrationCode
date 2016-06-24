/*
INSERT COMMENTS HERE
*/

// include files
#include "C:\Users\ehammond\Documents\RegistrationCode\SourceDirectory\MultiLevelRegistrationSuper\ReadWriteFunctions.hxx"
#include "C:\Users\ehammond\Documents\RegistrationCode\SourceDirectory\MultiLevelRegistrationSuper\itkRegistrationFramework.h"
#include "C:\Users\ehammond\Documents\RegistrationCode\SourceDirectory\MultiLevelRegistrationSuper\itkInitializationFilter.h"
#include "C:\Users\ehammond\Documents\RegistrationCode\SourceDirectory\MultiLevelRegistrationSuper\itkValidationFilter.h"
#include "C:\Users\ehammond\Documents\RegistrationCode\SourceDirectory\MultiLevelRegistrationSuper\itkManageTransformsFilter.h"

// rescale images
#include "itkRescaleIntensityImageFilter.h"
#include "itkPluginUtilities.h"

// monitoring
#include "itkTimeProbesCollectorBase.h"
#include "itkMemoryProbesCollectorBase.h"
#include <time.h>
#include <stdio.h>
#include <vector>

#include <windows.h>
#include "MultiLevelRegistrationCLP.h"

// declare function
void PrintOutManual();
void Timestamp();

int main( int argc, char * argv[] )
{
	// parse through inputs 
	PARSE_ARGS;

	// print out start
	std::cout << "-----------------------------------------------------------------------------" << std::endl;
	std::cout << "             MULTI-LEVEL REGISTRATION ";
	Timestamp();
	std::cout << "-----------------------------------------------------------------------------" << std::endl;
	std::cout << std::endl;

	// probes (time/memory)
	itk::TimeProbesCollectorBase	chronometer;
	itk::MemoryProbesCollectorBase	memorymeter;

	// full program
	chronometer.Start( "Full program" );
	memorymeter.Start( "Full program" );
	
	// instantiate image and transform types
	typedef itk::Image< short, 3 >	ImageType;
	typedef itk::Image< unsigned char, 3 >	MaskImageType;
	typedef itk::ScaleVersor3DTransform< double >	TransformType;
	typedef itk::AffineTransform< double >	AffineTransformType;

	// manage transforms/cropping/applying class
	itk::ManageTransformsFilter::Pointer transforms = itk::ManageTransformsFilter::New();

	// fixed and moving images
	ImageType::Pointer fixedImage = ImageType::New();
	ImageType::Pointer movingImage = ReadInImage< ImageType >(movingImageFilename.c_str());

	// apply fixed initial transform if given
	if( !fixedImageInitialTransform.empty() )
	{
		// apply transform to fixed image
		TransformType::Pointer initialFixedTransform = ReadInTransform< TransformType >( fixedImageInitialTransform.c_str() );
		ImageType::Pointer fixedImageTemp = ReadInImage< ImageType >( fixedImageFilename.c_str() );
		transforms->SetFixedImage( ReadInImage< ImageType >(referenceImage.c_str()) );
		fixedImage = transforms->ResampleImage( fixedImageTemp, initialFixedTransform );
		std::cout << "Initial transform applied to fixed image." << std::endl;
		std::cout << "Moving image read in." << std::endl;
	}
	else
	{
		fixedImage = ReadInImage< ImageType >( fixedImageFilename.c_str() );
		std::cout << "Fixed image read in." << std::endl;
		std::cout << "Moving image read in." << std::endl;
	}

	// initialization
	chronometer.Start( "Initialization" );
	memorymeter.Start( "Initialization" );

	// initialization with validation
	std::cout << "\n*********************************************" << std::endl;
	std::cout << "              INITIALIZATION                 " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	// perform initialization class
	itk::InitializationFilter::Pointer initialize = itk::InitializationFilter::New();
	TransformType::Pointer initialTransform = TransformType::New();
	initialize->SetFixedImage( fixedImage );
	initialize->SetMovingImage( movingImage );
	
	if( manualInitialTransformFilename.empty() )
	{
		// turn on flags if no predefined transform
		if( observe ){ initialize->ObserveOn(); }
		if( centerOfGeometry ){ initialize->CenteredOnGeometryOn(); }
		if( metricX ){ initialize->MetricAlignmentOn( 0 ); }
		if( metricY ){ initialize->MetricAlignmentOn( 1 ); }
		if( metricZ ){ initialize->MetricAlignmentOn( 2 ); }
		initialize->Update();
		initialTransform = initialize->GetTransform();
		std::cout << "Initialization complete." << std::endl;
	}
	else
	{
		initialize->Update( ReadInTransform< AffineTransformType >( manualInitialTransformFilename.c_str() ) );
		initialTransform = initialize->GetTransform();
		std::cout << "Initialization complete via predefined transform." << std::endl;
	}

	std::cout << "\nFinal Parameters" << std::endl;
	std::cout << "Transform" << std::endl;
	std::cout << "  Translation   : " << initialTransform->GetTranslation() << std::endl;
	std::cout << "  Rotation      : " << initialTransform->GetVersor().GetAngle() << std::endl;
	std::cout << std::endl;

	// set up transforms class and insert fixed image (will not change)
	//transforms->AddTransform( initialize->GetTransform() );
	transforms->SetInitialTransform( initialTransform );
	transforms->SetFixedImage( fixedImage );
	transforms->SetMovingImage( movingImage );

	// write out initial transform
	WriteOutTransform< TransformType >(finalTransform.c_str(), initialTransform);

	// initialization
	chronometer.Stop( "Initialization" );
	memorymeter.Stop( "Initialization" );

	// determine the number of ROIs and creating iterators
	int numberOfROIs = ROI.size();
	bool ROI1 = false;

	std::vector< std::vector< float> >::iterator it = ROI.begin();
	if (numberOfLevels - numberOfROIs == 1)
	{
		ROI1 = false;
	}
	else if (numberOfLevels - numberOfROIs == 0)
	{
		ROI1 = true;
	}
	else 
	{
		std::cout << "Mismatched number of ROIs specified." << std::endl;
		return EXIT_FAILURE;
	}

	// prepare the Registration framework
	itk::RegistrationFramework::Pointer registration = itk::RegistrationFramework::New();
	
	for (int level = 1; level < numberOfLevels+1;  ++level)
	{
		// monitors
		std::string message = "Level " + level;
		chronometer.Start(message.c_str());
		memorymeter.Start(message.c_str());

		std::cout << "\n*********************************************" << std::endl;
		std::cout << "            REGISTRATION LEVEL " << level << "               " << std::endl;
		std::cout << "*********************************************\n" << std::endl;

		// apply transform from previous level 
		transforms->ResampleImageOn();
		
		// insert appropriate ROI into transforms class and crop image
		if ( level == 1 && ROI1 ) // if it is level 1 and ROI is to be used in Level 1
		{
			std::cout << "Applying ROI at level 1" << std::endl;

			transforms->SetROI(*it);
			transforms->CropImageOn();

			it++;
			// insert images into registration class
			try
			{
				transforms->Update();
			}
			catch (itk::ExceptionObject & err)
			{
				std::cerr << "Exception Object Caught!" << std::endl;
				std::cerr << err << std::endl;
				std::cerr << std::endl;
				return EXIT_FAILURE;
			}
			registration->SetFixedImage(transforms->GetFixedCroppedImage());
			registration->SetMovingImage(transforms->GetMovingCroppedImage());
		}
		else if (level != 1) // if it is not level 1
		{
			transforms->SetROI(*it);
			transforms->CropImageOn();

			std::vector<float>::iterator jt = (*it).begin();

			it++;
			// insert images into registration class
			try
			{
				transforms->Update();
			}
			catch (itk::ExceptionObject & err)
			{
				std::cerr << "Exception Object Caught!" << std::endl;
				std::cerr << err << std::endl;
				std::cerr << std::endl;
				return EXIT_FAILURE;
			}
			registration->SetFixedImage(transforms->GetFixedCroppedImage());
			registration->SetMovingImage(transforms->GetMovingCroppedImage());
		}
		else // ROI is not applied at level
		{
			std::cout << "ROI not applied at level " << level << std::endl;
			// insert images into registration class
			try
			{
				transforms->Update();
			}
			catch (itk::ExceptionObject & err)
			{
				std::cerr << "Exception Object Caught!" << std::endl;
				std::cerr << err << std::endl;
				std::cerr << std::endl;
				return EXIT_FAILURE;
			}
			registration->SetFixedImage( fixedImage );
			registration->SetMovingImage(transforms->GetTransformedImage());
		}

		// insert parameters into registration
		registration->SetNumberOfIterations(numberOfIterations);
		registration->SetRelaxationFactor(relaxationFactor);
		registration->SetMaximumStepLength(maximumStepLength);
		registration->SetMinimumStepLength(minimumStepLength);
		registration->SetGradientMagnitudeTolerance(gradientMagnitudeTolerance);
		registration->SetRotationScale(rotationScale);
		registration->SetTranslationScale(translationScale);
		registration->SetScalingScale(scalingScale);

		// observe process
		if (observe) { registration->ObserveOn(); }

		// perform registration
		try
		{
			registration->Update();
		}
		catch (itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}

		// print results
		registration->Print();

		// add transform to transforms class
		transforms->AddTransform(registration->GetFinalTransform());

		// write out composite transform
		WriteOutTransform< itk::ManageTransformsFilter::CompositeTransformType >(finalTransform.c_str(), transforms->GetCompositeTransform());

		// Registration level 1
		chronometer.Stop(message.c_str());
		memorymeter.Stop(message.c_str());
	}
	
	// full program
	chronometer.Stop( "Full program" );
	memorymeter.Stop( "Full program" );

	// print out time/memory results
	//chronometer.Report( std::cout );
	//memorymeter.Report( std::cout );

	return EXIT_SUCCESS;
}

void Timestamp()
{
	time_t ltime;
	ltime = time( NULL );
	printf("%s",asctime( localtime( &ltime ) ) );
	return;
}

void PrintOutManual()
{
	std::cout << "                           **** USER MANUAL ****                             " << std::endl;
	std::cout << std::endl;

	std::cout << " IMAGES: " << std::endl;
	std::cout << "  outputDirectory: path of the directory to save the results | required" << std::endl;
	std::cout << "  fixedImageFilename: path to the fixed image | required" << std::endl;
	std::cout << "  movingImageFilename: path to the moving image | required" << std::endl;
	std::cout << "  [fixedValidationMaskFilename]: path to the label map corresponding to \n";
	std::cout << "                               the fixed image for validation | required" << std::endl;
	std::cout << "  [movingValidationMaskFilename]: path to the label map corresponding to \n";
	std::cout << "                                the moving image for validation | required" << std::endl;
	std::cout << std::endl;

	std::cout << " MULTI-LEVEL: " << std::endl;
	std::cout << "  [numberOfLevels]: number of desired levels for registration | default = 0" << std::endl;
	std::cout << "          = 0 | initialization only " << std::endl;
	std::cout << "          = 1 | whole image registration " << std::endl;
	std::cout << "          = 2 | ROI used to isolate a specific region for registration \n";
	std::cout << "                (must be smaller than the whole image) " << std::endl;
	std::cout << "          = 3 | an additional ROI used to pair down the process even \n";
	std::cout << "                further (must be within the 2nd level ROI) " << std::endl;
	std::cout << "  [level2ROIFilename]: path to the ROI used in the second level | required \n";
	std::cout << "                       if numberOfLevels >= 2" << std::endl;
	std::cout << "  [level3ROIFilename]: path to the ROI used in the third level | required \n";
	std::cout << "                       if numberOfLevels = 3" << std::endl;
	std::cout << "  [observe]: print out the monitoring of the registration process at each \n";
	std::cout << "             level | default = 0" << std::endl;
	std::cout << "          = 0 | YES " << std::endl;
	std::cout << "          = 1 | NO " << std::endl;
	std::cout << std::endl;

	std::cout << " INITIALIZATION: " << std::endl;
	std::cout << "  [manualInitialTransform]: initial transform filename as a manual input" << std::endl;
	std::cout << "                            | default = NULL" << std::endl;
	std::cout << "  [center]: perform center of geomentry initialization | default = 1" << std::endl;
	std::cout << "          = 0 | YES " << std::endl;
	std::cout << "          = 1 | NO " << std::endl;
	std::cout << "  [metricX]: perform metric initialization in the x axis | default = 0" << std::endl;
	std::cout << "          = 0 | YES " << std::endl;
	std::cout << "          = 1 | NO " << std::endl;
	std::cout << "  [metricY]: perform metric initialization in the y axis | default = 0" << std::endl;
	std::cout << "          = 0 | YES " << std::endl;
	std::cout << "          = 1 | NO " << std::endl;
	std::cout << "  [metricZ]: perform metric initialization in the z axis | default = 0" << std::endl;
	std::cout << "          = 0 | YES " << std::endl;
	std::cout << "          = 1 | NO " << std::endl;
	std::cout << std::endl;

	std::cout << " REGISTRATION PARAMETERS: " << std::endl;
	std::cout << "  [rotationScale]: expected amount of rotation to occur | default = 0.001" << std::endl;
	std::cout << "  [translationScale]: expected amount of translation to occur | default = 10" << std::endl;
	std::cout << "  [scalingScale]: expected amount of scaling to occur | default = 0.001" << std::endl;
	std::cout << "  [numberOfIterations]: number of iterations allowed at each level of \n";
	std::cout << "                        registration | default = 500" << std::endl;
	std::cout << "  [maximumStepLength]: maximum step length allowed at each level | default = 1" << std::endl;
	std::cout << "  [minimumStepLength]: minimum step length allowed at each level | default = 0.001" << std::endl;
	std::cout << "  [relaxationFactor]: amount by which the step length decreases during \n";
	std::cout << "                      optimization | default = 0.5" << std::endl;
	std::cout << "  [gradientMagnitudeTolerance]: the minimum gradient magnitude \n";
	std::cout << "                                allowed | default = 0.001" << std::endl;
	std::cout << std::endl;

	std::cout << " SPECIAL PARAMETERS: " << std::endl;
	std::cout << "  [skipWB]: skip the first level of registration and begin with application" << std::endl;
	std::cout << "            of the second level ROI after initialization | default = 0" << std::endl;
	std::cout << "  [debug]: print out images are at each level | default = 0" << std::endl;
	std::cout << "  [initialFixedTransform]: transform that is applied to the fixed image prior" << std::endl;
	std::cout << "                           to registration | default = NULL" << std::endl;
	std::cout << "  [referenceImage]: image required if there is an initial fixed transform to" << std::endl;
	std::cout << "                    define the resampled fixed image | default = NULL" << std::endl;
	std::cout << std::endl;
	
	return;
}
