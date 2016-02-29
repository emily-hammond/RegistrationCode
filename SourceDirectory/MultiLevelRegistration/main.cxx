/*
INSERT COMMENTS HERE
*/

// include files
#include "ReadWriteFunctions.hxx"
#include "itkRegistrationFramework.h"
#include "itkInitializationFilter.h"
#include "itkValidationFilter.h"
#include "itkManageTransformsFilter.h"

// monitoring
#include "itkTimeProbesCollectorBase.h"
#include "itkMemoryProbesCollectorBase.h"
#include <time.h>
#include <stdio.h>

// declare function
void PrintOutManual();
void timestamp();

int main( int argc, char * argv[] )
{
	std::cout << "-----------------------------------------------------------------------------" << std::endl;
	std::cout << "             MULTI-LEVEL REGISTRATION ";
	timestamp();
	std::cout << "-----------------------------------------------------------------------------" << std::endl;

	for( int i = 0; i < argc; ++i )
	{
		std::cout << argv[i] << std::endl;
	}

	// probes
	itk::TimeProbesCollectorBase	chronometer;
	itk::MemoryProbesCollectorBase	memorymeter;

	// full program
	chronometer.Start( "Full program" );
	memorymeter.Start( "Full program" );

	// inputs
	chronometer.Start( "Inputs" );
	memorymeter.Start( "Inputs" );

	// required inputs
	char * fixedImageFilename;
	char * movingImageFilename;
	char * fixedValidationMaskFilename;
	char * movingValidationMaskFilename;
	std::string outputDirectory;

	// multi-level
	int numberOfLevels = 0;
	char * level2ROIFilename;
	char * level3ROIFilename;
	int observe = 0;

	// initialization
	int center = 1;
	int metricX = 0;
	int metricY = 0;
	int metricZ = 0;

	// other inputs instantiation defaults
	int numberOfIterations = 500;
	float maximumStepLength = 1;
	float relaxationFactor = 0.5;
	float gradientMagnitudeTolerance = 0.001; 
	float rotationScale = 0.001; 
	float translationScale = 10;
	float scalingScale = 0.001;

	// minimum input
	if( argc < 6 )
	{
		PrintOutManual();
		return EXIT_FAILURE;
	}
	else
	{
		//desired inputs
		fixedImageFilename = argv[1];
		movingImageFilename = argv[2];
		outputDirectory = argv[3];
		fixedValidationMaskFilename = argv[4];
		movingValidationMaskFilename = argv[5];
		std::cout << "images" << std::endl;
	}

	// number of levels
	if( argc > 7 )
	{
		numberOfLevels = atoi( argv[6] );
		if( argc < 8 )
		{
			std::cout << "Please insert a region of interest filename." << std::endl;
			return EXIT_FAILURE;
		}
		else
		{
			level2ROIFilename = argv[7];
		}
	}
	if( argc > 8 && numberOfLevels > 2 )
	{
		level3ROIFilename = argv[8];
	}
	if( numberOfLevels > 3 )
	{
		std::cout << "Maximum number of levels is 3" << std::endl;
		return EXIT_FAILURE;
	}

	// observation
	if( argc > 9 ){ observe = atoi( argv[9] ); }

	// initialization
	if( argc > 10 ){ center = atoi( argv[10] ); }
	if( argc > 11 ){ metricX = atoi( argv[11] ); }
	if( argc > 12 ){ metricY = atoi( argv[12] ); }
	if( argc > 13 ){ metricZ = atoi( argv[13] ); }

	// registration parameters
	if( argc > 14 ){ rotationScale = atof( argv[14] ); }
	if( argc > 15 ){ translationScale = atof( argv[15] ); }
	if( argc > 16 ){ scalingScale = atof( argv[16] ); }
	if( argc > 17 ){ numberOfIterations = atoi( argv[17] ); }
	if( argc > 18 ){ maximumStepLength = atof( argv[18] ); }
	if( argc > 19 ){ relaxationFactor = atof( argv[19] ); }
	if( argc > 20 ){ gradientMagnitudeTolerance = atof( argv[20] ); }
	if( argc > 21 )
	{ 
		std::cout << "Too many inputs" << std::endl;
		return EXIT_FAILURE;
	}

	// instantiate image type
	typedef itk::Image< unsigned short, 3 >	ImageType;
	typedef itk::Image< unsigned char, 3 >	MaskImageType;
	typedef itk::ScaleVersor3DTransform< double >	TransformType;

	// check inputs
	// read in necessary images
	ImageType::Pointer fixedImage = ReadInImage< ImageType >( fixedImageFilename );
	ImageType::Pointer movingImage = ReadInImage< ImageType >( movingImageFilename );
	ImageType::Pointer fixedValidationMask = ReadInImage< ImageType >( fixedValidationMaskFilename );
	ImageType::Pointer movingValidationMask = ReadInImage< ImageType >( movingValidationMaskFilename );
	//TransformType::Pointer initialTransform = ReadInTransform< TransformType >( initialTransformFilename );

	std::cout << "\nFixed image           : " << fixedImageFilename << std::endl;
	std::cout << "Fixed validation mask : " << fixedValidationMaskFilename << std::endl;
	std::cout << "Moving image          : " << movingImageFilename << std::endl;
	std::cout << "Moving validation mask: " << movingValidationMaskFilename << std::endl;
	if( argc > 7 ){ std::cout << "Level 2 ROI filename  : " << level2ROIFilename << std::endl; }
	if( argc > 8 ){	std::cout << "Level 3 ROI filename  : " << level3ROIFilename << std::endl; }

	// inputs
	chronometer.Stop( "Inputs" );
	memorymeter.Stop( "Inputs" );

	// initialization
	chronometer.Start( "Initialization" );
	memorymeter.Start( "Initialization" );

	// initialization with validation
	std::cout << "\n*********************************************" << std::endl;
	std::cout << "              INITIALIZATION                 " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	itk::InitializationFilter::Pointer initialize = itk::InitializationFilter::New();
	initialize->SetFixedImage( fixedImage );
	initialize->SetMovingImage( movingImage );
	//initialize->ObserveOn();
	if( center ){ initialize->CenteredOnGeometryOn(); }
	if( metricX ){ initialize->MetricAlignmentOn( 0 ); }
	if( metricY ){ initialize->MetricAlignmentOn( 1 ); }
	if( metricZ ){ initialize->MetricAlignmentOn( 2 ); }
	initialize->Update();

	// set up transforms class and insert fixed image (will not change)
	itk::ManageTransformsFilter::Pointer transforms = itk::ManageTransformsFilter::New();
	//transforms->AddTransform( initialize->GetTransform() );
	transforms->SetInitialTransform( initialize->GetTransform() );
	transforms->SetFixedImage( fixedImage );

	// set up validation class and insert fixed image as image set 1 (will not change)
	itk::ValidationFilter::Pointer validation = itk::ValidationFilter::New();
	// insert image set 1 (this is the fixed image and will not change)
	validation->SetImage1( fixedImage );
	validation->SetLabelMap1( fixedValidationMask );

	// apply initial transform to the moving image and mask
	transforms->SetMovingImage( movingImage );
	transforms->SetMovingLabelMap( movingValidationMask );
	
	// perform validation
	validation->SetImage2( transforms->ResampleImage( movingImage, initialize->GetTransform() ) );
	transforms->NearestNeighborInterpolateOn();
	validation->SetLabelMap2( transforms->ResampleImage( movingValidationMask, initialize->GetTransform() ) );
	transforms->NearestNeighborInterpolateOff();
	validation->LabelOverlapMeasuresOn();
	try
		{
			validation->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}

	// write out initial transform and add to composite transform
	std::string initialTransformFilename = outputDirectory + "\\InitialTransform.tfm";
	WriteOutTransform< TransformType >( initialTransformFilename.c_str(), initialize->GetTransform() );
	//transforms->AddTransform( initialize->GetTransform() );

	// initialization
	chronometer.Stop( "Initialization" );
	memorymeter.Stop( "Initialization" );

	if( numberOfLevels > 0 )
	{
		// Registration level 1
		chronometer.Start( "Level 1" );
		memorymeter.Start( "Level 1" );

		itk::RegistrationFramework::Pointer level1Registration = itk::RegistrationFramework::New();

		// test functionality of itkRegistrationFramework.h
		std::cout << "\n*********************************************" << std::endl;
		std::cout << "            REGISTRATION LEVEL 1               " << std::endl;
		std::cout << "*********************************************\n" << std::endl;

		std::cout << "\n -> Registration\n" << std::endl;
		// create registration class
		level1Registration->SetFixedImage( fixedImage );
		level1Registration->SetMovingImage( movingImage );
		level1Registration->SetInitialTransform( initialize->GetTransform() );
		level1Registration->SetNumberOfIterations( numberOfIterations );
		level1Registration->SetMaximumStepLength( maximumStepLength );
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


		std::cout << "\n -> Transforms\n" << std::endl;
		// add transform to composite transform in transforms class and apply to moving image
		transforms->AddTransform( level1Registration->GetFinalTransform() );
		//transforms->SetTransform( transforms->GetCompositeTransform() );
		transforms->ResampleImageOn();
		transforms->CropImageOn();
		transforms->SetROIFilename( level2ROIFilename );
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

		std::cout << "\n -> Validation\n" << std::endl;
		// perform validation
		validation->SetImage2( transforms->GetTransformedImage());
		validation->SetLabelMap2( transforms->GetTransformedLabelMap() );
		validation->LabelOverlapMeasuresOn();
		try
		{
			validation->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}

		// write out level 1 transform
/*		std::string level1TransformFilename = outputDirectory + "\\Level1Transform.tfm";
		WriteOutTransform< TransformType >( level1TransformFilename.c_str(), level1Registration->GetFinalTransform() );
		// write out images
		std::string level1ResampledImageFilename = outputDirectory + "\\Level1ResampledImage.mhd";
		WriteOutImage< ImageType, ImageType >( level1ResampledImageFilename.c_str(), transforms->GetTransformedImage() );
		std::string level1MovingCroppedImageFilename = outputDirectory + "\\Level1MovingCroppedImage.mhd";
		WriteOutImage< ImageType, ImageType >( level1MovingCroppedImageFilename.c_str(), transforms->GetMovingCroppedImage() );
		std::string level1FixedCroppedImageFilename = outputDirectory + "\\Level1FixedCroppedImage.mhd";
		WriteOutImage< ImageType, ImageType >( level1FixedCroppedImageFilename.c_str(), transforms->GetFixedCroppedImage() );
		// write out label map
		std::string level1ResampledLabelMapFilename = outputDirectory + "\\Level1ResampledLabelMap.mhd";
		WriteOutImage< ImageType, ImageType >( level1ResampledLabelMapFilename.c_str(), transforms->GetTransformedLabelMap() );
		// write out final composite transform
		std::string level1CompositeTransformFilename = outputDirectory + "\\Level1CompositeTransform.tfm";
		WriteOutTransform< itk::ManageTransformsFilter::CompositeTransformType >( level1CompositeTransformFilename.c_str(), transforms->GetCompositeTransform() );
*/
		// Registration level 1
		chronometer.Stop( "Level 1" );
		memorymeter.Stop( "Level 1" );
	}

	if( numberOfLevels > 1 )
	{
		// Registration level 2
		chronometer.Start( "Level 2" );
		memorymeter.Start( "Level 2" );

		// test functionality of itkRegistrationFramework.h
		std::cout << "\n*********************************************" << std::endl;
		std::cout << "            REGISTRATION LEVEL 2               " << std::endl;
		std::cout << "*********************************************\n" << std::endl;

		std::cout << "\n -> Registration\n" << std::endl;
		// create new registration class
		itk::RegistrationFramework::Pointer level2Registration = itk::RegistrationFramework::New();
		level2Registration->SetFixedImage( transforms->GetFixedCroppedImage() );
		level2Registration->SetMovingImage( transforms->GetMovingCroppedImage() );
		level2Registration->SetNumberOfIterations( numberOfIterations );
		level2Registration->SetMaximumStepLength( maximumStepLength );
		level2Registration->SetGradientMagnitudeTolerance( gradientMagnitudeTolerance );
		level2Registration->SetRotationScale( rotationScale );
		level2Registration->SetTranslationScale( translationScale );
		level2Registration->SetScalingScale( scalingScale );
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
		//transforms->SetTransform( transforms->GetCompositeTransform() );
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

		std::cout << "\n -> Validation\n" << std::endl;
		// perform validation
		validation->SetImage2( transforms->GetTransformedImage());
		validation->SetLabelMap2( transforms->GetTransformedLabelMap() );
		validation->LabelOverlapMeasuresOn();
		try
		{
			validation->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}

		// write out level 1 transform
/*		std::string level2TransformFilename = outputDirectory + "\\Level2Transform.tfm";
		WriteOutTransform< TransformType >( level2TransformFilename.c_str(), level2Registration->GetFinalTransform() );
		// write out image
		std::string level2ResampledImageFilename = outputDirectory + "\\Level2ResampledImage.mhd";
		WriteOutImage< ImageType, ImageType >( level2ResampledImageFilename.c_str(), transforms->GetTransformedImage() );
		std::string level2MovingCroppedImageFilename = outputDirectory + "\\Level2MovingCroppedImage.mhd";
		WriteOutImage< ImageType, ImageType >( level2MovingCroppedImageFilename.c_str(), transforms->GetMovingCroppedImage() );
		std::string level2FixedCroppedImageFilename = outputDirectory + "\\Level2FixedCroppedImage.mhd";
		WriteOutImage< ImageType, ImageType >( level2FixedCroppedImageFilename.c_str(), transforms->GetFixedCroppedImage() );
		// write out label map
		std::string level2ResampledLabelMapFilename = outputDirectory + "\\Level2ResampledLabelMap.mhd";
		WriteOutImage< ImageType, ImageType >( level2ResampledLabelMapFilename.c_str(), transforms->GetTransformedLabelMap() );
		// final composite transform
		std::string level2CompositeTransformFilename = outputDirectory + "\\Level2CompositeTransform.tfm";
		WriteOutTransform< itk::ManageTransformsFilter::CompositeTransformType >( level2CompositeTransformFilename.c_str(), transforms->GetCompositeTransform() );
*/	
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

		std::cout << "\n -> Registration\n" << std::endl;
		// create new registration class
		itk::RegistrationFramework::Pointer level3Registration = itk::RegistrationFramework::New();
		level3Registration->SetFixedImage( transforms->GetFixedCroppedImage() );
		level3Registration->SetMovingImage( transforms->GetMovingCroppedImage() );
		level3Registration->SetNumberOfIterations( numberOfIterations );
		level3Registration->SetMaximumStepLength( maximumStepLength );
		level3Registration->SetGradientMagnitudeTolerance( gradientMagnitudeTolerance );
		level3Registration->SetRotationScale( rotationScale );
		level3Registration->SetTranslationScale( translationScale );
		level3Registration->SetScalingScale( scalingScale );
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
		//transforms->SetTransform( transforms->GetCompositeTransform() );
		transforms->ResampleImageOn();
		transforms->CropImageOff();
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

		std::cout << "\n -> Validation\n" << std::endl;
		// perform validation
		validation->SetImage2( transforms->GetTransformedImage());
		validation->SetLabelMap2( transforms->GetTransformedLabelMap() );
		validation->LabelOverlapMeasuresOn();
		try
		{
			validation->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}

		// write out level 1 transform
/*		std::string level3TransformFilename = outputDirectory + "\\Level3Transform.tfm";
		WriteOutTransform< TransformType >( level3TransformFilename.c_str(), level3Registration->GetFinalTransform() );
		// write out image
		std::string level3ResampledImageFilename = outputDirectory + "\\Level3ResampledImage.mhd";
		WriteOutImage< ImageType, ImageType >( level3ResampledImageFilename.c_str(), transforms->GetTransformedImage() );
		// write out label map
		std::string level3ResampledLabelMapFilename = outputDirectory + "\\Level3ResampledLabelMap.mhd";
		WriteOutImage< ImageType, ImageType >( level3ResampledLabelMapFilename.c_str(), transforms->GetTransformedLabelMap() );
*/
		// Registration level 3
		chronometer.Stop( "Level 3" );
		memorymeter.Stop( "Level 3" );
	}

	// write out final composite transform
	std::string level3CompositeTransformFilename = outputDirectory + "\\Level3CompositeTransform.tfm";
	WriteOutTransform< itk::ManageTransformsFilter::CompositeTransformType >( level3CompositeTransformFilename.c_str(), transforms->GetCompositeTransform() );

	// full program
	chronometer.Stop( "Full program" );
	memorymeter.Stop( "Full program" );

	// print out time/memory results
	chronometer.Report( std::cout );
	memorymeter.Report( std::cout );

	return EXIT_SUCCESS;
}

void timestamp()
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

	std::cout << " REQUIRED: " << std::endl;
	std::cout << "  fixedImageFilename: path to the fixed image | required" << std::endl;
	std::cout << "  movingImageFilename: path to the moving image | required" << std::endl;
	std::cout << "  outputDirectory: path of the directory to save the results | required" << std::endl;
	std::cout << "  fixedValidationMaskFilename: path to the label map corresponding to \n";
	std::cout << "                               the fixed image for validation | required" << std::endl;
	std::cout << "  movingValidationMaskFilename: path to the label map corresponding to \n";
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
	std::cout << "  [relaxationFactor]: amount by which the step length decreases during \n";
	std::cout << "                      optimization | default = 0.5" << std::endl;
	std::cout << "  [gradientMagnitudeTolerance]: the minimum gradient magnitude \n";
	std::cout << "                                allowed | default = 0.001" << std::endl;
	std::cout << std::endl;
	
	return;
}
