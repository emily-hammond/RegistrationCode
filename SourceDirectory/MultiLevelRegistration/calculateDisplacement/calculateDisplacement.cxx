/*
 * Emily Hammond
 * 05/05/16
 *
 * The purpose of this code is to calculate the displacement of points from a text file
 * with a given transform
 *
 */

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkCompositeTransform.h"
#include "itkAffineTransform.h"
#include "C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SourceDirectory\MultiLevelRegistration\ReadWriteFunctions.hxx"


template< typename TransformType >
int calculateDisplacement( char * pointsFilename, char * transformFilename )
{
	// read in transform
	TransformType::Pointer transform = ReadInTransform< TransformType >( transformFilename );
	
	// read in points from csv file and transform with input transform
	// write results out to file
	std::fstream myfile( pointsFilename );
	bool firstLine = true;
	
	// parse through every line in file
	std::string line;
	while( getline( myfile,line ))
	{
		if ( !firstLine )
		{
			size_t pos = line.find( ',', 0 );
			PointType point;
			std::cout << line << std::endl;
		}
		else
		{
			firstLine = false;
			myfile << line << ",xT,yT,zT,dis\n";
		}
	}

	// close file
	myfile.close();
	
	return EXIT_SUCCESS;
}

int main( int argc, char * argv[] )
{
    // obtain inputs
    char * pointsFilename = argv[1];
    char * transformFilename = argv[2];
	std::string transformType = argv[3];

	// read in transform
	
	itk::AffineTransform< double >::Pointer affineTransform = itk::AffineTransform< double >::New();

	if( transformType.compare("composite") == 0 )
	{
		calculateDisplacement< itk::CompositeTransform< double > >( pointsFilename, transformFilename );
	}
	else if( transformType.compare("affine") == 0 )
	{
		calculateDisplacement< itk::AffineTransform< double > >( pointsFilename, transformFilename );
	}
	else
	{
		std::cout << "Incorrect transform type: composite or affine only" << std::endl;
	}

    return EXIT_SUCCESS;
}

