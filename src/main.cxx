/*
 * Emily Hammond
 * 8/15/2014
 *
 * Goals with this code
 1. Write functions that read in images, csv files and txt files
 *
 */

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

// Write a function to read in images templated over dimension and pixel type
template<typename ImageType>
typename ImageType::Pointer ReadInImage( const char * ImageFilename )
{
	typedef itk::ImageFileReader<ImageType>		ReaderType;	
	typename ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName( ImageFilename );
	
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
	
	// return output
	return reader->GetOutput();
}

// Write a function to write out images
template<typename ImageType>
int WriteOutImage( const char * ImageFilename,  )
{
	typedef itk::ImageFileWriter<ImageType>		WriterType;
	typename WriterType::Pointer writer = WriterType::New();
	writer->SetFileName( ImageFilename );

	// update the writer
	try
	{
		writer->Update();
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}
	
	// return output
	return EXIT_SUCCESS;
}

int main( int argc, char * argv[] )
{
	// create specified image types to be used
	typedef itk::Image<float, 3>			FloatImageType;
	typedef itk::Image<unsigned char, 3>	CharImageType;

	// take in arguments
	char * InputFilename = argv[1];
	char * OutputFilename = argv[2];

	// read in the image
	FloatImageType::Pointer inputImage = ReadInImage<FloatImageType>( InputFilename );
	std::cout << "Image has been read in." << std::endl;

	// write out image
	WriteOutImage<CharImageType>( OutputFilename );
	std::cout << "Image has been written." << std::endl;

    return EXIT_SUCCESS;
}