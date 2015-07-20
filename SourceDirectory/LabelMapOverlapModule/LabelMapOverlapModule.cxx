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
#include "itkImageFileReader.h"
#include "itkLabelOverlapMeasuresImageFilter.h"

#include "itkAddImageFilter.h"

#include <iomanip>

template <unsigned int ImageDimension>
int LabelOverlapMeasures( int argc, char * argv[] )
{
	typedef unsigned int PixelType;
	typedef itk::Image<PixelType, ImageDimension> ImageType;

	typedef itk::ImageFileReader<ImageType>  ReaderType;
	typename ReaderType::Pointer reader1 = ReaderType::New();
	reader1->SetFileName( argv[2] );

	typename ReaderType::Pointer reader2 = ReaderType::New();
	reader2->SetFileName( argv[3] );

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

	typedef itk::LabelOverlapMeasuresImageFilter<ImageType> FilterType;
	typename FilterType::Pointer filter = FilterType::New();
	filter->SetSourceImage( reader1->GetOutput() );
	filter->SetTargetImage( reader2->GetOutput() );
	std::cout << "\n" << argv[3] << " has been read in.\n";
	std::cout << "\n" << argv[2] << " has been read in.\n";

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

	typename FilterType::MapType labelMap = filter->GetLabelSetMeasures();
	typename FilterType::MapType::const_iterator it;
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

	// define image and reader
	typedef unsigned int PixelType;
	int Dimension = 3;
	typedef itk::Image<PixelType, Dimension> ImageType;

	typedef itk::ImageFileReader<ImageType>  ReaderType;
	typename ReaderType::Pointer reader = ReaderType::New();

	// create map for images to contain filename and corresponding image
	std::map< char *, ImageType::Pointer > images;

	for( int n = 2; n > argc; ++n )
	{
		// insert filename
		reader->SetFileName( argv[n] );
		// read in image
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
		// insert filename and image into map
		images.insert( std::pair< char *, ImageType::Pointer >( argv[n], reader->GetOutput() );
	}
  
	switch( Dimension )
	{
		case 2:
			std::cout << "DIMENSION = 2" << std::endl;
			LabelOverlapMeasures<2>( argc, argv );
			break;
		case 3:
			std::cout << "DIMENSION = 3" << std::endl;
			LabelOverlapMeasures<3>( argc, argv );
			break;
		default:
			std::cerr << "Unsupported dimension" << std::endl;
			exit( EXIT_FAILURE );
   }
}