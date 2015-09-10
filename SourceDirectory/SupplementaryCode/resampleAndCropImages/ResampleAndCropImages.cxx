/*
 * Emily Hammond
 * 9/1/2015
 *
 * The goal of this code is to read in a bunch of images and their corresponding 
 * transformations and label maps, resample the images and crop the data with
 * respect to the reference image listed
 */

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"

// apply transform
#include "itkScaleVersor3DTransform.h"
#include "itkResampleImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"

// extract ROI image
#include "itkPoint.h"

#include <itksys/SystemTools.hxx>
#include <fstream>
#include <string>

// function to read in images
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
		//std::cout << std::endl;
		//std::cout << ImageFilename << " has been read in." << std::endl;
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

// function to write out images
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
		//std::cout << std::endl;
		//std::cout << ImageFilename << " has been written." << std::endl;
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}

	std::cout << ImageFilename << " written to file." << std::endl;

	// return output
	return EXIT_SUCCESS;
}

// function to extract ROI values
double * ExtractROI( const char * filename )
{
	// instantiate ROI array
	static double roi[] = {0.0,0.0,0.0,0.0,0.0,0.0};
	int m = 0;
	bool flag = true; // denoting that ROI is full

	// open file and extract lines
	std::ifstream file( filename );
	std::string line;
	// iterate through file
	while( getline( file, line ) )
	{
		// look at lines that are not commented
		if( line.compare(0,1,"#") != 0 && flag)
		{
			// allocate position array
			int pos [4] = {0};
			int n = 0;
			int posN = 0;

			// create iterator for line to fine | locations
			for( std::string::iterator it = line.begin(); it != line.end(); ++it )
			{
				if( (*it == '|') && (n < 4) )
				{
					pos[n] = posN;
					++n;
				}
				++posN;
			}

			// extract the point to the roi
			for( int i = 0; i < 3; ++i )
			{
				std::string roiT = line.substr(pos[i]+1,(pos[i+1]-pos[i]-1));
				roi[m] = atof(roiT.c_str());
				++m;

				// set flag to false
				if( m == 6 ){ flag = false; }
			}
		}
	}

	//std::cout << std::endl;
	//std::cout << filename << " has been read." << std::endl;

	return roi;
}

