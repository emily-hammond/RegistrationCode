/*
list of functions in file
1. ReadInImage
2. WriteOutImage
3. ReadFiducial ***will need modifications***
4. PrintFiducials
5. WriteOutTransform
*/

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"

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

// write function to print out fiducials
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

// Write a function to read in a transform
template<typename TransformType>
typename TransformType::Pointer ReadInTransform( const char * transformFilename)
{
	typedef itk::TransformFileReader	TransformReaderType;
	TransformReaderType::Pointer transformReader = TransformReaderType::New();

	// read in transform
	transformReader->SetFileName( transformFilename );
	try
	{
		transformReader->Update();
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}

	TransformReaderType::TransformListType * transforms = transformReader->GetTransformList();
	TransformType::Pointer transform = TransformType::New();
	//std::cout << "Number of transforms: " << transforms->size() << std::endl;

	// put into composite transform
	TransformReaderType::TransformListType::const_iterator it = transforms->begin();
	for( it = transforms->begin(); it != transforms->end(); ++it )
	{
		if( !strcmp( (*it)->GetNameOfClass(), "CompositeTransform" ) )
		{
			transform = static_cast< TransformType * >( (*it).GetPointer() );
		}
		else if( !strcmp( (*it)->GetNameOfClass(), "ScaleVersor3DTransform" ) )
		{
			transform = static_cast< TransformType * >( (*it).GetPointer() );
		}
		else
		{
			std::cout << "Transform is not compatible." << std::endl;
		}
	}

	return transform;
}

// Write a function to write out a transform
template<typename TransformType>
int WriteOutTransform( const char * transformFilename, typename TransformType::Pointer transform )
{
	typedef itk::TransformFileWriterTemplate< double > TransformWriterType;
	TransformWriterType::Pointer writer = TransformWriterType::New();
	writer->SetInput( transform );
	writer->SetFileName( transformFilename );

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
	std::cout << transformFilename << " has successfully been created." << std::endl;
	return EXIT_SUCCESS;
}