/*
INSERT COMMENTS HERE
*/

// include files
#include "ReadWriteFunctions.hxx"

int main( int argc, char * argv[] )
{
	//desired inputs
	char * imageFilename = argv[1];
	std::string outputDirectory = argv[2];

	// test functionality of ReadWriteFunctions.h file
	typedef itk::Image<unsigned short, 3>	ImageType;
	ImageType::Pointer inputImage = ReadInImage< ImageType >( imageFilename );
	std::string outputFilename  = outputDirectory + "image.mhd";
	std::cout << outputFilename << std::endl;
	WriteOutImage< ImageType, ImageType >( outputFilename.c_str(), inputImage );
	
	return EXIT_SUCCESS;
}
