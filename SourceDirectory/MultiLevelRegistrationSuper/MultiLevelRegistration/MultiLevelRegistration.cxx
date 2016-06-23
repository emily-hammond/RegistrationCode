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
	std::cout << "\n -> Transforms\n" << std::endl;
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
	if (numberOfROIs == 1)
	{
		std::vector< float >::iterator it = ROI.begin();
	}
	else if (numberOfROIs > 1)
	{
		std::vector<std::vector<float>>::iterator it = ROI.begin();
	}
	else if (numberOfROIs > numberOfLevels) 
	{ 
		std::cout << "Too many ROIs are specified" << std::endl; 
		return EXIT_FAILURE; 
	}
	else if ( numberOfROIs - numberOfLevels > 1)
	{
		std::cout << "Too few ROIs have been specified" << std::endl;
	}
	else
	{
		std::cout << "Strange things have happened with your ROIs" << std::endl;
	}

	// prepare the Registration framework
	itk::RegistrationFramework::Pointer registration = itk::RegistrationFramework::New();
	
	for (int NOL = 1; NOL < numberOfLevels, ++NOL)
	{
		// monitors
		std::string message = "Level " + NOL;
		chronometer.Start(message.c_str());
		memorymeter.Start(message.c_str());

		std::cout << "\n*********************************************" << std::endl;
		std::cout << "            REGISTRATION LEVEL " << NOL << "               " << std::endl;
		std::cout << "*********************************************\n" << std::endl;

		// apply transform from previous level and crop image
		transforms->ResampleImageOn();
		
		// insert appropriate ROI into 
		if (numberOfROIs == numberOfLevels)
		{

		}
		transforms->CropImageOn();
		
	}
	
	if( numberOfLevels > 0 )
	{
		// Registration level 1
		chronometer.Start( "Level 1" );
		memorymeter.Start( "Level 1" );

		// perform registration class
		itk::RegistrationFramework::Pointer level1Registration = itk::RegistrationFramework::New();

		// test functionality of itkRegistrationFramework.h
		std::cout << "\n*********************************************" << std::endl;
		std::cout << "            REGISTRATION LEVEL 1               " << std::endl;
		std::cout << "*********************************************\n" << std::endl;

		// create registration class
		level1Registration->SetFixedImage( fixedImage );
		level1Registration->SetMovingImage( movingImage );
		level1Registration->SetInitialTransform( initialTransform );
		level1Registration->SetNumberOfIterations( numberOfIterations );
		level1Registration->SetRelaxationFactor( relaxationFactor );
		level1Registration->SetMaximumStepLength( maximumStepLength );
		level1Registration->SetMinimumStepLength( minimumStepLength );
		level1Registration->SetGradientMagnitudeTolerance( gradientMagnitudeTolerance );
		level1Registration->SetRotationScale( rotationScale );
		level1Registration->SetTranslationScale( translationScale );
		level1Registration->SetScalingScale( scalingScale );
		if( observe ){ level1Registration->ObserveOn(); }
		
		try
		{
			level1Registration->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}
		
		level1Registration->Print();

		// add transform to composite transform in transforms class and apply to moving image
		transforms->AddTransform( level1Registration->GetFinalTransform() );

		// write out composite transform
		WriteOutTransform< itk::ManageTransformsFilter::CompositeTransformType >( finalTransform.c_str(), transforms->GetCompositeTransform() );

		// Registration level 1
		chronometer.Stop( "Level 1" );
		memorymeter.Stop( "Level 1" );
	}
	/*
	if( numberOfLevels > 1 )
	{
		// Registration level 2
		chronometer.Start( "Level 2" );
		memorymeter.Start( "Level 2" );


		// test functionality of itkRegistrationFramework.h
		std::cout << "\n*********************************************" << std::endl;
		std::cout << "            REGISTRATION LEVEL 2               " << std::endl;
		std::cout << "*********************************************\n" << std::endl;

		// crop images
		transforms->CropImageOn();
		transforms->ResampleImageOn();
		transforms->SetROI( ROI2 );
		try
		{
			transforms->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}

		// create new registration class
		itk::RegistrationFramework::Pointer level2Registration = itk::RegistrationFramework::New();
		level2Registration->SetFixedImage( transforms->GetFixedCroppedImage() );
		level2Registration->SetMovingImage( transforms->GetMovingCroppedImage() );
		level2Registration->SetNumberOfIterations( numberOfIterations );
		level2Registration->SetRelaxationFactor( relaxationFactor );
		level2Registration->SetMaximumStepLength( maximumStepLength/2.0 );
		level2Registration->SetMinimumStepLength( minimumStepLength );
		level2Registration->SetGradientMagnitudeTolerance( gradientMagnitudeTolerance );
		level2Registration->SetRotationScale( rotationScale/2.0 );
		level2Registration->SetTranslationScale( translationScale/2.0 );
		level2Registration->SetScalingScale( scalingScale/2.0 );


		if( observe ){ level2Registration->ObserveOn(); }

		try
		{
			level2Registration->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}
		level2Registration->Print();

		std::cout << "\n -> Transforms\n" << std::endl;
		// add transform to composite transform in transforms class and apply to moving image/label map image
		// don't update images here. Apply composite transform to the original moving image and label map
		transforms->AddTransform( level2Registration->GetFinalTransform() );

		// final composite transform
		WriteOutTransform< itk::ManageTransformsFilter::CompositeTransformType >( finalTransform.c_str(), transforms->GetCompositeTransform() );
	
		// Registration level 2
		chronometer.Stop( "Level 2" );
		memorymeter.Stop( "Level 2" );
	}

	if( numberOfLevels > 2 )
	{
		// Registration level 3
		chronometer.Start( "Level 3" );
		memorymeter.Start( "Level 3" );

		// test functionality of itkRegistrationFramework.h
		std::cout << "\n*********************************************" << std::endl;
		std::cout << "            REGISTRATION LEVEL 3               " << std::endl;
		std::cout << "*********************************************\n" << std::endl;

		// crop images
		transforms->CropImageOn();
		transforms->ResampleImageOn();
		transforms->SetROI( ROI3 );
		try
		{
			transforms->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}

		// create new registration class
		itk::RegistrationFramework::Pointer level3Registration = itk::RegistrationFramework::New();
		level3Registration->SetFixedImage( transforms->GetFixedCroppedImage() );
		level3Registration->SetMovingImage( transforms->GetMovingCroppedImage() );
		level3Registration->SetNumberOfIterations( numberOfIterations );
		level3Registration->SetRelaxationFactor( relaxationFactor );
		level3Registration->SetMaximumStepLength( maximumStepLength/4.0 );
		level3Registration->SetMinimumStepLength( minimumStepLength );
		level3Registration->SetGradientMagnitudeTolerance( gradientMagnitudeTolerance );
		level3Registration->SetRotationScale( rotationScale/4.0 );
		level3Registration->SetTranslationScale( translationScale/4.0 );
		level3Registration->SetScalingScale( scalingScale/4.0 );

		if( observe ){ level3Registration->ObserveOn(); }

		try
		{
			level3Registration->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}
		level3Registration->Print();

		std::cout << "\n -> Transforms\n" << std::endl;
		// add transform to composite transform in transforms class and apply to moving image/label map image
		// don't update images here. Apply composite transform to the original moving image and label map
		transforms->AddTransform( level3Registration->GetFinalTransform() );
		transforms->ResampleImageOn();
		transforms->CropImageOn();
		try
		{
			transforms->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}

		// write out final composite transform
		WriteOutTransform< itk::ManageTransformsFilter::CompositeTransformType >( finalTransform.c_str(), transforms->GetCompositeTransform() );
		
		// Registration level 3
		chronometer.Stop( "Level 3" );
		memorymeter.Stop( "Level 3" );
	}
	*/
	// full program
	chronometer.Stop( "Full program" );
	memorymeter.Stop( "Full program" );

	// print out time/memory results
	chronometer.Report( std::cout );
	memorymeter.Report( std::cout );

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
