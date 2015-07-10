/*
Author: Emily Hammond
Date: 7.1.2015

The purpose of this program is to calculate the dice and jaccard coefficients of two label maps.

Goals:
1. read in n label maps
2. obtain the dice/jaccard coefficients to get overlap of n label maps
3. write out results

*/

#include "itkImage.h"
#include "itkLabelObject.h"
#include "itkLabelMap.h"
#include "itkLabelImageToLabelMapFilter.h"
#include "itkLabelOverlapMeasuresImageFilter.h"
#include "itkImageFileReader.h"
#include <fstream>

//#include "LabelMapOverlapModuleCLP.h"

template<typename ImageType>
int LabelOverlapMeasures( typename ImageType::Pointer labelMap1, typename ImageType::Pointer labelMap2, std::ofstream& file )
{
	// instantiate overlap calculation filter
	typedef itk::LabelOverlapMeasuresImageFilter< ImageType >	OverlapFilter;
	OverlapFilter::Pointer overlap = OverlapFilter::New();

	// set images and calculate data
	overlap->SetSourceImage( labelMap1 );
	overlap->SetTargetImage( labelMap2 );
	overlap->Update();

	// write out data to file
	file << "******************** All Labels ********************\n";
	file << file.width( 10 ) << "   "
		 << file.width( 17 ) << "Total"
		 << file.width( 17 ) << "Union (jaccard)"
		 << file.width( 17 ) << "Mean (dice)"
		 << file.width( 17 ) << "Volume sim."
		 << file.width( 17 ) << "False negative"
		 << file.width( 17 ) << "False positive\n";
	file << file.width( 10 ) << "   ";
	file << file.width( 17 ) << overlap->GetTotalOverlap();
	file << file.width( 17 ) << overlap->GetUnionOverlap();
	file << file.width( 17 ) << overlap->GetMeanOverlap();
	file << file.width( 17 ) << overlap->GetVolumeSimilarity();
	file << file.width( 17 ) << overlap->GetFalseNegativeError();
	file << file.width( 17 ) << overlap->GetFalsePositiveError();
	file << "\n";

	file << "***************** Individual Labels *****************\n";
	file << file.width( 10 ) << "Label"
		 << file.width( 17 ) << "Target"
		 << file.width( 17 ) << "Union (jaccard)"
		 << file.width( 17 ) << "Mean (dice)"
		 << file.width( 17 ) << "Volume sim."
		 << file.width( 17 ) << "False negative"
		 << file.width( 17 ) << "False positive\n";

	typename OverlapFilter::MapType labelMap = overlap->GetLabelSetMeasures();
	typename OverlapFilter::MapType::const_iterator itM;
	for( itM = labelMap.begin(); itM != labelMap.end(); ++itM )
	{
		if( (*itM).first == 0 )
		{
			continue;
		}

		int label = (*itM).first;
		file << file.width( 10 ) << label;
		file << file.width( 17 ) << overlap->GetTargetOverlap( label );
		file << file.width( 17 ) << overlap->GetUnionOverlap( label );
		file << file.width( 17 ) << overlap->GetMeanOverlap( label );
		file << file.width( 17 ) << overlap->GetVolumeSimilarity( label );
		file << file.width( 17 ) << overlap->GetFalseNegativeError( label );
		file << file.width( 17 ) << overlap->GetFalsePositiveError( label );
		file << "\n";
	}

	return EXIT_SUCCESS;
}

