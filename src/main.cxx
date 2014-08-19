/*
 * Emily Hammond
 * 8/15/2014
 *
 * Goals with this code
 1. Write functions that read/write images and fiducial files from slicer
 2. 
 *
 */

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"

#include <itksys/SystemTools.hxx>
#include <fstream>
#include <string>

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
template<typename inputImageType, typename outputImageType>
int WriteOutImage( const char * ImageFilename, typename inputImageType::Pointer image )
{
	typedef itk::CastImageFilter<inputImageType, outputImageType> CastFilterType;
	typename CastFilterType::Pointer caster = CastFilterType::New();
	caster->SetInput( image );

	typedef itk::ImageFileWriter<outputImageType> WriterType;
	typename WriterType::Pointer writer = WriterType::New();
	writer->SetFileName( ImageFilename );
	writer->SetInput( caster->GetOutput() );

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

// Write a function to read in the fiducials (from Slicer) for each image
template<typename PointType, typename LandmarksType>
typename LandmarksType ReadFiducial( const char * fiducialFilename )
{
	// open file
	std::ifstream myfile( fiducialFilename );
	LandmarksType landmarks;
	
	// read in lines from file
	std::string line;
	while( getline( myfile, line ) )
	{
		if( line.compare( 0, 1, "#" ) != 0 ) // skip lines starting with #
		{
			// determine where the first coordinate lives
			size_t pos1 = line.find( ',', 0 );
			PointType	pointPos;

			// grab the coordinates from the string
			for( unsigned int i = 0; i < 3; ++i )
			{
				const size_t pos2 = line.find( ',', pos1+1 );
				pointPos[i] = atof( line.substr( pos1+1, pos2-pos1-1 ).c_str() );
				if( i < 2 ) // negate first two components for RAS->LPS
				{
					pointPos[i] *= -1;
				}
				pos1 = pos2;
			}
			
			// determine what label/location the coordinates are from
			if( line.find( "Carina" ) != -1 )
			{
				landmarks["Carina"] = pointPos;
			}
			else if( line.find( "Aorta" ) != -1 )
			{
				landmarks["Aorta"] = pointPos;
			}
			else if( line.find( "BaseOfHeart" ) != -1 )
			{
				landmarks["BaseOfHeart"] = pointPos;
			}
			else
			{
				std::cerr << "Reassign labels to fiducials." << std::endl;
				std::cerr << "   Acceptable labels: Carina, Aorta, BaseOfHeart" << std::endl;
			}
		}
	}
	return landmarks;
}

template< typename LandmarksType >
int printFiducials( LandmarksType landmarks )
{
	// iterate through the landmarks and print out the contents
	LandmarksType::const_iterator it = landmarks.begin();
	for(; it!=landmarks.end(); it++)
	{
		std::cout << it->first << " " << it->second << std::endl;
	}

	return EXIT_SUCCESS;
}

int main( int argc, char * argv[] )
{
	// create specified image types to be used
	typedef itk::Image<float, 3>			FloatImageType;
	typedef itk::Image<unsigned char, 3>	CharImageType;
	typedef std::map<std::string, FloatImageType::PointType> LandmarksType;

	// take in arguments
	char * InputFilename = argv[1];
	char * OutputFilename = argv[2];
	char * FiducialFilename = argv[3];

	// read in the image
	FloatImageType::Pointer inputImage = ReadInImage<FloatImageType>( InputFilename );
	std::cout << "Image has been read in." << std::endl;

	// read in fiducials
	LandmarksType landmarks = ReadFiducial<FloatImageType::PointType,LandmarksType>( FiducialFilename );
	printFiducials<LandmarksType>(landmarks);

	// convert points to index and draw on the image
	FloatImageType::IndexType carina;
	FloatImageType::IndexType aorta;
	FloatImageType::IndexType baseofheart;

	inputImage->TransformPhysicalPointToIndex(landmarks["Carina"], carina);
	inputImage->TransformPhysicalPointToIndex(landmarks["Aorta"],aorta);
	inputImage->TransformPhysicalPointToIndex(landmarks["BaseOfHeart"],baseofheart);

	inputImage->SetPixel(carina, 2000);
	inputImage->SetPixel(aorta, 2000);
	inputImage->SetPixel(baseofheart, 2000);

	std::cout << "Fiducials Marked!" << std::endl;

	// write out image
	WriteOutImage<FloatImageType,FloatImageType>( OutputFilename, inputImage );
	std::cout << "Image has been written." << std::endl;

    return EXIT_SUCCESS;
}