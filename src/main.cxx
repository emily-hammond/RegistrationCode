/*
 * Emily Hammond
 * 8/15/2014
 *
 * Goals with this code
 1. Write functions that read in images, csv files and txt files
 *
 */

// Write a function to read in images templated over dimension and pixel type
template< const unsigned int Dimension, typename InputPixelType>
int ReadInImage( const char * ImageFilename)
{
	// set up aliases
	typedef InputPixelType						PixelType;
	typedef itk::Image<PixelType, Dimension>	ImageType;
	typedef itk::ImageFileReader<ImageType>		ReaderType;
	
	// create pointers to images
	typename ReaderType::Pointer reader = ReaderType::New();
	
	// set parameters of reader
	reader->SetFilename( ImageFilename );
	
	// update reader
	try
	{
		reader->Update();
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}
	
	return EXIT_SUCCESS;
}
	

int main( int argc, char * argv[] )
{
    return 0;
}