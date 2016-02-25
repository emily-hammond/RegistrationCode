/*
INSERT COMMENTS HERE
*/

// include files
#include "ReadWriteFunctions.hxx"
#include "itkRegistrationFramework.h"
#include "itkInitializationFilter.h"
#include "itkValidationFilter.h"
#include "itkManageTransformsFilter.h"
#include "itkIdentityTransform.h"

int main( int argc, char * argv[] )
{
	//desired inputs
	char * fixedImageFilename = argv[1];
	char * movingImageFilename = argv[2];
	std::string outputDirectory = argv[3];
	char * fixedValidationMaskFilename = argv[4];
	char * movingValidationMaskFilename = argv[5];
	int numberOfLevels = atoi( argv[6] );
	char * level2ROIFilename = argv[7];
	char * level3ROIFilename = argv[8];

	if( argc < 8 )
	{
		std::cout << "Usage: MultiLevelRegistration.exe fixedImage movingImage outputDirectory fixedValidationMask " << std::endl;
		std::cout << "            movingValidationMask numberOfLevels level2ROIFilename level3ROIFilename" << std::endl;
	}

	// instantiate image type
	typedef itk::Image< unsigned short, 3 >	ImageType;
	typedef itk::Image< unsigned char, 3 >	MaskImageType;
	typedef itk::ScaleVersor3DTransform< double >	TransformType;

	// read in necessary images
	ImageType::Pointer fixedImage = ReadInImage< ImageType >( fixedImageFilename );
	ImageType::Pointer movingImage = ReadInImage< ImageType >( movingImageFilename );
	ImageType::Pointer fixedValidationMask = ReadInImage< ImageType >( fixedValidationMaskFilename );
	ImageType::Pointer movingValidationMask = ReadInImage< ImageType >( movingValidationMaskFilename );
	//TransformType::Pointer initialTransform = ReadInTransform< TransformType >( initialTransformFilename );
	itk::IdentityTransform< double >::Pointer identityTransform = itk::IdentityTransform< double >::New();

	std::cout << "\nFixed image           : " << fixedImageFilename << std::endl;
	std::cout << "Fixed validation mask : " << fixedValidationMaskFilename << std::endl;
	std::cout << "Moving image          : " << movingImageFilename << std::endl;
	std::cout << "Moving validation mask: " << movingValidationMaskFilename << std::endl;
	std::cout << "Level 2ROI filename   : " << level2ROIFilename << std::endl;

	// initialization with validation
	std::cout << "\n*********************************************" << std::endl;
	std::cout << "              INITIALIZATION                 " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	itk::InitializationFilter::Pointer initialize = itk::InitializationFilter::New();
	initialize->SetFixedImage( fixedImage );
	initialize->SetMovingImage( movingImage );
	//initialize->ObserveOn();
	initialize->CenteredOnGeometryOn();
	initialize->MetricAlignmentOn( 0 );
	initialize->MetricAlignmentOn( 1 );
	initialize->MetricAlignmentOn( 2 );
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
	/*transforms->ResampleImageOn();
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

	// insert resampled moving image and mask (WRT initial transform) into validation class
	validation->SetImage2( transforms->GetTransformedImage() );
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
	}*/

	// write out initial transform and add to composite transform
	std::string initialTransformFilename = outputDirectory + "\\InitialTransform.tfm";
	WriteOutTransform< TransformType >( initialTransformFilename.c_str(), initialize->GetTransform() );
	//transforms->AddTransform( initialize->GetTransform() );

	// test functionality of itkRegistrationFramework.h
	std::cout << "\n*********************************************" << std::endl;
	std::cout << "            REGISTRATION LEVEL 1               " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	std::cout << "\n -> Registration\n" << std::endl;
	// create registration class
	itk::RegistrationFramework::Pointer level1Registration = itk::RegistrationFramework::New();
	level1Registration->SetFixedImage( fixedImage );
	level1Registration->SetMovingImage( movingImage );
	level1Registration->SetInitialTransform( initialize->GetTransform() );
	level1Registration->SetScalingScale( 0 );
	level1Registration->ObserveOn();
	//registration->ObserveOn();
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

	std::string level1CompositeTransformFilename = outputDirectory + "\\Level1CompositeTransform.tfm";
	WriteOutTransform< itk::ManageTransformsFilter::CompositeTransformType >( level1CompositeTransformFilename.c_str(), transforms->GetCompositeTransform() );

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

	// acquire resampled and cropped image as the moving image for the next level
	ImageType::Pointer level1ResampledImage = transforms->GetTransformedImage();
	ImageType::Pointer level1MovingCroppedImage = transforms->GetMovingCroppedImage();

	std::cout << "\n -> Validation\n" << std::endl;
	// perform validation
	validation->SetImage2( level1ResampledImage);
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
	std::string level1TransformFilename = outputDirectory + "\\Level1Transform.tfm";
	WriteOutTransform< TransformType >( level1TransformFilename.c_str(), level1Registration->GetFinalTransform() );
	// write out images
	std::string level1ResampledImageFilename = outputDirectory + "\\Level1ResampledImage.mhd";
	WriteOutImage< ImageType, ImageType >( level1ResampledImageFilename.c_str(), level1ResampledImage );
	std::string level1MovingCroppedImageFilename = outputDirectory + "\\Level1MovingCroppedImage.mhd";
	WriteOutImage< ImageType, ImageType >( level1MovingCroppedImageFilename.c_str(), level1MovingCroppedImage );
	std::string level1FixedCroppedImageFilename = outputDirectory + "\\Level1FixedCroppedImage.mhd";
	WriteOutImage< ImageType, ImageType >( level1FixedCroppedImageFilename.c_str(), transforms->GetFixedCroppedImage() );
	// write out label map
	std::string level1ResampledLabelMapFilename = outputDirectory + "\\Level1ResampledLabelMap.mhd";
	WriteOutImage< ImageType, ImageType >( level1ResampledLabelMapFilename.c_str(), transforms->GetTransformedLabelMap() );
	// write out final composite transform
	std::string level1CompositeTransformFilename = outputDirectory + "\\Level1CompositeTransform.tfm";
	WriteOutTransform< itk::ManageTransformsFilter::CompositeTransformType >( level1CompositeTransformFilename.c_str(), transforms->GetCompositeTransform() );
	
	if( numberOfLevels > 1 )
	{
		// test functionality of itkRegistrationFramework.h
		std::cout << "\n*********************************************" << std::endl;
		std::cout << "            REGISTRATION LEVEL 2               " << std::endl;
		std::cout << "*********************************************\n" << std::endl;

		std::cout << "\n -> Registration\n" << std::endl;
		// create new registration class
		itk::RegistrationFramework::Pointer level2Registration = itk::RegistrationFramework::New();
		level2Registration->SetFixedImage( transforms->GetFixedCroppedImage() );
		level2Registration->SetMovingImage( level1MovingCroppedImage );
		//level2Registration->SetInitialTransform( level1Registration->GetFinalTransform() );
		level2Registration->SetScalingScale( 0 );
		level2Registration->SetMaximumStepLength( 0.1 );
		level2Registration->ObserveOn();
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

		// acquire resampled image as the moving image for the next level
		ImageType::Pointer level2ResampledImage = transforms->GetTransformedImage();

		std::cout << "\n -> Validation\n" << std::endl;
		// perform validation
		validation->SetImage2( level2ResampledImage);
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
		std::string level2TransformFilename = outputDirectory + "\\Level2Transform.tfm";
		WriteOutTransform< TransformType >( level2TransformFilename.c_str(), level2Registration->GetFinalTransform() );
		// write out image
		std::string level2ResampledImageFilename = outputDirectory + "\\Level2ResampledImage.mhd";
		WriteOutImage< ImageType, ImageType >( level2ResampledImageFilename.c_str(), level2ResampledImage );
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
	}

	if( numberOfLevels > 2 )
	{
		// test functionality of itkRegistrationFramework.h
		std::cout << "\n*********************************************" << std::endl;
		std::cout << "            REGISTRATION LEVEL 3               " << std::endl;
		std::cout << "*********************************************\n" << std::endl;

		std::cout << "\n -> Registration\n" << std::endl;
		// create new registration class
		itk::RegistrationFramework::Pointer level3Registration = itk::RegistrationFramework::New();
		level3Registration->SetFixedImage( transforms->GetFixedCroppedImage() );
		level3Registration->SetMovingImage( transforms->GetMovingCroppedImage() );
		//level2Registration->SetInitialTransform( level1Registration->GetFinalTransform() );
		level3Registration->SetScalingScale( 0 );
		level3Registration->SetMaximumStepLength( 0.1 );
		level3Registration->ObserveOn();
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

		// acquire resampled image as the moving image for the next level
		ImageType::Pointer level3ResampledImage = transforms->GetTransformedImage();

		std::cout << "\n -> Validation\n" << std::endl;
		// perform validation
		validation->SetImage2( level3ResampledImage);
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
		std::string level3TransformFilename = outputDirectory + "\\Level3Transform.tfm";
		WriteOutTransform< TransformType >( level3TransformFilename.c_str(), level3Registration->GetFinalTransform() );
		// write out image
		std::string level3ResampledImageFilename = outputDirectory + "\\Level3ResampledImage.mhd";
		WriteOutImage< ImageType, ImageType >( level3ResampledImageFilename.c_str(), level3ResampledImage );
		// write out label map
		std::string level3ResampledLabelMapFilename = outputDirectory + "\\Level3ResampledLabelMap.mhd";
		WriteOutImage< ImageType, ImageType >( level3ResampledLabelMapFilename.c_str(), transforms->GetTransformedLabelMap() );
		// write out final composite transform
		std::string level3CompositeTransformFilename = outputDirectory + "\\Level3CompositeTransform.tfm";
		WriteOutTransform< itk::ManageTransformsFilter::CompositeTransformType >( level3CompositeTransformFilename.c_str(), transforms->GetCompositeTransform() );
	}

	return EXIT_SUCCESS;
}
