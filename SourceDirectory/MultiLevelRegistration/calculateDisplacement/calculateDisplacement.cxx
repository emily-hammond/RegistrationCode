/*
 * Emily Hammond
 * 05/05/16
 *
 * The purpose of this code is to calculate the displacement of points from a text file
 * with a given transform -> very specific code for specific testing. Not universal
 *
 */

#include "itkCompositeTransform.h"
#include "itkAffineTransform.h"
#include "itkTransformToDisplacementFieldFilter.h"
#include "itkAddImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SourceDirectory\MultiLevelRegistration\ReadWriteFunctions.hxx"
#include <string>

template< typename TransformType, typename ImageType >
typename ImageType::Pointer calculateDisplacementField( std::string transformFilename, std::string imageFilename )
{
	// read in transform
	TransformType::Pointer transform = ReadInTransform< TransformType >( transformFilename.c_str() );
	
	/*// read in points from csv file and transform with input transform
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
	*/

	// read in fixed image
	typedef itk::TransformToDisplacementFieldFilter< ImageType, double > DisplacementFieldGenType;
	typedef itk::Image< short, 3 > ReferenceImageType;

	DisplacementFieldGenType::Pointer dispFieldGen = DisplacementFieldGenType::New();

	dispFieldGen->UseReferenceImageOn();
	dispFieldGen->SetReferenceImage( ReadInImage< ReferenceImageType >( imageFilename.c_str() ) );
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
	WriteOutImage< ImageType, ImageType >( displacementFilename.c_str(), dispFieldGen->GetOutput() );
	
	return dispFieldGen->GetOutput();
}

int main( int argc, char * argv[] )
{
    // obtain inputs
    //char * pointsFilename = argv[1];
	//char * outputFilename = argv[2];
    std::string appliedTransformFilename = argv[1];
	std::string registeredTransformFilename = argv[2];
	std::string referenceImageFilename = argv[3];
	char * outputFilename = argv[4];

	// define transforms
	typedef itk::CompositeTransform< double > CompositeTransformType;
	typedef itk::AffineTransform< double > AffineTransformType;

	// define deformation field
	typedef itk::Vector< float, 3 > VectorPixelType;
	typedef itk::Image< VectorPixelType, 3 > DisplacementImageType;
	typedef itk::Image< float, 3 > ImageType;

	DisplacementImageType::Pointer def1 = calculateDisplacementField< AffineTransformType, DisplacementImageType >( appliedTransformFilename, referenceImageFilename );
	DisplacementImageType::Pointer def2 = calculateDisplacementField< CompositeTransformType, DisplacementImageType >( registeredTransformFilename, referenceImageFilename );

	// write results to file
	std::ofstream outFile( outputFilename, std::ios::app );
	outFile << std::endl;
	outFile << "Applied Transform   : " << appliedTransformFilename << std::endl;
	outFile << "Registered Transform: " << registeredTransformFilename << std::endl;

	typedef itk::AddImageFilter< DisplacementImageType, DisplacementImageType, DisplacementImageType > AddDisplacementImageType;
	AddDisplacementImageType::Pointer addDisp = AddDisplacementImageType::New();
	addDisp->SetInput1( def1 );
	addDisp->SetInput2( def2 );
	addDisp->Update();

	size_t pos = referenceImageFilename.rfind( '.' );
	std::string filename = appliedTransformFilename.substr(0,pos) + "-deformationFieldDifference.mhd";
	WriteOutImage< DisplacementImageType, DisplacementImageType >( filename.c_str(), addDisp->GetOutput() );

	// for each index in the vector images
	for( int i = 0; i < 3; ++i )
	{
		// extract scalar image from index (def1)
		typedef itk::VectorIndexSelectionCastImageFilter< DisplacementImageType, ImageType > IndexSelectionType;
		IndexSelectionType::Pointer selection1 = IndexSelectionType::New();
		selection1->SetIndex( i );
		selection1->SetInput( def1 );
		// (def2)
		IndexSelectionType::Pointer selection2 = IndexSelectionType::New();
		selection2->SetIndex( i );
		selection2->SetInput( def2 );

		// subtract the two deformation fields
		typedef itk::AddImageFilter< ImageType, ImageType, ImageType > AddImageType;
		AddImageType::Pointer add = AddImageType::New();
		add->SetInput1( selection1->GetOutput() );
		add->SetInput2( selection2->GetOutput() );
		
		// acquire statistics
		typedef itk::StatisticsImageFilter< ImageType > StatisticsFilterType;
		StatisticsFilterType::Pointer stats = StatisticsFilterType::New();
		stats->SetInput( add->GetOutput() );

		// update for statistics
		try
		{
			stats->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}

		pos = referenceImageFilename.rfind( '.' );
		char index[1];
		filename = appliedTransformFilename.substr(0,pos) + "-" + itoa(i,index,10) + "-deformationField.mhd";
		WriteOutImage< ImageType, ImageType >( filename.c_str(), add->GetOutput() );

		// write results to file
		outFile << std::endl;
		outFile << "INDEX: " << i << std::endl;
		outFile << "  Mean  : " << stats->GetMean() << std::endl;
		outFile << "  Std   : " << stats->GetSigma() << std::endl;
		outFile << "  Min   : " << stats->GetMinimum() << std::endl;
		outFile << "  Max   : " << stats->GetMaximum() << std::endl;
		outFile << std::endl;
	}

    return EXIT_SUCCESS;
}

