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
		 this->m_transform = RigidTransformType::New();
		 this->m_initialTransform = RigidTransformType::New();

		 this->m_metric = MetricType::New();
		 this->m_optimizer = OptimizerType::New();
		 this->m_registration = RegistrationType::New();
	}

	void RegistrationFramework::PerformRegistration()
	{
		// set up components
		this->SetDefaults();
		this->SetUpMetric();
		this->SetUpOptimizer();
		

		// plug into registration method
		this->m_registration->SetMetric( this->m_metric );
		this->m_registration->SetOptimizer( this->m_optimizer );

		// set up images
		this->m_registration->SetFixedImage( this->m_fixedImage );
		this->m_registration->SetMovingImage( this->m_movingImage );
		this->m_registration->SetInitialTransform( this->m_initialTransform );

		std::cout << this->m_initialTransform << std::endl;

		// step up one level of registration
		RegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
		shrinkFactorsPerLevel.SetSize( 1 );
		shrinkFactorsPerLevel[0] = 1;

		RegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
		smoothingSigmasPerLevel.SetSize( 1 );
		smoothingSigmasPerLevel[0] = 0;

		this->m_registration->SetNumberOfLevels( 1 );
		this->m_registration->SetSmoothingSigmasPerLevel( smoothingSigmasPerLevel );
		this->m_registration->SetShrinkFactorsPerLevel( shrinkFactorsPerLevel );
		
		std::cout << "Components set up. Beginning registration." << std::endl;
		// update registration process
		try
		{
			this->m_registration->Update();
			std::cout << "Optimizer stop condition: " << this->m_registration->GetOptimizer()->GetStopConditionDescription() << std::endl;
		}
		catch( itk::ExceptionObject & err )
		{
			std::cout << "ExceptionObject caught!" << std::endl;
			std::cout << err << std::endl;
			return;
		}

		//std::cout << "Registration performed." << std::endl;
		return;
	}

	// member function implementations
	void RegistrationFramework::SetImages( ImageType::Pointer fixedImage, ImageType::Pointer movingImage )
	{
		this->m_fixedImage = fixedImage;
		this->m_movingImage = movingImage;
		
		//std::cout << "Images set." << std::endl;
		return;
	}

	void RegistrationFramework::SetInitialTransform( RigidTransformType::Pointer initialTransform )
	{
		this->m_initialTransform = initialTransform;
		
		//std::cout << "Initial rigid transform set." << std::endl;
		return;
	}

	void RegistrationFramework::SetDefaults()
	{
		// metric
		this->m_percentageOfSamples = 0.01;
		this->m_histogramBins = 35;

		// optimizer
		this->m_minimumStepLength = 0.001;
		this->m_numberOfIterations = 200;
		this->m_relaxationFactor = 0.5;
		this->m_learningRate = 0.2;
		this->m_gradientMagnitudeTolerance = 0.01;
		this->m_rotationScale = 0.01;
		this->m_translationScale = 10;
		this->m_scalingScale = 0.001;
		
		//std::cout << "Defaults set." << std::endl;
		return;
	}

	void RegistrationFramework::SetUpMetric()
	{
		// determine number of samples to use
		this->m_registration->SetMetricSamplingPercentage( this->m_percentageOfSamples );
		RegistrationType::MetricSamplingStrategyType samplingStrategy = RegistrationType::RANDOM;
		this->m_registration->SetMetricSamplingStrategy( samplingStrategy );
		this->m_metric->SetNumberOfHistogramBins( this->m_histogramBins );

		//std::cout << "Metric set." << std::endl;
		return;
	}

	void RegistrationFramework::SetUpOptimizer()
	{
		// set defaults
		this->m_optimizer->SetMinimumStepLength( this->m_minimumStepLength );
		this->m_optimizer->SetNumberOfIterations( this->m_numberOfIterations );
		this->m_optimizer->SetRelaxationFactor( this->m_relaxationFactor );
		this->m_optimizer->SetLearningRate( this->m_learningRate );
		this->m_optimizer->SetGradientMagnitudeTolerance( this->m_gradientMagnitudeTolerance );
		this->m_optimizer->SetReturnBestParametersAndValue( true );

		// automatically estimate optimizer scales
		OptimizerType::ScalesType optimizerScales( this->m_initialTransform->GetNumberOfParameters() );
	
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
		//std::cout << optimizerScales << std::endl;

		// set the scales
		this->m_optimizer->SetScales( optimizerScales );

		// register with command class to monitor process
		RigidCommandIterationUpdate::Pointer rigidObserver = RigidCommandIterationUpdate::New();
		this->m_optimizer->AddObserver( itk::IterationEvent(), rigidObserver );

		//std::cout << "Optimizer set." << std::endl;

		return;
	}


} // end namespace

#endif