/*
INSERT COMMENTS HERE
*/

// include files
#include "ReadWriteFunctions.hxx"
#include "itkRegistrationFramework.h"
#include "itkInitializationFilter.h"
#include "itkValidationFilter.h"
#include "itkManageTransformsFilter.h"

int main( int argc, char * argv[] )
{
	//desired inputs
	char * fixedImageFilename = argv[1];
	char * movingImageFilename = argv[2];
	std::string outputDirectory = argv[3];
	char * fixedValidationMaskFilename = argv[4];
	char * movingValidationMaskFilename = argv[5];
	int numberOfLevels = atoi( argv[6] );
	char * roiFilename = argv[7];

	if( argc < 7 )
	{
		std::cout << "Usage: MultiLevelRegistration.exe fixedImage movingImage outputDirectory fixedValidationMask " << std::endl;
		std::cout << "            movingValidationMask numberOfLevels roiFilename" << std::endl;
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

	std::cout << "\nFixed image           : " << fixedImageFilename << std::endl;
	std::cout << "Fixed validation mask : " << fixedValidationMaskFilename << std::endl;
	std::cout << "Moving image          : " << movingImageFilename << std::endl;
	std::cout << "Moving validation mask: " << movingValidationMaskFilename << std::endl;
	std::cout << "ROI filename          : " << roiFilename << std::endl;

	// initialization
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
	transforms->Update();

	// insert resampled moving image and mask (WRT initial transform) into validation class
	validation->SetImage2( transforms->GetTransformedImage() );
	validation->SetLabelMap2( transforms->GetTransformedLabelMap() );
	validation->LabelOverlapMeasuresOn();
	validation->Update();

	// write out initial transform
	std::string initialTransformFilename = outputDirectory + "\\InitialTransform.tfm";
	WriteOutTransform< TransformType >( initialTransformFilename.c_str(), initialize->GetTransform() );

	// test functionality of itkRegistrationFramework.h
	std::cout << "\n*********************************************" << std::endl;
	std::cout << "               REGISTRATION                  " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	itk::RegistrationFramework::Pointer registration = itk::RegistrationFramework::New();
	registration->SetFixedImage( fixedImage );
	registration->SetMovingImage( movingImage );
	registration->SetInitialTransform( initialize->GetTransform() );
	//registration->ObserveOn();
	registration->Update();
	registration->Print();

	// write out final transform
	std::string finalTransformFilename = outputDirectory + "\\FinalTransform.tfm";
	WriteOutTransform< TransformType >( finalTransformFilename.c_str(), registration->GetFinalTransform() );

	// test functionality of itkManageTransformsFilter.h
	std::cout << "\n*********************************************" << std::endl;
	std::cout << "                TRANSFORMS                  " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	transforms->AddTransform( registration->GetFinalTransform() );

	// apply transform to image by resampling
	transforms->ResampleImageOn();
	transforms->Update();

	// write out resampled image
	ImageType::Pointer resampledImage = transforms->GetTransformedImage();
	std::string resampledImageFilename = outputDirectory + "\\ResampledImage.mhd";
	WriteOutImage< ImageType, ImageType >( resampledImageFilename.c_str(), resampledImage );

	// resample validation mask
	transforms->SetMovingImage( movingValidationMask );
	transforms->Update();

	// write out resampled mask
	ImageType::Pointer resampledMask = transforms->GetTransformedImage();
	std::string resampledMaskFilename = outputDirectory + "\\ResampledMask.mhd";
	WriteOutImage< ImageType, ImageType >( resampledMaskFilename.c_str(), resampledMask );

	std::cout << "\n*********************************************" << std::endl;
	std::cout << "                VALIDATION                  " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	validation->SetImage2( resampledImage);
	validation->SetLabelMap2( resampledMask );
	validation->LabelOverlapMeasuresOn();
	validation->Update();
	
	return EXIT_SUCCESS;
}
