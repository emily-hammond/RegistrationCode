/*
Author: Emily Hammond
Date: 7.1.2015

The purpose of this program is to calculate the dice and jaccard coefficients of two label maps.

Goals:
1. read in n label maps (defined by number of input arguments) and store into a list
2. obtain the dice/jaccard coefficients to get overlap of n label maps
3. write out results

*/

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkLabelOverlapMeasuresImageFilter.h"
#include <iomanip>

// write function to perform the overlap measures
int LabelOverlapMeasures( char * filename1 , char * filename2 )
{
	// define image and reader
	typedef unsigned int PixelType;
	int Dimension = 3;
	typedef itk::Image< PixelType, 3 > ImageType;

	typedef itk::ImageFileReader< ImageType >  ReaderType;
	ReaderType::Pointer reader1 = ReaderType::New();
	reader1->SetFileName( filename1 );
	ReaderType::Pointer reader2 = ReaderType::New();
	reader2->SetFileName( filename2 );

	// read in images
	try
	{
		reader1->Update();
		reader2->Update();
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}

	// instantiate filter and insert images
	typedef itk::LabelOverlapMeasuresImageFilter< ImageType >  FilterType;
	FilterType::Pointer filter = FilterType::New();
	filter->SetSourceImage( reader1->GetOutput() );
	filter->SetTargetImage( reader2->GetOutput() );

	// update filter
	try
	{
		filter->Update();
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}

	// write out results to the screen
	// for all labels
	std::cout << "                                          "
            << "************ All Labels *************" << std::endl;
	std::cout << std::setw( 10 ) << "   "
    << std::setw( 17 ) << "Total"
    << std::setw( 17 ) << "Union (jaccard)"
    << std::setw( 17 ) << "Mean (dice)"
    << std::setw( 17 ) << "Volume sim."
    << std::setw( 17 ) << "False negative"
    << std::setw( 17 ) << "False positive" << std::endl;
	std::cout << std::setw( 10 ) << "   ";
	std::cout << std::setw( 17 ) << filter->GetTotalOverlap();
	std::cout << std::setw( 17 ) << filter->GetUnionOverlap();
	std::cout << std::setw( 17 ) << filter->GetMeanOverlap();
	std::cout << std::setw( 17 ) << filter->GetVolumeSimilarity();
	std::cout << std::setw( 17 ) << filter->GetFalseNegativeError();
	std::cout << std::setw( 17 ) << filter->GetFalsePositiveError();
	std::cout << std::endl;

	std::cout << "                                       "
            << "************ Individual Labels *************" << std::endl;
	std::cout << std::setw( 10 ) << "Label"
            << std::setw( 17 ) << "Target"
            << std::setw( 17 ) << "Union (jaccard)"
            << std::setw( 17 ) << "Mean (dice)"
            << std::setw( 17 ) << "Volume sim."
            << std::setw( 17 ) << "False negative"
            << std::setw( 17 ) << "False positive" << std::endl;

	// for each individual labels
	FilterType::MapType labelMap = filter->GetLabelSetMeasures();
	FilterType::MapType::const_iterator it;
	for( it = labelMap.begin(); it != labelMap.end(); ++it )
    {
		if( (*it).first == 0 )
		{
			continue;
		}

		int label = (*it).first;

		std::cout << std::setw( 10 ) << label;
		std::cout << std::setw( 17 ) << filter->GetTargetOverlap( label );
		std::cout << std::setw( 17 ) << filter->GetUnionOverlap( label );
		std::cout << std::setw( 17 ) << filter->GetMeanOverlap( label );
		std::cout << std::setw( 17 ) << filter->GetVolumeSimilarity( label );
		std::cout << std::setw( 17 ) << filter->GetFalseNegativeError( label );
		std::cout << std::setw( 17 ) << filter->GetFalsePositiveError( label );
		std::cout << std::endl;
    }

	return EXIT_SUCCESS;
}

int main( int argc, char *argv[] )
{
	// perform error handling on inputs
	if( argc < 4 )
    {
		std::cerr << "Usage: " << argv[0] << " outputFile sourceImage targetImage" << std::endl;
		return EXIT_FAILURE;
    }

	// perform comparisons
	for( int n = 2; n < argc; ++n )
	{
		for( int m = n+1; m < argc; ++m )
		{
			std::cout << std::endl;
			std::cout << "Source: " << argv[n] << std::endl;
			std::cout << "Target: " << argv[m] << std::endl;
			std::cout << std::endl;
			LabelOverlapMeasures( argv[n], argv[m] );
			std::cout << std::endl;
		}
	}

	return EXIT_SUCCESS;
}