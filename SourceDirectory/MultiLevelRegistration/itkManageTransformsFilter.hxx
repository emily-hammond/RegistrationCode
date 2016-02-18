#ifndef __itkManageTransformsFilter_hxx
#define __itkManageTransformsFilter_hxx_hxx

#include "itkManageTransformsFilter.h"

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
				int positionofBar = 0;
				
				// iterate through the line in the file to find the | (bar) locations
				// example line: point|18.8396|305.532|-458.046|1|1
				for( std::string::iterator it = line.begin(); it != line.end(); ++it )
				{
					// only note the first 4 locations
					if( (*it == "|") && numberOfBars < 4 )
					{
						positionsOfBars[numberOfBars] = positionOfBar;
						++numberOfBars;
						std::cout << positionOfBar << "  ";
					}
					++positionOfBar;
				}
				std::cout << std::endl;

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

	ManageTransformsFilter::MaskImageType::Pointer ManageTransformsFilter::CreateMask( double * roi )
	{
		MaskImageType::Pointer maskImage = MaskImageType::New();
		MaskImageType::RegionType region = this->m_fixedImage->GetLargestPossibleRegion();
		MaskImageType::OriginType origin = this->m_fixedImage->GetOrigin();
		MaskImageType::SpacingType spacing = this->m_fixedImage->GetSpacing();
		MaskImageType::SizeType size = this->m_fixedImage->GetLargestPossibleRegion().GetSize();

		return maskImage;
	}
			
	
} // end namespace

#endif