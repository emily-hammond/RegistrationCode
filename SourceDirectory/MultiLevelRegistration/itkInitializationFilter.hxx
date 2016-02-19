#ifndef __itkInitializationFilter_hxx
#define __itkInitializationFilter_hxx

#include "itkInitializationFilter.h"

namespace itk
{
	// contructor to set up initializations and transform
	InitializationFilter::InitializationFilter()
	{
		this->m_centeredOnGeometryFlag = false;
		this->m_metricAlignment0Flag = false;
		this->m_metricAlignment1Flag = false;
		this->m_metricAlignment2Flag = false;
		this->m_transform = TransformType::New();
		this->m_observeFlag = false;
	}

	// read in fixed and moving image
	void InitializationFilter::SetImages( ImageType::Pointer fixedImage, ImageType::Pointer movingImage )
	{
		this->m_fixedImage = fixedImage;
		this->m_movingImage = movingImage;
		
		std::cout << "Images set." << std::endl;
		return;
	}

	// set the flags for metric alignment by axis
	void InitializationFilter::MetricAlignmentOn( int axis )
	{
		// x-axis
		if( axis == 0 )
		{
			this->m_metricAlignment0Flag = true;
		}
		// y-axis
		else if( axis == 1 )
		{
			this->m_metricAlignment1Flag = true;
		}
		// z-axis
		else if( axis == 2 )
		{
			this->m_metricAlignment2Flag = true;
		}
		// error handling
		else
		{
			std::cout << "Axis number invalid. 0 = x, 1 = y, 2 = z" << std::endl;
		}
		return;
	}

	// perform the initialization by going through the set flags
	void InitializationFilter::Update()
	{
		// center images based on geometry
		if( this->m_centeredOnGeometryFlag )
		{
			CenterOnGeometry();
		}
		// x-axis
		if( this->m_metricAlignment0Flag )
		{
			MetricAlignment( 0 );
		}
		// y-axis
		if( this->m_metricAlignment1Flag )
		{
			MetricAlignment( 1 );
		}
		// z-axis
		if( this->m_metricAlignment2Flag )
		{
			MetricAlignment( 2 );
		}

		return;
	}

	// read out the final transform 
	InitializationFilter::TransformType::Pointer InitializationFilter::GetOutput()
	{
		return this->m_transform;
	}

	// obtain the proper range to translate the moving image for metric alignment depending on the given axis
	void InitializationFilter::GetRange( int axis )
	{
		// moving image parameters
		const ImageType::SizeType & movingSize = this->m_movingImage->GetLargestPossibleRegion().GetSize();
		const ImageType::SpacingType & movingSpacing = this->m_movingImage->GetSpacing();

		// fixed image parameters
		const ImageType::SizeType & fixedSize = this->m_fixedImage->GetLargestPossibleRegion().GetSize();
		const ImageType::SpacingType & fixedSpacing = this->m_fixedImage->GetSpacing();

		// determine range
		this->m_translationRange = fixedSize[ axis ]*fixedSpacing[ axis ] - movingSize[ axis ]*movingSpacing[ axis ];
		
		// print out to screen
		//std::cout << "Range: " << this->m_translationRange << std::endl;
		return;
	}

	// center the images based on geometry
	void InitializationFilter::CenterOnGeometry()
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

	// metric alignment based on the desired axis
	void InitializationFilter::MetricAlignment( int axis )
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

		// obtain current transform parameters
		TransformType::ParametersType parameters = this->m_transform->GetParameters();

		// initializations
		this->m_minMetric = 100000.0;
		this->m_minParameters = parameters;

		// get desired translation range and calculate start and end parameters
		this->GetRange( axis );
		float start = parameters[ axis + 3 ] - this->m_translationRange/2.0;
		float end = parameters[ axis + 3 ] + this->m_translationRange/2.0;

		// header for section
		if( this->m_observeFlag )
		{
			std::cout << "\n\nMetric Initialization on " << axis << " axis:\n";
		}

		// parse through translation range and determine smallest metric value
		for( float i = start; i < end; i = i - (start-end)/20.0)
		{
			// change parameters
			parameters[ axis + 3 ] = i;
			// store parameters and corresponding metric into array
			if( mmi->GetValue( parameters ) < this->m_minMetric )
			{
				this->m_minMetric = mmi->GetValue( parameters );
				this->m_minParameters = parameters;
			}

			// print out results if observing on
			if( this->m_observeFlag )
			{
				std::cout<< mmi->GetValue( parameters ) << ":";
				for( int j = 0; j < 9; ++j )
				{
					std::cout << " " << parameters[j];
				}
				std::cout << std::endl;
			}
		}

		// save results into transform
		this->m_transform->SetParameters( this->m_minParameters );
		//std::cout << this->m_transform << std::endl;
	
		std::cout << "Metric initialization on " << axis << " complete." << std::endl;
		return;
	}

} // end namespace

#endif