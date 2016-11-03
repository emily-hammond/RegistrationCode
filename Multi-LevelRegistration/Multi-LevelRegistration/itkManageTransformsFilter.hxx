#ifndef __itkManageTransformsFilter_hxx
#define __itkManageTransformsFilter_hxx_hxx

#include "itkManageTransformsFilter.h"
#include "itkImageRegionIterator.h"

namespace itk
{
	// constructor
	template< typename TPixelType >
	ManageTransformsFilter< typename TPixelType >::ManageTransformsFilter():
		m_FixedImage( ITK_NULLPTR ),	// defined by user
		m_MovingImage( ITK_NULLPTR ),	// defined by user
		m_MovingLabelMap( ITK_NULLPTR ),	// defined by user
		m_InitialTransform( ITK_NULLPTR ),	// defined by user
		m_ROIFilename( ITK_NULLPTR ),	// defined by user
		m_HardenTransform( false ),
		m_ResampleImage( false ),
		m_NearestNeighbor( false ),
		m_CropImage( false )
	{
		m_CompositeTransform = CompositeTransformType::New();
		m_FixedCroppedImage = ImageType::New();
		m_MovingCroppedImage = ImageType::New();
		m_ROI.assign(6, 0.0);
	}

	template< typename TPixelType >
	void ManageTransformsFilter< typename TPixelType >::SetROIFilename(const char * filename)
	{
		m_ROIFilename = filename;
		ExtractROIPoints();
		return;
	}

	template< typename TPixelType >
	void ManageTransformsFilter< typename TPixelType >::SetROI(std::vector<float> roi)
	{
		m_ROI = roi;
		return;
	}

	template< typename TPixelType >
	void ManageTransformsFilter< typename TPixelType >::AddTransform(TransformType::Pointer transform)
	{
		this->m_CompositeTransform->AddTransform( transform );
	}

	template< typename TPixelType >
	std::vector<float> ManageTransformsFilter< typename TPixelType >::ExtractROIPoints(const char * filename)
	{
		m_ROIFilename = filename;
		ExtractROIPoints();

		return m_ROI;
	}

	template< typename TPixelType >
	void ManageTransformsFilter< typename TPixelType >::Update()
	{
		// error checking
		if( !m_FixedImage )
		{
			itkExceptionMacro( << "FixedImage not present" );
		}
		if( !m_MovingImage )
		{
			itkExceptionMacro( << "MovingImage not present" );
		}

		// perform functionality
		if( this->m_ResampleImage )
		{
			this->m_TransformedImage = ResampleImage< typename ImageType >( this->m_MovingImage );
			std::cout << "Moving image resampled." << std::endl;

			// repeat for label map with nearest neighbor interpolation
			if( m_MovingLabelMap )
			{
				NearestNeighborInterpolateOn();
				this->m_TransformedLabelMap = ResampleImage< MaskImageType >( this->m_MovingLabelMap );
				NearestNeighborInterpolateOff();
				std::cout << "Moving label map resampled." << std::endl;
			}
		}
		if( this->m_HardenTransform )
		{
			HardenTransform();
		}
		if( this->m_CropImage )
		{
			this->m_MovingCroppedImage = CropImage< typename ImageType >( this->m_TransformedImage );
			this->m_FixedCroppedImage = CropImage< typename ImageType >(this->m_FixedImage);
			std::cout << "Images cropped." << std::endl;
			if( m_MovingLabelMap ) 
			{ 
				this->m_MovingCroppedLabelMap = CropImage< MaskImageType >( this->m_TransformedLabelMap );
				std::cout << "Moving label map cropped." << std::endl;
			} 
			if( m_FixedLabelMap ) 
			{ 
				this->m_FixedCroppedLabelMap = CropImage< MaskImageType >( this->m_FixedLabelMap );
				std::cout << "Fixed label map cropped." << std::endl;
			}
		}

		return;
	}

