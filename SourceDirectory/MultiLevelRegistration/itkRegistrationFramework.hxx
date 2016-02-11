/*
insert comments here
*/

#ifndef __itkRegistrationFramework_hxx
#define __itkRegistrationFramework_hxx

#include "itkRegistrationFramework.h"

namespace itk
{
	RegistrationFramework::RegistrationFramework()
	{
		 this->m_transforms = CompositeTransformType::New();
		 this->m_interpolator = InterpolatorType::New();
		 this->m_metric = MetricType::New();
	}

	void RegistrationFramework::PerformRegistration()
	{
		this->SetDefaults();
		this->SetUpMetric();

		std::cout << "Registration performed." << std::endl;
		return;
	}

	// member function implementations
	void RegistrationFramework::SetImages( ImageType::Pointer fixedImage, ImageType::Pointer movingImage )
	{
		this->m_fixedImage = fixedImage;
		this->m_movingImage = movingImage;
		
		std::cout << "Images set." << std::endl;
		return;
	}

	void RegistrationFramework::SetInitialTransform( RigidTransformType::Pointer initialTransform )
	{
		this->m_transforms->AddTransform( initialTransform );
		
		std::cout << "Initial rigid transform set." << std::endl;
		return;
	}

	void RegistrationFramework::SetInitialTransform( CompositeTransformType::Pointer initialTransform )
	{
		this->m_transforms->AddTransform( initialTransform );
		
		std::cout << "Initial composite transform set." << std::endl;
		return;
	}

	void RegistrationFramework::SetUpMetric()
	{
		// determine number of samples to use
		ImageType::SizeType size = this->m_fixedImage->GetLargestPossibleRegion().GetSize();
		int numOfPixels = size[0]*size[1]*size[2];
		this->m_metric->SetNumberOfSpatialSamples( numOfPixels*(this->m_PercentageOfSamples) );
		this->m_metric->SetNumberOfHistogramBins( this->m_HistogramBins );

		std::cout << "Metric set." << std::endl;
		return;
	}

	void RegistrationFramework::SetDefaults()
	{
		this->m_PercentageOfSamples = 0.01;
		this->m_HistogramBins = 25;
		
		std::cout << "Defaults set." << std::endl;
		return;
	}

} // end namespace

#endif