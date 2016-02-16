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
			std::cout << this->m_transform << std::endl;

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

		}
		
		std::cout << "Initialization complete." << std::endl;
		return;
	}

	InitializationFilter::TransformType::Pointer InitializationFilter::GetOutput()
	{
		return this->m_transform;
	}

} // end namespace

#endif