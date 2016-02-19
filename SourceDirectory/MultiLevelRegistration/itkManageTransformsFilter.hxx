#ifndef __itkManageTransformsFilter_hxx
#define __itkManageTransformsFilter_hxx_hxx

#include "itkManageTransformsFilter.h"
#include "itkImageRegionIterator.h"

namespace itk
{
	// constructor
	ManageTransformsFilter::ManageTransformsFilter()
	{
		this->m_compositeTransform = CompositeTransformType::New();
	}

	void ManageTransformsFilter::AddTransform( TransformType::Pointer transform )
	{
		this->m_compositeTransform->AddTransform( transform );

		return;
	}

	// print transform to screen
	void ManageTransformsFilter::Print()
	{
		std::cout << std::endl;
		std::cout << this->m_compositeTransform << std::endl;
		return;
	}

	// save composite transform
	void ManageTransformsFilter::SaveTransform()
	{
		WriteOutTransform< CompositeTransformType >( "transforms.tfm", this->m_compositeTransform );
		return;
	}

	// set fixed and moving images
	void ManageTransformsFilter::SetImages( ImageType::Pointer fixedImage, ImageType::Pointer movingImage )
	{
		this->m_fixedImage = fixedImage;
		this->m_movingImage = movingImage;
		
		std::cout << "Images set." << std::endl;
		return;
	}

	ManageTransformsFilter::MaskImageType::Pointer ManageTransformsFilter::GenerateMaskFromROI( const char * filename )
	{
		double * roi = ExtractROIPoints( filename );
		MaskImageType::Pointer maskImage = CreateMask( roi );

		return maskImage;
	}

	// extract point values from the slicer ROI file
	double * ManageTransformsFilter::ExtractROIPoints( const char * filename )
	{
		// instantiate ROI array
		static double roi[] = {0.0,0.0,0.0,0.0,0.0,0.0};
		bool fullROI = false; // denotes that ROI array is full
		int numberOfPoints = 0;

		// open file and extract lines
		std::ifstream file( filename );
		std::string line;
		// iterate through file
		while( getline( file,line ) )
		{
			// look at uncommented lines only unless point is found
			if( line.compare(0,1,"#") != 0 && !fullROI )
			{
				// allocate position array and indices
				int positionsOfBars[4] = {0};
				int numberOfBars = 0;
				int positionOfBar = 0;
				
				// iterate through the line in the file to find the | (bar) locations
				// example line: point|18.8396|305.532|-458.046|1|1
				for( std::string::iterator it = line.begin(); it != line.end(); ++it )
				{
					// only note the first 4 locations
					if( (*it == '|') && numberOfBars < 4 )
					{
						positionsOfBars[numberOfBars] = positionOfBar;
						++numberOfBars;
					}
					++positionOfBar;
				}

				// extract each point and place into ROI array
				for( int i = 0; i < 3; ++i )
				{
					roi[numberOfPoints] = atof( line.substr( positionsOfBars[i]+1, positionsOfBars[i+1]-positionsOfBars[i]-1 ).c_str() );
					++numberOfPoints;

					// set flag to false if the roi array is filled
					if( numberOfPoints > 5 )
					{
						fullROI = true;
					}
				}
			}	
		}
		// array is output as [ centerx, centery, centerz, radiusx, radiusy, radiusz ]
		return roi;
	}

	// create the mask as an image
	ManageTransformsFilter::MaskImageType::Pointer ManageTransformsFilter::CreateMask( double * roi )
	{
		// input fixed image properties into mask image
		MaskImageType::Pointer maskImage = MaskImageType::New();
		maskImage->SetRegions( this->m_fixedImage->GetLargestPossibleRegion() );
		maskImage->SetOrigin( this->m_fixedImage->GetOrigin() );
		maskImage->SetSpacing( this->m_fixedImage->GetSpacing() );
		maskImage->SetDirection( this->m_fixedImage->GetDirection() );
		maskImage->Allocate();

		// extract center and radius
		double c[3] = {-*(roi),-*(roi+1),*(roi+2)};
		double r[3] = {*(roi+3),*(roi+4),*(roi+5)};
		
		// create size of mask according to the roi array
		// set start index of mask according to the roi array
		MaskImageType::PointType startPoint, endPoint;
		startPoint[0] = c[0] - r[0];
		startPoint[1] = c[1] - r[1];
		startPoint[2] = c[2] - r[2];

		// find end index
		endPoint[0] = c[0] + r[0];
		endPoint[1] = c[1] + r[1];
		endPoint[2] = c[2] + r[2];

		// convert to indices
		MaskImageType::IndexType startIndex, endIndex;
		maskImage->TransformPhysicalPointToIndex( startPoint, startIndex );
		maskImage->TransformPhysicalPointToIndex( endPoint, endIndex );

		// plug into region
		MaskImageType::SizeType regionSize;
		regionSize[0] = abs( startIndex[0] - endIndex[0] );
		regionSize[1] = abs( startIndex[1] - endIndex[1] );
		regionSize[2] = abs( startIndex[2] - endIndex[2] );

		MaskImageType::RegionType maskedRegion;
		maskedRegion.SetSize( regionSize );
		maskedRegion.SetIndex( startIndex );

		// iterate over region and set pixels to white
		itk::ImageRegionIterator< MaskImageType > it( maskImage, maskedRegion );
		while( !it.IsAtEnd() )
		{
			it.Set( 255 );
			++it;
		}

		return maskImage;
	}
			
	
} // end namespace

#endif