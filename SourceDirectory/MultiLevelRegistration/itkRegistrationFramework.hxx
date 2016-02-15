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
		this->m_initialTransform = RigidTransformType::New();
		this->m_transform = RigidTransformType::New();
		this->m_interpolator = InterpolatorType::New();
		this->m_metric = MetricType::New();
		this->m_optimizer = OptimizerType::New();
		this->m_observer = RigidCommandIterationUpdate::New();
	}

	void RegistrationFramework::PerformRegistration()
	{
		this->SetDefaults();
		this->SetUpMetric();
		this->SetUpOptimizer();

		//std::cout << this->m_metric << std::endl;
		//std::cout << this->m_optimizer << std::endl;



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
		this->m_initialTransform = initialTransform ;
		
		std::cout << "Initial rigid transform set." << std::endl;
		return;
	}

	void RegistrationFramework::SetDefaults()
	{
		// metric
		this->m_percentageOfSamples = 0.01;
		this->m_histogramBins = 35;

		// optimizer
		this->m_minimumStepLength = 0.001;
		this->m_maximumStepLength = 1.5;
		this->m_numberOfIterations = 1000;
		this->m_relaxationFactor = 0.5;
		this->m_gradientMagnitudeTolerance = 0.01;
		this->m_rotationScale = 0.01;
		this->m_translationScale = 10;
		this->m_scalingScale = 0.001;
		
		std::cout << "Defaults set." << std::endl;
		return;
	}

	void RegistrationFramework::SetUpMetric()
	{
		// determine number of samples to use
		ImageType::SizeType size = this->m_fixedImage->GetLargestPossibleRegion().GetSize();
		int numOfPixels = size[0]*size[1]*size[2];
		this->m_metric->SetNumberOfSpatialSamples( numOfPixels*(this->m_percentageOfSamples) );
		this->m_metric->SetNumberOfHistogramBins( this->m_histogramBins );

		std::cout << "Metric set." << std::endl;
		return;
	}

	void RegistrationFramework::SetUpOptimizer()
	{
		// set defaults
		this->m_optimizer->SetMinimumStepLength( this->m_minimumStepLength );
		this->m_optimizer->SetMaximumStepLength( this->m_maximumStepLength );
		this->m_optimizer->SetNumberOfIterations( this->m_numberOfIterations );
		this->m_optimizer->SetRelaxationFactor( this->m_relaxationFactor );
		this->m_optimizer->SetGradientMagnitudeTolerance( this->m_gradientMagnitudeTolerance );

		// automatically estimate optimizer scales
		OptimizerType::ScalesType optimizerScales( this->m_transform->GetNumberOfParameters() );
		// rotation
		optimizerScales[0] = 1.0/this->m_rotationScale;
		optimizerScales[1] = 1.0/this->m_rotationScale;
		optimizerScales[2] = 1.0/this->m_rotationScale;
		// translation
		optimizerScales[3] = 1.0/this->m_translationScale;
		optimizerScales[4] = 1.0/this->m_translationScale;
		optimizerScales[5] = 1.0/this->m_translationScale;
		// scaling
		optimizerScales[6] = 1.0/this->m_scalingScale;
		optimizerScales[7] = 1.0/this->m_scalingScale;
		optimizerScales[8] = 1.0/this->m_scalingScale;

		// set the scales
		this->m_optimizer->SetScales( optimizerScales );

		// insert into observer
		this->m_optimizer->AddObserver( itk::IterationEvent(), this->m_observer );

		std::cout << "Optimizer set." << std::endl;

		return;
	}


} // end namespace

#endif