/*
INSERT COMMENTS HERE
*/

// include files
#include "ReadWriteFunctions.hxx"
#include "itkRegistrationFramework.h"

int main( int argc, char * argv[] )
{
	//desired inputs
	char * fixedImageFilename = argv[1];
	char * movingImageFilename = argv[2]
	std::string outputDirectory = argv[3];

	// instantiate image type
	typedef itk::Image<unsigned short, 3>	ImageType;

	// read in fixed and moving images
	ImageType::Pointer fixedImage = ReadInImage< ImageType >( fixedImageFilename );
	ImageType::Pointer movingImage = ReadInImage< ImageType >( movingImageFilename );

	std::cout << fixedImage << std::endl;
	std::cout << movingImage << std::endl;

	// test functionality of itkRegistrationFramework.h
	typedef itk::RegistrationFramework	Registration;
	Registration( fixedImage, movingImage );
	
	
	return EXIT_SUCCESS;
}
