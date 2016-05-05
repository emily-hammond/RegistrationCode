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
#include "itkSubtractImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include "itkVectorMagnitudeImageFilter.h"
#include "C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SourceDirectory\MultiLevelRegistration\ReadWriteFunctions.hxx"


template< typename TransformType, typename ImageType >
typename ImageType::Pointer calculateDisplacementField( std::string transformFilename, char * imageFilename )
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
	dispFieldGen->SetReferenceImage( ReadInImage< ReferenceImageType >( imageFilename ) );
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
	//std::string displacementFilename = transformFilename.substr(0,pos) + "-deformationField.mhd";
	//WriteOutImage< DisplacementImageType, DisplacementImageType >( displacementFilename.c_str(), dispFieldGen->GetOutput() );
	
	return dispFieldGen->GetOutput();
}

int main( int argc, char * argv[] )
{
    // obtain inputs
    //char * pointsFilename = argv[1];
	//char * outputFilename = argv[2];
    std::string appliedTransformFilename = argv[1];
	std::string registeredTransformFilename = argv[2];
	char * referenceImageFilename = argv[3];
	char * outputFilename = argv[4];

	// define transforms
	typedef itk::CompositeTransform< double > CompositeTransformType;
	typedef itk::AffineTransform< double > AffineTransformType;

	// define deformation field
	typedef itk::Vector< float, 3 > VectorPixelType;
	typedef itk::Image< VectorPixelType, 3 > DisplacementImageType;

	DisplacementImageType::Pointer def1 = calculateDisplacementField< AffineTransformType, DisplacementImageType >( appliedTransformFilename, referenceImageFilename );
	DisplacementImageType::Pointer def2 = calculateDisplacementField< CompositeTransformType, DisplacementImageType >( registeredTransformFilename, referenceImageFilename );

	// subtract the two deformation fields
	typedef itk::SubtractImageFilter< DisplacementImageType, DisplacementImageType, DisplacementImageType > SubtractImageType;
	SubtractImageType::Pointer subtractor = SubtractImageType::New();
	subtractor->SetInput1( def1 );
	subtractor->SetInput2( def2 );

	typedef itk::VectorMagnitudeImageFilter< DisplacementImageType, itk::Image< short, 3 > > MagnitudeImageType;
	MagnitudeImageType::Pointer magImage = MagnitudeImageType::New();
	magImage->SetInput( subtractor->GetOutput() );
	
	// acquire statistics
	typedef itk::StatisticsImageFilter< itk::Image< short, 3 > > StatisticsFilterType;
	StatisticsFilterType::Pointer stats = StatisticsFilterType::New();
	stats->SetInput( magImage->GetOutput() );

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

	// write results to file
	std::ofstream outFile( outputFilename, std::ios::app );
	outFile << std::endl;
	outFile << "Applied Transform   : " << appliedTransformFilename << std::endl;
	outFile << "Registered Transform: " << registeredTransformFilename << std::endl;
	outFile << std::endl;
	outFile << "Mean  : " << stats->GetMean() << std::endl;
	outFile << "Std   : " << stats->GetSigma() << std::endl;
	outFile << "Min   : " << stats->GetMinimum() << std::endl;
	outFile << "Max   : " << stats->GetMaximum() << std::endl;
	outFile << std::endl;

    return EXIT_SUCCESS;
}

