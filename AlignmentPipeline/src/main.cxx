/* Emily Hammond
 * 2014-10-17
 *
 * This code is the main code written to align multi-modal images taken of the same
 * subject on the same day.
 * 
 * steps:
 *	1. Create a set up for the desired inputs and outputs
 *			Inputs: fixed image, moving image, output directory name
 *			Outputs: transformed moving image, final transform parameters, 
 *					 deformation image, jacobian map			
 *	2. Read and write functions pulled from RegistrationCode/src/ReadWriteFunctions
 *
 */

/**************************************************************************
 * Header files to include
 **************************************************************************/

// reading/writing files
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"

#include <itksys/SystemTools.hxx>
#include <fstream>
#include <string>

/*************************************************************************
 * Write templated functions for reading/writing files to clean up code 
 *************************************************************************/

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


// Write a function to write out images templated over input and output pixel type
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
// templated over point type (taken from image) and landmarks type (map<string, point type>)
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



// Write a function to write out a transform
template<typename TransformType>
int WriteOutTransform( const char * transformFilename, TransformType transform )
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
	return EXIT_SUCCESS;
}

/*************************************************************************
 * Main function to perform/test functionality
 *************************************************************************/
int main(int argc, char * argv[])
{
	// list desired inputs
	const char * fixedImageFilename = argv[1];
	const char * movingImageFilename = argv[2];
	const char * outputDirectory = argv[3];
	const char * outputFileFormat = argv[4];

	// list desired outputs
	const char * rigidResult = outputDirectory + "\rigidResult." + outputFileFormat;
	const char * rigidTransform = outputDirectory + "\rigidTransformParameters.txt";
	const char * deformableResult = outputDirectory + "\deformableResult." + outputFileFormat;
	const char * deformableTransform = outputDirectory + "\deformableTransformParameters.txt";
	const char * deformation = outputDirectory + "deformation." + outputFileFormat;
	const char * jacobianMap = outputDirectory + "jacobianMap." + outputFileFormat;

	return EXIT_SUCCESS;
}