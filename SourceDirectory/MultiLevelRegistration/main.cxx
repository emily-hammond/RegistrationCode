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

	if( argc < 7 )
	{
		std::cout << "Usage: MultiLevelRegistration.exe fixedImage movingImage outputDirectory fixedValidationMask " << std::endl;
		std::cout << "            movingValidationMask numberOfLevels level2ROIFilename" << std::endl;
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
	itk::IdentityTransform::Pointer identityTransform = itk::IdentityTransform::New();

	std::cout << "\nFixed image           : " << fixedImageFilename << std::endl;
	std::cout << "Fixed validation mask : " << fixedValidationMaskFilename << std::endl;
	std::cout << "Moving image          : " << movingImageFilename << std::endl;
	std::cout << "Moving validation mask: " << movingValidationMaskFilename << std::endl;
	std::cout << "ROI filename          : " << roiFilename << std::endl;

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
	transforms->ResampleImageWithInitialTransformOn();
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
	}

	// write out initial transform
	std::string initialTransformFilename = outputDirectory + "\\InitialTransform.tfm";
	WriteOutTransform< TransformType >( initialTransformFilename.c_str(), initialize->GetTransform() );

	// test functionality of itkRegistrationFramework.h
	std::cout << "\n*********************************************" << std::endl;
	std::cout << "            REGISTRATION LEVEL 1               " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	// create registration class
	itk::RegistrationFramework::Pointer registration = itk::RegistrationFramework::New();
	registration->SetFixedImage( fixedImage );
	registration->SetMovingImage( movingImage );
	registration->SetInitialTransform( initialize->GetTransform() );
	//registration->ObserveOn();
	try
	{
		registration->Update();
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}
	registration->Print();


	// add transform to composite transform in transforms class and apply to moving image
	transforms->AddTransform( registration->GetFinalTransform() );
	transforms->ResampleImageOn();
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
	ImageType::Pointer level1ResampledImage = transforms->GetTransformedImage();

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
	WriteOutTransform< TransformType >( level1TransformFilename.c_str(), registration->GetFinalTransform() );
	// write out image
	std::string level1ResampledImageFilename = outputDirectory + "\\Level1ResampledImage.mhd";
	WriteOutImage< ImageType, ImageType >( level1ResampledImageFilename.c_str(), level1ResampledImage );

	if( numberOfLevels > 1 )
	{
		// test functionality of itkRegistrationFramework.h
		std::cout << "\n*********************************************" << std::endl;
		std::cout << "            REGISTRATION LEVEL 2               " << std::endl;
		std::cout << "*********************************************\n" << std::endl;

		// change inputs to registration object
		registration->SetMovingImage( level1ResampledImage );
		registration->SetInitialTransform( identityTransform );
		registration->SetROIFilename( level2ROIfilename );
		try
		{
			registration->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}
		registration->Print();

		// write out mask used
		std::string ROIMaskFilename = outputDirectory + "\Level2ROIMask.mhd";
		WriteOutImage< ImageType, ImageType >( ROIMaskFilename.c_str(), registration->GetMaskImage() );

		// add transform to composite transform in transforms class and apply to moving image/label map image
		// don't update images here. Apply composite transform to the original moving image and label map
		transforms->AddTransform( registration->GetFinalTransform() );
		transforms->ResampleImageOn();
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
		WriteOutTransform< TransformType >( level2TransformFilename.c_str(), registration->GetFinalTransform() );
		// write out image
		std::string level2ResampledImageFilename = outputDirectory + "\\Level2ResampledImage.mhd";
		WriteOutImage< ImageType, ImageType >( level2ResampledImageFilename.c_str(), level2ResampledImage );
	}

	return EXIT_SUCCESS;
}
