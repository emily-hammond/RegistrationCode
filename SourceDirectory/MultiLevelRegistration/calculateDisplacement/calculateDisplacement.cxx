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
#include "itkTransformToDisplacementFieldFilter.h"
#include "C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SourceDirectory\MultiLevelRegistration\ReadWriteFunctions.hxx"


template< typename TransformType >
int calculateDisplacement( char * pointsFilename, std::string transformFilename, char * outputFilename, char * imageFilename )
{
	// read in transform
	TransformType::Pointer transform = ReadInTransform< TransformType >( transformFilename.c_str() );
	
	// read in points from csv file and transform with input transform
	// write results out to file
	std::ifstream infile( pointsFilename );
	std::ofstream outfile( outputFilename, std::ios::app );
	bool firstLine = true;

	outfile << transformFilename << "\n";
	
	// parse through every line in file
	std::string line;
	while( getline( infile,line ))
	{
		if ( !firstLine )
		{
			size_t pos1 = line.find( ',', 0 );
			TransformType::InputPointType point;

			// read in point
			for( int i = 0; i < 3; ++i )
			{
				const size_t pos2 = line.find( ',', pos1+1 );
				point[i] = atof( line.substr( pos1+1, pos2-pos1-1 ).c_str() );
				if( i < 2 ) // negate first two components for RAS->LPS
				{
					point[i] *= -1;
				}
				pos1 = pos2;
			}

			// transform point
			TransformType::OutputPointType transPoint = transform->TransformPoint( point );

			// write results out to file
			outfile << point[0] << "," << point[1] << "," << point[2] << "," << transPoint[0] << "," << transPoint[1] << "," << transPoint[2]
				<< "," << point[0]-transPoint[0] << "," << point[1]-transPoint[1] << "," << point[2]-transPoint[2] << "\n";
		}
		else
		{
			firstLine = false;
			outfile << line << ",xT,yT,zT,xD,yD,zD\n";
		}
	}
	outfile << "\n";

	// close file
	infile.close();
	outfile.close();

	// read in fixed image
	typedef itk::Image< short, 3 > ImageType;

	// obtain deformation field
	typedef itk::Vector< float, 3 > VectorPixelType;
	typedef itk::Image< VectorPixelType, 3 > DisplacementImageType;
	typedef itk::TransformToDisplacementFieldFilter< DisplacementImageType, double > DisplacementFieldGenType;

	DisplacementFieldGenType::Pointer dispFieldGen = DisplacementFieldGenType::New();

	dispFieldGen->UseReferenceImageOn();
	dispFieldGen->SetReferenceImage( ReadInImage< ImageType >( imageFilename ) );
	dispFieldGen->SetTransform( transform );
	
	// update displacement field generator
	try
	{
		dispFieldGen->Update();
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}

	size_t pos = transformFilename.rfind( '.' );
	std::string displacementFilename = transformFilename.substr(0,pos) + "-deformationField.mhd";
	WriteOutImage< DisplacementImageType, DisplacementImageType >( displacementFilename.c_str(), dispFieldGen->GetOutput() );
	
	return EXIT_SUCCESS;
}

int main( int argc, char * argv[] )
{
    // obtain inputs
    char * pointsFilename = argv[1];
	char * outputFilename = argv[2];
    std::string transformFilename = argv[3];
	std::string transformType = argv[4];
	char * imageFilename = argv[5];

	// read in transform
	
	itk::AffineTransform< double >::Pointer affineTransform = itk::AffineTransform< double >::New();

	if( transformType.compare("composite") == 0 )
	{
		calculateDisplacement< itk::CompositeTransform< double > >( pointsFilename, transformFilename, outputFilename, imageFilename );
	}
	else if( transformType.compare("affine") == 0 )
	{
		calculateDisplacement< itk::AffineTransform< double > >( pointsFilename, transformFilename, outputFilename, imageFilename );
	}
	else
	{
		std::cout << "Incorrect transform type: composite or affine only" << std::endl;
	}

    return EXIT_SUCCESS;
}

