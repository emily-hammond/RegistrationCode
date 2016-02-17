/*
insert comments here
*/

#ifndef __itkInitializationFilter_hxx
#define __itkInitializationFilter_hxx

#include "itkInitializationFilter.h"

namespace itk
{
	// contructor
	InitializationFilter::InitializationFilter()
	{
		this->m_centeredOnGeometryFlag = false;
		this->m_metricAlignmentFlag = false;
		this->m_metricAxis = 0;
		this->m_transform = TransformType::New();
	}

	// member function implementations
	void InitializationFilter::SetImages( ImageType::Pointer fixedImage, ImageType::Pointer movingImage )
	{
		this->m_fixedImage = fixedImage;
		this->m_movingImage = movingImage;
		
		std::cout << "Images set." << std::endl;
		return;
	}

	void InitializationFilter::PerformInitialization()
	{
		if( this->m_centeredOnGeometryFlag )
		{
			// instantiate initialization filter
			typedef itk::CenteredTransformInitializer< TransformType, ImageType, ImageType >	CenteredInitializeType;
			CenteredInitializeType::Pointer initializer = CenteredInitializeType::New();

			// set parameters
			initializer->GeometryOn();
			initializer->SetTransform( this->m_transform );
			initializer->SetFixedImage( this->m_fixedImage );
			initializer->SetMovingImage( this->m_movingImage );
			
			// perform initialization
			initializer->InitializeTransform();
			//std::cout << this->m_transform << std::endl;

			std::cout << "Centered on geometry initialization complete." << std::endl;
		}

		if( this->m_metricAlignmentFlag > 0 )
		{
			// instantiate metric to use
			typedef itk::MattesMutualInformationImageToImageMetric< ImageType, ImageType > MetricType;
			MetricType::Pointer mmi = MetricType::New();

			// connect interpolator
			typedef itk::LinearInterpolateImageFunction< ImageType, double >	InterpolatorType;
			InterpolatorType::Pointer interpolator = InterpolatorType::New();

			// set parameters
			mmi->SetFixedImage( this->m_fixedImage );
			mmi->SetMovingImage( this->m_movingImage );
			mmi->SetFixedImageRegion( this->m_fixedImage->GetLargestPossibleRegion() );
			mmi->SetTransform( this->m_transform );
			mmi->SetInterpolator( interpolator );

			// initialize metric
			mmi->Initialize();

			// obtain transform parameters
			TransformType::ParametersType parameters = this->m_transform->GetParameters();

			// initialize parameters
			this->m_minMetric = 100000.0;
			this->m_minParameters = parameters;
			this->GetRange();
			float start = parameters[ this->m_metricAxis + 3 ] - this->m_translationRange/2.0;
			float end = parameters[ this->m_metricAxis + 3 ] + this->m_translationRange/2.0;

			std::cout << "Start: " << start << "    End: " << end << std::endl;

			for( float i = start; i < end; i = i - (start-end)/20.0)
			{
				// change parameters
				parameters[ this->m_metricAxis + 3 ] = i;
				// store parameters and corresponding metric into array
				if( mmi->GetValue( parameters ) < this->m_minMetric )
				{
					this->m_minMetric = mmi->GetValue( parameters );
					this->m_minParameters = parameters;
				}

				// print out results
				std::cout<< mmi->GetValue( parameters ) << ":";
				for( int j = 0; j < 9; ++j )
				{
					std::cout << " " << parameters[j];
				}
				std::cout << std::endl;
			}

			// save results into transform
			this->m_transform->SetParameters( this->m_minParameters );
			std::cout << this->m_transform << std::endl;
		}
		
		std::cout << "Initialization complete." << std::endl;
		return;
	}

	InitializationFilter::TransformType::Pointer InitializationFilter::GetOutput()
	{
		return this->m_transform;
	}

	void InitializationFilter::GetRange()
	{
		// moving image parameters
		const ImageType::SizeType & movingSize = this->m_movingImage->GetLargestPossibleRegion().GetSize();
		const ImageType::SpacingType & movingSpacing = this->m_movingImage->GetSpacing();

		// fixed image parameters
		const ImageType::SizeType & fixedSize = this->m_fixedImage->GetLargestPossibleRegion().GetSize();
		const ImageType::SpacingType & fixedSpacing = this->m_fixedImage->GetSpacing();

		// determine range
		this->m_translationRange = fixedSize[ this->m_metricAxis ]*fixedSpacing[ this->m_metricAxis ] - 
			movingSize[ this->m_metricAxis ]*movingSpacing[ this->m_metricAxis ];
		
		// print out to screen
		std::cout << "Range: " << this->m_translationRange << std::endl;
		return;
	}

} // end namespace

#endif