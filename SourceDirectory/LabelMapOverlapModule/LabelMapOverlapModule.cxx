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
#include "itkBinaryThresholdImageFilter.h"
#include "itkHausdorffDistanceImageFilter.h"
#include <iomanip>

// write function to perform the overlap measures
template< typename ImageType >
int LabelOverlapMeasures( typename ImageType::Pointer source, typename ImageType::Pointer target, std::ofstream & file )
{
	// instantiate filter and insert images
	typedef itk::LabelOverlapMeasuresImageFilter< ImageType >  FilterType;
	FilterType::Pointer filter = FilterType::New();
	filter->SetSourceImage( source );
	filter->SetTargetImage( target );

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
	file << " ** All Labels ** " << std::endl;
	file << ",Total,Union (jaccard),Mean (dice),Volume sim.,False negative,False positive\n";
	file << "," << filter->GetTotalOverlap();
	file << "," << filter->GetUnionOverlap();
	file << "," << filter->GetMeanOverlap();
	file << "," << filter->GetVolumeSimilarity();
	file << "," << filter->GetFalseNegativeError();
	file << "," << filter->GetFalsePositiveError();
	file << std::endl;

	file << " ** Individual Labels ** " << std::endl;
	file << "Label,Target,Union (jaccard),Mean (dice),Volume sim.,False negative,False positive,Hausdorff distance,Average distance\n";

	// for each individual labels
	FilterType::MapType labelMap = filter->GetLabelSetMeasures();
	FilterType::MapType::const_iterator it;
	for( it = labelMap.begin(); it != labelMap.end(); ++it )
    {
		// ignore label 0 (background)
		if( (*it).first == 0 )
		{
			continue;
		}

		// identify label
		int label = (*it).first;

		// write out to file
		file << label;
		file << "," << filter->GetTargetOverlap( label );
		file << "," << filter->GetUnionOverlap( label );
		file << "," << filter->GetMeanOverlap( label );
		file << "," << filter->GetVolumeSimilarity( label );
		file << "," << filter->GetFalseNegativeError( label );
		file << "," << filter->GetFalsePositiveError( label );

		// isolate label in source image
		typedef itk::BinaryThresholdImageFilter< ImageType, ImageType > ThresholdType;
		typename ThresholdType::Pointer disSource = ThresholdType::New();
		disSource->SetInput( filter->GetSourceImage() );
		disSource->SetLowerThreshold( label );
		disSource->SetUpperThreshold( label );
		disSource->SetInsideValue( static_cast< ImageType::PixelType >( 1 ) );
		disSource->SetOutsideValue( static_cast< ImageType::PixelType >( 0 ) );
		disSource->Update();

		// isolate label in target image
		typename ThresholdType::Pointer disTarget = ThresholdType::New();
		disTarget->SetInput( filter->GetTargetImage() );
		disTarget->SetLowerThreshold( label );
		disTarget->SetUpperThreshold( label );
		disTarget->SetInsideValue( static_cast< ImageType::PixelType >( 1 ) );
		disTarget->SetOutsideValue( static_cast< ImageType::PixelType >( 0 ) );
		disTarget->Update();

		// calculate Hausdorff distance
		typedef itk::HausdorffDistanceImageFilter< ImageType, ImageType >	DistanceType;
		typename DistanceType::Pointer distance = DistanceType::New();
		distance->SetInput1( disSource->GetOutput() );
		distance->SetInput2( disTarget->GetOutput() );
		distance->Update();
		//distance->Print( std::cout );

		file << "," << distance->GetHausdorffDistance();
		file << "," << distance->GetAverageHausdorffDistance();
		file << std::endl;
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

	// open file to place results into
	std::ofstream file;
	file.open( argv[1] );

	// perform comparisons
	for( int n = 2; n < argc; ++n )
	{
		for( int m = n+1; m < argc; ++m )
		{
			// define image and reader
			typedef unsigned int PixelType;
			int Dimension = 3;
			typedef itk::Image< PixelType, 3 > ImageType;

			typedef itk::ImageFileReader< ImageType >  ReaderType;
			ReaderType::Pointer reader1 = ReaderType::New();
			reader1->SetFileName( argv[n] );
			ReaderType::Pointer reader2 = ReaderType::New();
			reader2->SetFileName( argv[m] );

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

			file << std::endl;
			file << "Source: " << argv[n] << std::endl;
			file << "Target: " << argv[m] << std::endl;
			file << std::endl;
			LabelOverlapMeasures<ImageType>( reader1->GetOutput() , reader2->GetOutput(), file );
			file << std::endl;
		}
	}

	file.close();

	return EXIT_SUCCESS;
}