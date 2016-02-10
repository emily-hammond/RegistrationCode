/*
INSERT COMMENTS HERE
*/

// include files
#include ReadWriteFunction.h


int main( int argc, char * argv[] )
{
	//desired inputs
	char * imageFilename = argv[1];
	std::string outputDirectory = argv[2];

	// test functionality of ReadWriteFunctions.h file
	typedef itk::Image<unsigned short, 3>	ImageType;
	ImageType::Pointer inputImage = ReadInImage< ImageType >( imageFilename );
	std::string outputFilename  = outputDirectory + "\image.mhd";
	WriteOutImage< ImageType, ImageType >( outputDirectory.c_str() );
	
	return EXIT_SUCCESS;
}
