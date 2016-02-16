/*
INSERT COMMENTS HERE
*/

// include files
#include "ReadWriteFunctions.hxx"
#include "itkRegistrationFramework.h"
#include "itkInitializationFilter.h"

int main( int argc, char * argv[] )
{
	//desired inputs
	char * fixedImageFilename = argv[1];
	char * movingImageFilename = argv[2];
	char * initialTransformFilename = argv[3];

	// instantiate image type
	typedef itk::Image<unsigned short, 3>	ImageType;
	typedef itk::ScaleVersor3DTransform< double >	TransformType;

	// read in fixed and moving images
	ImageType::Pointer fixedImage = ReadInImage< ImageType >( fixedImageFilename );
	ImageType::Pointer movingImage = ReadInImage< ImageType >( movingImageFilename );
	TransformType::Pointer initialTransform = ReadInTransform< TransformType >( initialTransformFilename );

	// initialization
	itk::InitializationFilter::Pointer initialize = itk::InitializationFilter::New();
	initialize->SetImages( fixedImage, movingImage );
	initialize->CenteredOnGeometry();
	initialize->PerformInitialization();

	std::cout << initialize->GetOutput() << std::endl;

	// test functionality of itkRegistrationFramework.h
	itk::RegistrationFramework::Pointer registration = itk::RegistrationFramework::New();
	registration->SetImages( fixedImage, movingImage );
	registration->SetInitialTransform( initialize->GetOutput() );
	registration->ObserveOn();
	registration->PerformRegistration();

	std::cout << registration->GetOutput() << std::endl;
	
	std::cout << "\nFinished\n" << std::endl;

	return EXIT_SUCCESS;
}