	// apply current transform on file to the header information of the input image
	template< typename TPixelType >
	void ManageTransformsFilter< typename TPixelType >::HardenTransform()
	{
		// get image properties
		/*ImageType::PointType origin = this->movingImage->GetOrigin();
		ImageType::SpacingType spacing = this->movingImage->GetSpacing();
		ImageType::DirectionType direction = this->movingImage->GetDirection();

		// print out old parameters
		std::cout << "OLD PARAMETERS" << std::endl;
		std::cout << "Origin: [" << origin[0] << ", " << origin[1] << ", " << origin[2] << "]" << std::endl;
		std::cout << "Spacing: [" << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << "]" << std::endl;
		std::cout << "Direction: [" << *direction[0] << ", " << *direction[1] << ", " << *direction[2] << std::endl;
		std::cout << "            " << *direction[3] << ", " << *direction[4] << ", " << *direction[5] << std::endl;
		std::cout << "            " << *direction[6] << ", " << *direction[7] << ", " << *direction[8] << "]" << std::endl;

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
	
		std::cout << "\nNEW PARAMETERS" << std::endl;
		std::cout << "Origin: [" << origin[0] << ", " << origin[1] << ", " << origin[2] << "]" << std::endl;
		std::cout << "Spacing: [" << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << "]" << std::endl;
		std::cout << "Direction: [" << *direction[0] << ", " << *direction[1] << ", " << *direction[2] << std::endl;
		std::cout << "            " << *direction[3] << ", " << *direction[4] << ", " << *direction[5] << std::endl;
		std::cout << "            " << *direction[6] << ", " << *direction[7] << ", " << *direction[8] << "]" << std::endl;

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
		this->m_transformedImage = harden->GetOutput();*/
		std::cout << "Transform hardened." << std::endl;
		return; 
	}

	/*template< typename TPixelType >
	typename ManageTransformsFilter< TPixelType >::ImageType::Pointer ManageTransformsFilter< typename TPixelType >::ResampleImage(typename ImageType::Pointer image)
	{
		// set up resampling object
		typedef itk::ResampleImageFilter< ImageType, ImageType >	ResampleFilterType;
		ResampleFilterType::Pointer resample = ResampleFilterType::New();

		// define image resampling with respect to fixed image
		resample->SetSize( this->m_FixedImage->GetLargestPossibleRegion().GetSize() );
		resample->SetOutputOrigin( this->m_FixedImage->GetOrigin() );
		resample->SetOutputSpacing( this->m_FixedImage->GetSpacing() );
		resample->SetOutputDirection( this->m_FixedImage->GetDirection() );

				// input parameters
		resample->SetInput( image );
		if (m_CompositeTransform->IsTransformQueueEmpty())
		{
			resample->SetTransform(m_InitialTransform);
		}
		else
		{
			resample->SetTransform(m_CompositeTransform);
		}

		// define interpolator
		ResampleFilterType::InterpolatorType * linInterpolator = resample->GetInterpolator();
		typedef itk::NearestNeighborInterpolateImageFunction< ImageType, double > NearestNeighborType;
		NearestNeighborType::Pointer nnInterpolator = NearestNeighborType::New();

		if( this->m_NearestNeighbor )
		{
			resample->SetInterpolator( nnInterpolator );
			std::cout << "Nearest neighbor interpolator." << std::endl;
		}
		else
		{
			resample->SetInterpolator( linInterpolator );
		}

		// apply
		resample->Update();

		return resample->GetOutput();
	}*/

	/*template< typename TPixelType >
	typename ManageTransformsFilter< TPixelType >::ImageType::Pointer ManageTransformsFilter< TPixelType >::ResampleImage(typename ImageType::Pointer image, TransformType::Pointer transform)
	{
		// set up resampling object
		typedef itk::ResampleImageFilter< ImageType, ImageType >	ResampleFilterType;
		ResampleFilterType::Pointer resample = ResampleFilterType::New();

		if( !this->m_FixedImage )
		{
			std::cout << "Fixed Image not defined. " << std::endl;
			return image;
		}

		// define image resampling with respect to fixed image
		resample->SetSize( this->m_FixedImage->GetLargestPossibleRegion().GetSize() );
		resample->SetOutputOrigin( this->m_FixedImage->GetOrigin() );
		resample->SetOutputSpacing( this->m_FixedImage->GetSpacing() );
		resample->SetOutputDirection( this->m_FixedImage->GetDirection() );

		// input parameters
		resample->SetInput( image );
		resample->SetTransform( transform );

		// define interpolator
		ResampleFilterType::InterpolatorType * linInterpolator = resample->GetInterpolator();
		typedef itk::NearestNeighborInterpolateImageFunction< ImageType, double > NearestNeighborType;
		NearestNeighborType::Pointer nnInterpolator = NearestNeighborType::New();

		if( this->m_NearestNeighbor )
		{
			resample->SetInterpolator( nnInterpolator );
			std::cout << "Nearest neighbor interpolator." << std::endl;
		}
		else
		{
			resample->SetInterpolator( linInterpolator );
		}

		// apply
		resample->Update();

		return resample->GetOutput();
	}*/

