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
		this->m_fixedImage = ImageType::New();
		this->m_movingImage = ImageType::New();
		this->m_transformedImage = ImageType::New();
		this->m_hardenTransform = false;
		this->m_resampleImage = true;
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

	void ManageTransformsFilter::HardenTransformOn()
	{
		this->m_hardenTransform = true;
		return;
	}

	void ManageTransformsFilter::ResampleImageOn()
	{
		this->m_resampleImage = true;
		return;
	}

	ManageTransformsFilter::ImageType::Pointer ManageTransformsFilter::GetTransformedImage()
	{
		return this->m_transformedImage;
	}

	ManageTransformsFilter::MaskImageType::Pointer ManageTransformsFilter::GenerateMaskFromROI( const char * filename )
	{
		double * roi = ExtractROIPoints( filename );
		MaskImageType::Pointer maskImage = CreateMask( roi );

		std::cout << "Mask has been generated." << std::endl;

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

	// create the mask as an image based on the properties of the input image
	ManageTransformsFilter::MaskImageType::Pointer ManageTransformsFilter::CreateMask( double * roi )
	{
		// input fixed image properties into mask image
		MaskImageType::Pointer maskImage = MaskImageType::New();
		maskImage->SetRegions( this->fixedImage->GetLargestPossibleRegion() );
		maskImage->SetOrigin( this->fixedImage->GetOrigin() );
		maskImage->SetSpacing( this->fixedImage->GetSpacing() );
		maskImage->SetDirection( this->fixedImage->GetDirection() );
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

	// apply current transform on file to the header information of the input image
	void ManageTransformsFilter::HardenTransform()
	{
		// get image properties
		ImageType::PointType origin = this->movingImage->GetOrigin();
		ImageType::SpacingType spacing = this->movingImage->GetSpacing();
		ImageType::DirectionType direction = this->movingImage->GetDirection();

		// print out old parameters
/*		std::cout << "OLD PARAMETERS" << std::endl;
		std::cout << "Origin: [" << origin[0] << ", " << origin[1] << ", " << origin[2] << "]" << std::endl;
		std::cout << "Spacing: [" << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << "]" << std::endl;
		std::cout << "Direction: [" << *direction[0] << ", " << *direction[1] << ", " << *direction[2] << std::endl;
		std::cout << "            " << *direction[3] << ", " << *direction[4] << ", " << *direction[5] << std::endl;
		std::cout << "            " << *direction[6] << ", " << *direction[7] << ", " << *direction[8] << "]" << std::endl;
*/
		// get transform parameters
		TransformType::TranslationType translation = this->m_compositeTransform->GetTranslation();
		TransformType::ScaleVectorType scale = this->m_compositeTransform->GetScale();
		TransformType::VersorType versor = this->m_compositeTransform->GetVersor();

		// apply parameters to image
		for( int i = 0; i < 3; i++ )
		{
			spacing[i] *= scale[i];
			origin[i] += translation[i]*scale[i];
		}

		// apply rotation to image
		ImageType::DirectionType newDirection = direction*versor.GetMatrix();
	
/*		std::cout << "\nNEW PARAMETERS" << std::endl;
		std::cout << "Origin: [" << origin[0] << ", " << origin[1] << ", " << origin[2] << "]" << std::endl;
		std::cout << "Spacing: [" << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << "]" << std::endl;
		std::cout << "Direction: [" << *direction[0] << ", " << *direction[1] << ", " << *direction[2] << std::endl;
		std::cout << "            " << *direction[3] << ", " << *direction[4] << ", " << *direction[5] << std::endl;
		std::cout << "            " << *direction[6] << ", " << *direction[7] << ", " << *direction[8] << "]" << std::endl;
*/
		// allocate hardening filter
		typedef itk::ChangeInformationImageFilter< ImageType > HardenTransformFilter;
		HardenTransformFilter::Pointer harden = HardenTransformFilter::New();
		harden->SetInput( this->m_movingImage );

		// set new parameters
		harden->SetOutputSpacing( spacing );
		harden->SetOutputOrigin( origin );
		harden->SetOutputDirection( newDirection );

		// turn change flags on
		harden->ChangeSpacingOn();
		harden->ChangeOriginOn();
		harden->ChangeDirectionOn();

		harden->Update();
		this->m_transformedImage = harden->GetOutput();
		return; 
	}

	void ManageTransformsFilter::ResampleImage()
	{
		// set up resampling object
		typedef itk::ResampleImageFilter< ImageType, ImageType >	ResampleFilterType;
		ResampleFilterType::Pointer resample = ResampleFilterType::New();

		// define image resampling with respect to fixed image
		resample->SetSize( this->m_fixedImage->GetLargestPossibleRegion().GetSize() );
		resample->SetOutputOrigin( this->m_fixedImage->GetOrigin() );
		resample->SetOutputSpacing( this->m_fixedImage->GetSpacing() );
		resample->SetOutputDirection( this->m_fixedImage->GetDirection() );

		// input parameters
		resample->SetInput( this->m_movingImage );
		resample->SetTransform( this->m_compositeTransform );

		// apply
		resample->Update();
		this->m_transformedImage = resample->GetOutput();
		return;
	}
	
} // end namespace

#endif