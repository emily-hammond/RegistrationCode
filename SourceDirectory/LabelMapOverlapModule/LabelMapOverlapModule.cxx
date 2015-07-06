/*
Author: Emily Hammond
Date: 7.1.2015

The purpose of this program is to calculate the dice and jaccard coefficients of two label maps.

Goals:
1. read in n label maps
2. obtain the dice/jaccard coefficients to get overlap of n label maps

*/

#include "itkImage.h"
#include "itkLabelObject.h"
#include "itkLabelMap.h"
#include "itkLabelImageToLabelMapFilter.h"
#include "itkLabelOverlapMeasuresImageFilter.h"
#include "itkImageFileReader.h"

#include "LabelMapOverlapModuleCLP.h"

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

int main(int argc, char * argv[])
{
    PARSE_ARGS;

	// create a list of all the label map filenames
	std::list< std::string > labelMapFilenames;
	std::list< std::string >::iterator it = labelMapFilenames.begin();
	if( !labelMap1.empty() ){ labelMapFilenames.insert(it,labelMap1); }
	if( !labelMap2.empty() ){ labelMapFilenames.insert(it,labelMap2); }
	if( !labelMap3.empty() ){ labelMapFilenames.insert(it,labelMap3); }
	if( !labelMap4.empty() ){ labelMapFilenames.insert(it,labelMap4); }
	if( !labelMap5.empty() ){ labelMapFilenames.insert(it,labelMap5); }

	// print out filenames
	std::cout << "Label map filenames: " << std::endl;
	for( it = labelMapFilenames.begin(); it != labelMapFilenames.end(); ++it )
	{
		std::cout << *it << std::endl;
	}

	// define type of labelmap
	typedef itk::LabelObject< unsigned char, 3 >	LabelObjectType;
	typedef itk::LabelMap< LabelObjectType >		LabelMapType;

	
	// read in label maps
	std::list< LabelMapType::Pointer > labelMaps;
	std::list< LabelMapType::Pointer >::iterator it2 = labelMaps.begin();
	for( it = labelMapFilenames.begin(); it != labelMapFilenames.end(); ++it )
	{
		// read in image as a label map image
		typedef itk::Image< unsigned char, 3 > LabelImageType;
		LabelImageType::Pointer labelImage = ReadInImage<LabelImageType>( (*it).c_str() );

		// convert label image to label map
		typedef itk::LabelImageToLabelMapFilter< LabelImageType, LabelMapType > LabelImageToMapFilter;
		LabelImageToMapFilter::Pointer convertLM = LabelImageToMapFilter::New();
		convertLM->SetInput( labelImage );

		labelMaps.insert(it2, convertLM->GetOutput() );
		++it2;
	}

	std::cout << "Label maps: " << std::endl;
	for( it2 = labelMaps.begin(); it2 != labelMaps.end(); ++it2 )
	{
		std::cout << *it2 << std::endl;
	}

	/*
	// instantiate the overlap filter
	typedef itk::LabelOverlapMeasuresImageFilter< LabelMapType >	OverlapFilter;
	OverlapFilter::Pointer overlapFilter = OverlapFilter::New();

	// insert labelmaps into filter
	overlapFilter->SetSourceImage( labelMap1 );
	overlapFilter->SetTargerImage( labelMap2 );

	// get DICE coefficient
	OverlapFilter::RealType dice = overlapFilter->GetDiceCoefficient();

	// get Jaccard coefficient
	OverlapFilter::ReadType jaccard = overlapFilter->GetJaccardCoefficient();
	*/

    return EXIT_SUCCESS;
}
