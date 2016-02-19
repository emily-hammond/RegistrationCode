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
	char * roiFilename = argv[3];

	if( argc < 4 )
	{
		std::cout << "Usage: MultiLevelRegistration.exe fixedImage movingImage roiFilename" << std::endl;
	}

	// instantiate image type
	typedef itk::Image< unsigned short, 3 >	ImageType;
	typedef itk::Image< unsigned int, 3 >	LabelMapType;
	typedef itk::ScaleVersor3DTransform< double >	TransformType;

	// read in necessary images
	ImageType::Pointer fixedImage = ReadInImage< ImageType >( fixedImageFilename );
	ImageType::Pointer movingImage = ReadInImage< ImageType >( movingImageFilename );
	//ImageType::Pointer fixedLabelMap = ReadInImage< LabelMapType >( fixedLabelMapFilename );
	//ImageType::Pointer movingLabelMap = ReadInImage< LabelMapType >( movingLabelMapFilename );
	//TransformType::Pointer initialTransform = ReadInTransform< TransformType >( initialTransformFilename );

	std::cout << "\nFixed image: " << fixedImageFilename << std::endl;
	std::cout << "Moving image: " << movingImageFilename << std::endl;
	std::cout << "ROI filename: " << roiFilename << std::endl;

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
	initialize->PerformInitialization();

	// test functionality of itkRegistrationFramework.h
	std::cout << "\n*********************************************" << std::endl;
	std::cout << "               REGISTRATION                  " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	itk::RegistrationFramework::Pointer registration = itk::RegistrationFramework::New();
	registration->SetImages( fixedImage, movingImage );
	registration->SetInitialTransform( initialize->GetOutput() );
	//registration->ObserveOn();
	registration->PerformRegistration();
	registration->PrintResults();
	std::cout << registration->GetFinalTransform() << std::endl;
	
	// test functionality of itkManageTransformsFilter.h
	std::cout << "\n*********************************************" << std::endl;
	std::cout << "                TRANSFORMS                  " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	itk::ManageTransformsFilter::Pointer transforms = itk::ManageTransformsFilter::New();
	transforms->AddTransform( registration->GetFinalTransform() );
	// create mask file from ROI points
	transforms->GenerateMaskFromROI( roiFilename, fixedImage );
	WriteOutTransform< TransformType >( "finalTransform.tfm", registration->GetFinalTransform() );
	// apply transform to image by resampling
	transforms->ResampleImageOn();
	WriteOutImage< ImageType, ImageType >( "finalImage.mhd", transforms->GetTransformedImage() );

	// compare two images
	std::cout << "\n\nORIGINAL IMAGE\n" << std::endl;
	std::cout << movingImage << std::endl;

	std::cout << "\n\nMODIFIED IMAGE\n" << std::endl;
	std::cout << transforms->HardenTransform( movingImage, registration->GetFinalTransform() ) << std::endl;

	std::cout << "\n*********************************************" << std::endl;
	std::cout << "                VALIDATION                  " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	itk::ValidationFilter::Pointer validation = itk::ValidationFilter::New();
	//validation->LabelOverlapMeasures( fixedLabelMap, movingLabelMap )

	return EXIT_SUCCESS;
}