	/*template< typename TPixelType >
	typename ManageTransformsFilter< TPixelType >::ImageType::Pointer ManageTransformsFilter< TPixelType >::ResampleImage(typename ImageType::Pointer image, CompositeTransformType::Pointer transform)
	{
		// set up resampling object
		typedef itk::ResampleImageFilter< ImageType, ImageType >	ResampleFilterType;
		ResampleFilterType::Pointer resample = ResampleFilterType::New();

		if (!this->m_FixedImage)
		{
			std::cout << "Fixed Image not defined. " << std::endl;
			return image;
		}

		// define image resampling with respect to fixed image
		resample->SetSize(this->m_FixedImage->GetLargestPossibleRegion().GetSize());
		resample->SetOutputOrigin(this->m_FixedImage->GetOrigin());
		resample->SetOutputSpacing(this->m_FixedImage->GetSpacing());
		resample->SetOutputDirection(this->m_FixedImage->GetDirection());

		// input parameters
		resample->SetInput(image);
		resample->SetTransform(transform);

		// define interpolator
		ResampleFilterType::InterpolatorType * linInterpolator = resample->GetInterpolator();
		typedef itk::NearestNeighborInterpolateImageFunction< ImageType, double > NearestNeighborType;
		NearestNeighborType::Pointer nnInterpolator = NearestNeighborType::New();

		if (this->m_NearestNeighbor)
		{
			resample->SetInterpolator(nnInterpolator);
			std::cout << "Nearest neighbor interpolator." << std::endl;
		}
		else
		{
			resample->SetInterpolator(linInterpolator);
		}

		// apply
		resample->Update();

		return resample->GetOutput();
	}*/

	// create the mask given an ROI filename
	/*template< typename TPixelType >
	typename ManageTransformsFilter< TPixelType >::ImageType::Pointer ManageTransformsFilter< TPixelType >::CropImage(typename ImageType::Pointer image)
	{
		std::vector<float>::iterator it = m_ROI.begin();
	
		// extract center and radius
		double c[3] = {-*(it),-*(it+1),*(it+2)};
		double r[3] = {*(it+3),*(it+4),*(it+5)};
		
		// create size of mask according to the roi array
		// set start index of mask according to the roi array
		ImageType::PointType startPoint, endPoint;
		startPoint[0] = c[0] - r[0];
		startPoint[1] = c[1] - r[1];
		startPoint[2] = c[2] - r[2];

		// find end index
		endPoint[0] = c[0] + r[0];
		endPoint[1] = c[1] + r[1];
		endPoint[2] = c[2] + r[2];

		// convert to indices
		ImageType::IndexType startIndex, endIndex;
		image->TransformPhysicalPointToIndex( startPoint, startIndex );
		image->TransformPhysicalPointToIndex( endPoint, endIndex );

		// plug into region
		ImageType::SizeType regionSize;
		regionSize[0] = abs( startIndex[0] - endIndex[0] );
		regionSize[1] = abs( startIndex[1] - endIndex[1] );
		regionSize[2] = abs( startIndex[2] - endIndex[2] );

		m_CropRegion.SetSize( regionSize );
		m_CropRegion.SetIndex( startIndex );
		m_CropRegion.Crop( image->GetLargestPossibleRegion() );

		// extract cropped image
		typedef itk::ExtractImageFilter< ImageType, ImageType > ExtractFilterType;
		ExtractFilterType::Pointer extract = ExtractFilterType::New();
		extract->SetExtractionRegion( m_CropRegion );
		extract->SetInput( image );
		extract->SetDirectionCollapseToIdentity();

		// update filter
		try
		{
			extract->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}

		return extract->GetOutput();
	}*/

	// extract point values from the slicer ROI file
	template< typename TPixelType >
	void ManageTransformsFilter< TPixelType >::ExtractROIPoints()
	{
		// instantiate ROI array
		bool fullROI = false; // denotes that ROI array is full

		// open file and extract lines
		std::ifstream file( this->m_ROIFilename );
		// check to see if file is open
		if( !file.is_open() )
		{
			std::cerr << this->m_ROIFilename << " not properly opened" << std::endl;
		}
		std::string line;
		int idx = 0; 
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
					m_ROI[idx] = atof( line.substr( positionsOfBars[i]+1, positionsOfBars[i+1]-positionsOfBars[i]-1 ).c_str() );
					idx++;

					// set flag to false if the roi array is filled
					if( idx > 5 )
					{
						
						fullROI = true;
					}
				}
			}
		}

		// array is output as [ centerx, centery, centerz, radiusx, radiusy, radiusz ]
		return;
	}

} // end namespace

#endif