int main(int argc, char * argv[])
{
	// parse through arguments from interface
    //PARSE_ARGS;

	std::cout << argc << ": " << std::endl;
	for( int i = 0; i < argc; i++ )
	{
		std::cout << i << ": " << argv[i] << std::endl;
	}
	std::cout << std::endl;

	// read in arguments
	char * outputFilename = "output.csv";
	char * labelMap1 = "";
	char * labelMap2 = "";
	char * labelMap3 = "";
	char * labelMap4 = "";
	char * labelMap5 = "";

	if( argc < 4 )
	{
		std::cout << "Please enter more inputs." << std::endl;
		std::cout << argv[0] << " outputFilename labelMap1" << std::endl;
		return EXIT_FAILURE;
	}
	if( argc > 7 )
	{
		std::cout << "Too many arguments." << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Inputs correctly identified." << std::endl;

	// collect label map filenames into list
	std::list< char * >	lmFilenames;
	std::list< char * >::iterator it = lmFilenames.begin();
	for( int i = 2; i < argc; i++ )
	{
		//std::cout << i << ": " << argv[i] << std::endl;
		lmFilenames.push_back(argv[i]);
	}

	std::cout << "Filenames stored:" << std::endl;
	// print out list of filenames
	for( it = lmFilenames.begin(); it != lmFilenames.end(); ++it )
	{
		std::cout << *it << std::endl;
	}
	std::cout << std::endl;

	// read in images and store in list
	typedef itk::Image< unsigned int, 3 >			LabelImageType;
	typedef itk::ImageFileReader< LabelImageType >	ImageReaderType;
	ImageReaderType::Pointer reader = ImageReaderType::New();
	
	// create list
	std::list< LabelImageType::Pointer >		lmImages;
	std::list< LabelImageType::Pointer >::iterator it2 = lmImages.begin();
	it = lmFilenames.begin();

	//std::cout << std::endl;
	//std::cout << "Reader created." << std::endl;
	//std::cout << std::endl;

	// read in images
	while( it != lmFilenames.end() )
	{
		//std::cout << *it << std::endl;
		reader->SetFileName( *it );
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

		LabelImageType::Pointer image = reader->GetOutput();
		lmImages.push_back(image);
		++it;
	}

	std::cout << "Images stored." << std::endl;

	// instantiate the overlap filter
	typedef itk::LabelOverlapMeasuresImageFilter< LabelImageType >	OverlapFilter;
	OverlapFilter::Pointer overlapFilter = OverlapFilter::New();

	// create arrays to store values
	int n = lmFilenames.size();
	std::cout << "Number of label maps: " << n << std::endl;
	/*
	OverlapFilter::RealType *dice = new OverlapFilter::RealType[n*n];
	OverlapFilter::RealType *jaccard = new OverlapFilter::RealType[n*n];
	*/

	//std::cout << "dice and jaccard arrays created." << std::endl;

	// initialize iterators
	std::list< LabelImageType::Pointer >::iterator it3 = lmImages.begin();
	it2 = lmImages.begin();

	std::ofstream outputFile;
	outputFile.open( outputFilename );

	// iterate through label maps and compare with all other label maps
	for( int i=0; i<n; i++)
	{
		//std::cout << std::endl;
		//std::cout << "Source: " << (*(*it2)).GetNameOfClass() << std::endl;
		//overlapFilter->SetSourceImage( *it2 );
		it3 = lmImages.begin();

		for( int j=0; j<n; j++)
		{
			//std::cout << std::endl;
			//std::cout << "i: " << i << "  j: " << j << std::endl;
			// calculate index
			//int ind = (n)*i+j;
			//std::cout << ind << std::endl;

			//std::cout << "Target: " << (*(*it3)).GetNameOfClass() << std::endl;
			
			// insert labelmaps into filter
			//overlapFilter->SetTargetImage( *it3 );
			//overlapFilter->Update();

			// get DICE coefficient
			//std::cout << "DICE: " << overlapFilter->GetDiceCoefficient() << std::endl;
			//dice[ind] = overlapFilter->GetDiceCoefficient();

			// get Jaccard coefficient
			//std::cout << "JACC: " << overlapFilter->GetJaccardCoefficient() << std::endl;
			//jaccard[ind] = overlapFilter->GetJaccardCoefficient();

			// increase iterator

			LabelOverlapMeasures< LabelImageType >( *it2, *it3, outputFile );
			++it3;
		}
		// increase iterator
		++it2;
	}
/*
	// write results out to file
	std::ofstream outputFile;
	outputFile.open( outputFilename );

	// write out DICE results
	outputFile << "DICE\n";
	outputFile << "0,1,2,3,4,5\n";
	for( int i=1; i<=n; i++)
	{
		outputFile << i << ",";
		for( int j=0; j<n; j++)
		{
			// output data from DICE
			int ind = (n+1)*(i-1)+j;
			outputFile << dice[ind] << ",";
		}
		outputFile << "\n";
	}

	// write out JACCARD results
	outputFile << "\n\nJACCARD\n";
	outputFile << "0,1,2,3,4,5\n";
	for( int i=1; i<=n; i++)
	{
		outputFile << i << ",";
		for( int j=0; j<n; j++)
		{
			// output data from DICE
			int ind = (n+1)*(i-1)+j;
			outputFile << dice[ind] << ",";
		}
		outputFile << "\n";
	}
	
	// clean up memory
	delete[] dice;
	delete[] jaccard;
*/
    return EXIT_SUCCESS;
}
