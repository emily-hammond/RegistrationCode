/*
INSERT COMMENTS HERE
*/

// include files
#include "ReadWriteFunctions.hxx"
#include "itkRegistrationFramework.h"

int main( int argc, char * argv[] )
{
	if( argc < 1 )
	{
		std::cout << "Usage: MultiLevelRegistration.exe fixedImage movingImage initialTransform" << std::endl;
	}

	//desired inputs
	char * fixedImageFilename = argv[1];
	char * movingImageFilename = argv[2];
	char * initialTransformFilename = argv[3];

	// instantiate image type
	typedef itk::Image<float, 3>	ImageType;
	typedef itk::ScaleVersor3DTransform< double >	TransformType;

	// read in fixed and moving images
	ImageType::Pointer fixedImage = ReadInImage< ImageType >( fixedImageFilename );
	ImageType::Pointer movingImage = ReadInImage< ImageType >( movingImageFilename );
	TransformType::Pointer initialTransform = ReadInTransform< TransformType >( initialTransformFilename );

	// test functionality of itkRegistrationFramework.h
	typedef itk::RegistrationFramework	Registration;
	Registration::Pointer registration = Registration::New();
	registration->SetImages( fixedImage, movingImage );
	registration->SetInitialTransform( initialTransform );
	registration->PerformRegistration();
	
	std::cout << "\nFinished\n" << std::endl;

	return EXIT_SUCCESS;
}
