#ifndef __itkManageTransformsFilter_hxx
#define __itkManageTransformsFilter_hxx_hxx

#include "itkManageTransformsFilter.h"
#include "itkImageRegionIterator.h"

namespace itk
{
	// constructor
	ManageTransformsFilter::ManageTransformsFilter():
		m_FixedImage( ITK_NULLPTR ),	// defined by user
		m_MovingImage( ITK_NULLPTR ),	// defined by user
		m_MovingLabelMap( ITK_NULLPTR ),	// defined by user
		m_InitialTransform( ITK_NULLPTR ),	// defined by user
		m_HardenTransform( false ),
		m_ResampleImage( false ),
		m_NearestNeighbor( false ),
		m_ResampleImageWithInitialTransform( false )
	{
		m_CompositeTransform = CompositeTransformType::New();
	}

	void ManageTransformsFilter::AddTransform( TransformType::Pointer transform )
	{
		this->m_CompositeTransform->AddTransform( transform );
	}

	void ManageTransformsFilter::Update()
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
			std::cout << "Resampling image. " << std::endl;
			this->m_TransformedImage = ResampleImage( this->m_MovingImage );
			std::cout << "Moving image resampled." << std::endl;

			// repeat for label map with nearest neighbor interpolation
			if( m_MovingLabelMap )
			{
				std::cout << "Resampling label map. " << std::endl;
				NearestNeighborInterpolateOn();
				this->m_TransformedLabelMap = ResampleImage( this->m_MovingLabelMap );
				NearestNeighborInterpolateOff();
				std::cout << "Moving label map resampled." << std::endl;
			}
		}
		if( this->m_HardenTransform )
		{
			HardenTransform();
		}
		if( this->m_ResampleImageWithInitialTransform )
		{
			if( !m_InitialTransform )
			{
				itkExceptionMacro( << "InitialTransform not present" );
			}

			std::cout << "Resampling image with respect to initial transform. " << std::endl;
			this->m_TransformedImage = ResampleImageWithInitialTransform( this->m_MovingImage );
			std::cout << "Moving image resampled." << std::endl;

			// repeat for label map with nearest neighbor interpolation
			if( m_MovingLabelMap )
			{
				std::cout << "Resampling label map with respect to initial transform. " << std::endl;
				NearestNeighborInterpolateOn();
				this->m_TransformedLabelMap = ResampleImageWithInitialTransform( this->m_MovingLabelMap );
				NearestNeighborInterpolateOff();
				std::cout << "Moving label map resampled." << std::endl;
			}
		}

		return;
	}

	// apply current transform on file to the header information of the input image
	void ManageTransformsFilter::HardenTransform()
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

	ManageTransformsFilter::ImageType::Pointer ManageTransformsFilter::ResampleImage( ImageType::Pointer image )
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
		resample->SetTransform( this->m_CompositeTransform );

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
	}

	// resample moving image with an externally defined transform
	ManageTransformsFilter::ImageType::Pointer ManageTransformsFilter::ResampleImageWithInitialTransform( ImageType::Pointer image )
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
		resample->SetTransform( this->m_InitialTransform );

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
	}
	
} // end namespace

#endif