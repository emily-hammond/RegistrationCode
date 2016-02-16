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

	// test functionality of itkRegistrationFramework.h
	itk::RegistrationFramework::Pointer registration = itk::RegistrationFramework::New();
	registration->SetImages( fixedImage, movingImage );
	registration->SetInitialTransform( initialTransform );
	registration->ObserveOn();
	registration->PerformRegistration();
	TransformType::Pointer finalTransform = registration->GetOutput();
	std::cout << finalTransform << std::endl;
	
	std::cout << "\nFinished\n" << std::endl;

	return EXIT_SUCCESS;
}