// ***********************Main function********************************
int main( int argc, char * argv[] )
{
	std::string resultsDir;

	// instantiate variables to be used
	typedef itk::Image< float, 3 >					ImageType;
	typedef itk::Image< unsigned int, 3 >			LabelMapType;
	typedef itk::ScaleVersor3DTransform< double >	TransformType;

	// transform file reader
	typedef itk::TransformFileReader				TransformReaderType;
	TransformReaderType::Pointer transformReader = TransformReaderType::New();
	
	// reference image (CT TP1 image) and label image
	ImageType::Pointer referenceImage = ImageType::New();
	LabelMapType::Pointer referenceLabelMap = LabelMapType::New();
	
	// moving image and label map
	ImageType::Pointer movingImage = ImageType::New();
	LabelMapType::Pointer movingLabelMap = LabelMapType::New();
	std::string movingImageID;
	std::string referenceImageID;
	std::string organID;

	// transform and ROI
	TransformType::Pointer transform = TransformType::New();
	double * roi = NULL;

	// open input file
	std::ifstream myfile( argv[1] );
	bool IMflag = false;
	bool TRflag = false;
	bool RFflag = false;
	bool ROIflag = false;
	bool cropFlag = false;
	bool DIRflag = false;
	bool ORflag = false;

	// read in line from file
	std::string line;
	while( getline( myfile, line ) )
	{
		// check if line has a # at the beginning -> comments
		if( line.compare( 0,1,"#" ) != 0 )
		{
			// extract the first characters until a space
			size_t pos2 = line.find(' ', 0);
			std::string id = line.substr(0, pos2);	// identifier
			std::string path = line.substr(pos2+1);	// image path

			// obtain path to reference directory
			if( id.compare( "ResultsDirectory" ) == 0 )
			{
				resultsDir = path;
				DIRflag = true;
				//std::cout << "RESULTS DIRECTORY: " << path << std::endl;
			}

			// know to crop the images
			if( id.compare( "Crop" ) == 0 )
			{
				cropFlag = atoi(path.c_str());
			}

			// organ identifier
			if( id.compare( "Organ" ) == 0 )
			{
				organID = path;
				ORflag = true;
			}

			// gather all the reference information
			if( id.compare( "ReferenceImage" ) == 0 )
			{
				// read in image
				referenceImage = ReadInImage< ImageType >( path.c_str() );
				std::size_t pos1 = path.find_last_of("/\\");
				std::size_t pos2 = path.rfind(".");
				referenceImageID = path.substr(pos1+1,(pos2-pos1-1));
				//std::cout << "REFERENCE IMAGE: " << path << std::endl;

				// read in label map
				std::string labelMapFilename = path.substr(0,pos2) + "-label.mhd";
				referenceLabelMap = ReadInImage< LabelMapType >( labelMapFilename.c_str() );
				//std::cout << "REFERENCE LABEL MAP: " << labelMapFilename << std::endl;
				// set reference flag
				RFflag = true;
				
			}
			else if( id.compare( "ReferenceROI" ) == 0 )
			{
				roi = ExtractROI( path.c_str() );
				// set roi flag
				ROIflag = true;
			}

			// collect the moving image data (if CT)
			if( id.compare( "CT" ) == 0 || id.compare( "MR" ) == 0 )
			{
				// read in image
				movingImage = ReadInImage< ImageType >( path.c_str() );
				std::size_t pos1 = path.find_last_of("/\\");
				std::size_t pos2 = path.rfind(".");
				movingImageID = path.substr(pos1+1,(pos2-pos1-1));
				//std::cout << "MOVING IMAGE: " << path << std::endl;

				// read in label map
				std::string labelMapFilename = path.substr(0,pos2) + "-label.mhd";
				movingLabelMap = ReadInImage< LabelMapType >( labelMapFilename.c_str() );
				//std::cout << "MOVING LABEL MAP: " << labelMapFilename << std::endl;
				// set IM flag
				IMflag = true;
			}

			// obtain results directory
			if( id.compare( "Transform" ) == 0 )
			{
				// access transform and read in
				transformReader->SetFileName( path.c_str() );
				try
				{
					transformReader->Update();
					//std::cout << std::endl;
					//std::cout << path << " has been read in." << std::endl;
				}
				catch(itk::ExceptionObject & err)
				{
					std::cerr << "Exception Object Caught!" << std::endl;
					std::cerr << err << std::endl;
					std::cerr << std::endl;
				}

				// check to make sure there is only one transform here
				TransformReaderType::TransformListType * transforms = transformReader->GetTransformList();
				if( transforms->size() != 1 )
				{
					std::cerr << "There is more than 1 transform in initial transform file" << std::endl;
					return EXIT_FAILURE;
				}

				// store initial transform into transform
				TransformReaderType::TransformListType::const_iterator it = transforms->begin();
				transform = static_cast< TransformType * >( (*it).GetPointer() );
				//std::cout << "TRANSFORM: " << path << std::endl;
				// set transform flag
				TRflag = true;
			}
/*
			std::cout << std::endl;
			std::cout << line << std::endl;
			std::cout << "Image flag: " << IMflag << std::endl;
			std::cout << "ReferenceImage flag: " << RFflag << std::endl;
			std::cout << "ROI flag: " << ROIflag << std::endl;
			std::cout << "Transform flag: " << TRflag << std::endl;
			std::cout << "Crop image flag: " << cropFlag << std::endl;
			std::cout << "Results dir flag: " << DIRflag << std::endl;
*/
			// apply transform and extract image from ROI
			if( RFflag && IMflag && TRflag && ROIflag && DIRflag && ORflag )
			{
				// resample image with respect to reference image
				typedef itk::ResampleImageFilter< ImageType, ImageType >	ResampleFilterType;
				ResampleFilterType::Pointer resampleImage = ResampleFilterType::New();

				// define image with respect to reference image
				resampleImage->SetSize( referenceImage->GetLargestPossibleRegion().GetSize() );
				resampleImage->SetOutputOrigin( referenceImage->GetOrigin() );
				resampleImage->SetOutputSpacing( referenceImage->GetSpacing() );
				resampleImage->SetOutputDirection( referenceImage->GetDirection() );
				resampleImage->SetInput( movingImage );
				resampleImage->SetTransform( transform );
				resampleImage->Update();

				// extract region of interest from resampled image
				// extract center and radii from the roi values
				double c[3] = {-*(roi),-*(roi+1),*(roi+2)};
				double r[3] = {*(roi+3),*(roi+4),*(roi+5)};
				
				// create lower point and upper point
				itk::Point<double, 3> pt1;
				itk::Point<double, 3> pt2;

				// create lower point
				pt1[0] = c[0] + r[0]; //93
				pt1[1] = c[1] + r[1]; //-153
				pt1[2] = c[2] + r[2]; //-649

				// create upper point
				pt2[0] = c[0] - r[0]; //17
				pt2[1] = c[1] - r[1]; //-213
				pt2[2] = c[2] - r[2]; //-787

				// transform points to physical coordinates
				itk::Index<3> idx1, idx2;
				resampleImage->GetOutput()->TransformPhysicalPointToIndex( pt1, idx1 );
				resampleImage->GetOutput()->TransformPhysicalPointToIndex( pt2, idx2 );

				// ensure idx2 values are greater than idx1 values
				for( int i = 0; i < 3; ++i )
				{
					if( idx1[i] > idx2[i] )
					{
						const itk::IndexValueType t = idx1[i];
						idx1[i] = idx2[i];
						idx2[i] = t;
					}
				}

				// define desired region
				itk::ImageRegion<3> desiredRegion;
				desiredRegion.SetIndex(idx1);
				desiredRegion.SetUpperIndex(idx2);
				desiredRegion.Crop( resampleImage->GetOutput()->GetLargestPossibleRegion() );

				// extract the image
				typedef itk::ExtractImageFilter< ImageType, ImageType > ExtractFilterType;
				ExtractFilterType::Pointer extractImage = ExtractFilterType::New();
				extractImage->SetExtractionRegion( desiredRegion );

				if( cropFlag )
				{					
					extractImage->SetInput( resampleImage->GetOutput() );
					extractImage->SetDirectionCollapseToIdentity();
					extractImage->InPlaceOn();
					
					// update filter
					try
					{
						extractImage->Update();
					}
					catch(itk::ExceptionObject & err)
					{
						std::cerr << "Exception Object Caught!" << std::endl;
						std::cerr << err << std::endl;
						std::cerr << std::endl;
					}

					// write result to file
					std::string outputImageFilename = resultsDir + "\\" + movingImageID + "_" + organID + ".mhd";
					WriteOutImage< ImageType, ImageType >( outputImageFilename.c_str(), extractImage->GetOutput() );
				}
				else
				{
					std::string outputImageFilename = resultsDir + "\\" + movingImageID + "_resampled.mhd";
					WriteOutImage< ImageType, ImageType >( outputImageFilename.c_str(), resampleImage->GetOutput() );
				}

				// repeat with label maps
				typedef itk::ResampleImageFilter< LabelMapType, LabelMapType >	ResampleLMFilterType;
				ResampleLMFilterType::Pointer resampleLabelMap = ResampleLMFilterType::New();

				// define image with respect to reference image
				resampleLabelMap->SetSize( referenceImage->GetLargestPossibleRegion().GetSize() );
				resampleLabelMap->SetOutputOrigin( referenceImage->GetOrigin() );
				resampleLabelMap->SetOutputSpacing( referenceImage->GetSpacing() );
				resampleLabelMap->SetOutputDirection( referenceImage->GetDirection() );
				resampleLabelMap->SetInput( movingLabelMap );
				resampleLabelMap->SetTransform( transform );

				// threshold label map to just get one label
				typedef itk::BinaryThresholdImageFilter< LabelMapType, LabelMapType >	ThresholdLabelMapType;
				ThresholdLabelMapType::Pointer thresholdLabelMap = ThresholdLabelMapType::New();
				thresholdLabelMap->SetLowerThreshold( 1 );
				thresholdLabelMap->SetOutsideValue( 0 );
				thresholdLabelMap->SetInsideValue( 1 );
				thresholdLabelMap->SetInput( resampleLabelMap->GetOutput() );

				// extract the image
				typedef itk::ExtractImageFilter< LabelMapType, LabelMapType > ExtractLMFilterType;
				ExtractLMFilterType::Pointer extractLabelMap = ExtractLMFilterType::New();
				extractLabelMap->SetExtractionRegion( desiredRegion );

				if( cropFlag )
				{
					extractLabelMap->SetInput( thresholdLabelMap->GetOutput() );
					extractLabelMap->SetDirectionCollapseToIdentity();
					extractLabelMap->InPlaceOn();
					
					// update filter
					try
					{
						extractImage->Update();
					}
					catch(itk::ExceptionObject & err)
					{
						std::cerr << "Exception Object Caught!" << std::endl;
						std::cerr << err << std::endl;
						std::cerr << std::endl;
					}

					std::string outputLabelMapFilename = resultsDir + "\\" + movingImageID + "_" + organID + "-label.mhd";
					WriteOutImage< LabelMapType, LabelMapType >( outputLabelMapFilename.c_str(), extractLabelMap->GetOutput() );
				}
				else
				{
					std::string outputImageFilename = resultsDir + "\\" + movingImageID + "_resampled-label.mhd";
					WriteOutImage< LabelMapType, LabelMapType >( outputImageFilename.c_str(), resampleLabelMap->GetOutput() );
				}

				// set flags to false
				RFflag = false;
				IMflag = false;
				TRflag = false;
				ROIflag = false;
				cropFlag = false;
				DIRflag = false;
				ORflag = false;
			}
		}
	}

    return EXIT_SUCCESS;
}