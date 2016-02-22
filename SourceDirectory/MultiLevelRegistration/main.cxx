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
	char * fixedValidationMaskFilename = argv[3];
	char * movingValidationMaskFilename = argv[4];
	int numberOfLevels = atoi( argv[5] );
	char * roiFilename = argv[6];

	if( argc < 7 )
	{
		std::cout << "Usage: MultiLevelRegistration.exe fixedImage movingImage fixedValidationMask " << std::endl;
		std::cout << "            movingValidationMask numberOfLevels roiFilename" << std::endl;
	}

	// instantiate image type
	typedef itk::Image< unsigned short, 3 >	ImageType;
	typedef itk::Image< unsigned short, 3 >	LabelMapType;
	typedef itk::Image< unsigned char, 3 >	MaskImageType;
	typedef itk::ScaleVersor3DTransform< double >	TransformType;

	// read in necessary images
	ImageType::Pointer fixedImage = ReadInImage< ImageType >( fixedImageFilename );
	ImageType::Pointer movingImage = ReadInImage< ImageType >( movingImageFilename );
	LabelMapType::Pointer fixedValidationMask = ReadInImage< LabelMapType >( fixedValidationMaskFilename );
	LabelMapType::Pointer movingValidationMask = ReadInImage< LabelMapType >( movingValidationMaskFilename );
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
	initialize->SetImages( fixedImage, movingImage );
	//initialize->ObserveOn();
	initialize->CenteredOnGeometryOn();
	initialize->MetricAlignmentOn( 0 );
	initialize->MetricAlignmentOn( 1 );
	initialize->MetricAlignmentOn( 2 );
	initialize->Update();

	// test functionality of itkRegistrationFramework.h
	std::cout << "\n*********************************************" << std::endl;
	std::cout << "               REGISTRATION                  " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	itk::RegistrationFramework::Pointer registration = itk::RegistrationFramework::New();
	registration->SetImages( fixedImage, movingImage );
	registration->SetInitialTransform( initialize->GetOutput() );
	//registration->ObserveOn();
	registration->Update();
	registration->PrintResults();
	
	// test functionality of itkManageTransformsFilter.h
	std::cout << "\n*********************************************" << std::endl;
	std::cout << "                TRANSFORMS                  " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	itk::ManageTransformsFilter::Pointer transforms = itk::ManageTransformsFilter::New();
	transforms->AddTransform( registration->GetFinalTransform() );
	transforms->SetImages( fixedImage, movingImage );
	// create mask file from ROI points
	WriteOutImage< MaskImageType, MaskImageType >( "maskImage.mhd", transforms->GenerateMaskFromROI( roiFilename ) );
	// write out final transform
	WriteOutTransform< TransformType >( "finalTransform.tfm", registration->GetFinalTransform() );
	// apply transform to image by resampling
	transforms->ResampleImageOn();
	transforms->Update();
	
	// get transfomed image
	ImageType::Pointer transformedImage = transforms->GetTransformedImage();
	WriteOutImage< ImageType, ImageType >( "finalImage.mhd", transforms->GetTransformedImage() );

	// transform corresponding mask
	transforms->SetImages( fixedValidationMask, movingValidationMask );
	transforms->NearestNeighborInterpolateOn();
	transforms->Update();
	LabelMapType::Pointer transformedMask = transforms->GetTransformedImage();

	std::cout << "\n*********************************************" << std::endl;
	std::cout << "                VALIDATION                  " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	itk::ValidationFilter::Pointer validation = itk::ValidationFilter::New();
	validation->SetImageAndLabelMap1( fixedImage, fixedValidationMask );
	validation->SetImageAndLabelMap2( transformedImage, transformedMask );
	validation->LabelOverlapMeasuresOn();
	validation->Update();

	return EXIT_SUCCESS;
}
