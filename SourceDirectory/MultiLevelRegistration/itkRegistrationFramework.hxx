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

		// initialize metric
		this->m_metric = MetricType::New();
		this->m_metric->SetNumberOfHistogramBins( 20 );
				
		// set up optimizer
		this->m_optimizer = OptimizerType::New();
		// set defaults
		this->m_optimizer->SetMinimumStepLength( 0.001 );
		this->m_optimizer->SetNumberOfIterations( 100 );
		this->m_optimizer->SetRelaxationFactor( 0.8 );
		this->m_optimizer->SetLearningRate( 0.5 );
		this->m_optimizer->SetGradientMagnitudeTolerance( 0.01 );
		this->m_optimizer->SetReturnBestParametersAndValue( true );
		// automatically estimate optimizer scales
		OptimizerType::ScalesType optimizerScales( this->m_initialTransform->GetNumberOfParameters() );
		// rotation
		optimizerScales[0] = 1.0/0.1;
		optimizerScales[1] = 1.0/0.1;
		optimizerScales[2] = 1.0/0.1;
		// translation
		optimizerScales[3] = 1.0/10.0;
		optimizerScales[4] = 1.0/10.0;
		optimizerScales[5] = 1.0/10.0;
		// scaling
		optimizerScales[6] = 1.0/0.01;
		optimizerScales[7] = 1.0/0.01;
		optimizerScales[8] = 1.0/0.01;
		// set the scales
		this->m_optimizer->SetScales( optimizerScales );

		// register with command class to monitor process
		RigidCommandIterationUpdate::Pointer rigidObserver = RigidCommandIterationUpdate::New();
		this->m_optimizer->AddObserver( itk::IterationEvent(), rigidObserver );

		this->m_registration = RegistrationType::New();
		// plug into registration method
		this->m_registration->SetMetric( this->m_metric );
		// start with 1% of all samples sampled randomly
		this->m_registration->SetMetricSamplingPercentage( 0.01 ); 
		RegistrationType::MetricSamplingStrategyType samplingStrategy = RegistrationType::RANDOM;
		this->m_registration->SetMetricSamplingStrategy( samplingStrategy );
		this->m_registration->SetOptimizer( this->m_optimizer );
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
	}

	void RegistrationFramework::PerformRegistration()
	{
		// set up images
		this->m_registration->SetFixedImage( this->m_fixedImage );
		this->m_registration->SetMovingImage( this->m_movingImage );
		this->m_registration->SetInitialTransform( this->m_initialTransform );

		std::cout << this->m_optimizer << std::endl;
		
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

} // end namespace

#endif