#ifndef __itkRegistrationFramework_hxx
#define __itkRegistrationFramework_hxx

#include "itkRegistrationFramework.h"

namespace itk
{
	// set up defaults in constructor
	RegistrationFramework::RegistrationFramework()
	{
		// initialize components
		this->m_initialTransform = TransformType::New();
		this->m_transform = TransformType::New();
		this->m_interpolator = InterpolatorType::New();
		this->m_metric = MetricType::New();
		this->m_optimizer = OptimizerType::New();
		this->m_registration = RegistrationType::New();
		this->m_observer = RigidCommandIterationUpdate::New();

		// set defaults
		this->SetDefaults();
	}

	// run through registration process
	void RegistrationFramework::PerformRegistration()
	{
		//set up components
		this->SetUpMetric();
		this->SetUpOptimizer();

		//std::cout << this->m_metric << std::endl;
		//std::cout << this->m_optimizer << std::endl;

		// input components to registration object
		this->m_registration->SetMetric( this->m_metric );
		this->m_registration->SetOptimizer( this->m_optimizer );
		this->m_registration->SetTransform( this->m_transform );
		this->m_registration->SetInterpolator( this->m_interpolator );

		// input images and transform to registration class
		this->m_registration->SetFixedImage( this->m_fixedImage );
		this->m_registration->SetMovingImage( this->m_movingImage );
		this->m_registration->SetInitialTransformParameters( this->m_initialTransform->GetParameters() );
		this->m_registration->SetFixedImageRegion( this->m_fixedImage->GetBufferedRegion() );

		// begin registration
		std::cout << "Begin registration." << std::endl;
		try
		{
			this->m_registration->Update();
			std::cout << "OptimizerStopCondition: " << this->m_registration->GetOptimizer()->GetStopConditionDescription() << std::endl;
		}
		catch( itk::ExceptionObject & err )
		{
			std::cerr << "ExceptionObjectCaught!" << std::endl;
			std::cerr << err << std::endl;
			return;
		}

		std::cout << "Registration performed." << std::endl;
		return;
	}

	// set fixed and moving images
	void RegistrationFramework::SetImages( ImageType::Pointer fixedImage, ImageType::Pointer movingImage )
	{
		this->m_fixedImage = fixedImage;
		this->m_movingImage = movingImage;
		
		std::cout << "Images set." << std::endl;
		return;
	}

	// set initial transform if desired
	void RegistrationFramework::SetInitialTransform( TransformType::Pointer initialTransform )
	{
		this->m_initialTransform = initialTransform ;
		
		std::cout << "Initial rigid transform set." << std::endl;
		return;
	}

	// define defaults
	void RegistrationFramework::SetDefaults()
	{
		// metric
		this->m_percentageOfSamples = 0.01;
		this->m_histogramBins = 50;

		// optimizer
		this->m_minimumStepLength = 0.001;
		this->m_maximumStepLength = 1.0;
		this->m_numberOfIterations = 500;
		this->m_relaxationFactor = 0.5;
		this->m_gradientMagnitudeTolerance = 0.001;
		this->m_rotationScale = 0.01;
		this->m_translationScale = 10;
		this->m_scalingScale = 0;

		// observer
		this->m_observeFlag = false;
		
		std::cout << "Defaults set." << std::endl;
		return;
	}

	// set up MMI metric for defaults
	void RegistrationFramework::SetUpMetric()
	{
		// determine number of samples to use
		ImageType::SizeType size = this->m_fixedImage->GetLargestPossibleRegion().GetSize();
		int numOfPixels = size[0]*size[1]*size[2];
		this->m_metric->SetNumberOfSpatialSamples( numOfPixels*(this->m_percentageOfSamples) );
		// define number of histogram bins
		this->m_metric->SetNumberOfHistogramBins( this->m_histogramBins );

		std::cout << "Metric set." << std::endl;
		return;
	}

	// set up RSGD optimizer
	void RegistrationFramework::SetUpOptimizer()
	{
		// set defaults
		this->m_optimizer->SetMinimumStepLength( this->m_minimumStepLength );
		this->m_optimizer->SetMaximumStepLength( this->m_maximumStepLength );
		this->m_optimizer->SetNumberOfIterations( this->m_numberOfIterations );
		this->m_optimizer->SetRelaxationFactor( this->m_relaxationFactor );
		this->m_optimizer->SetGradientMagnitudeTolerance( this->m_gradientMagnitudeTolerance );

		// insert optimizer scales
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

		// insert into observer if desired
		if( this->m_observeFlag )
		{
			this->m_optimizer->AddObserver( itk::IterationEvent(), this->m_observer );
			std::cout << "Set to observe registration process." << std::endl;
		}

		std::cout << "Optimizer set." << std::endl;

		return;
	}

	// get final transform
	RegistrationFramework::TransformType::Pointer RegistrationFramework::GetOutput()
	{
		// print out final optimizer parameters
		std::cout << "\n==== Final Parameters ====" << std::endl;
		std::cout << "Iterations: " << this->m_optimizer->GetCurrentIteration() << std::endl;
		std::cout << "Metric: " << this->m_optimizer->GetValue() << std::endl;
		std::cout << "Stop Condition: " << this->m_registration->GetOptimizer()->GetStopConditionDescription() << std::endl;

		// get final transform
		TransformType::Pointer finalTransform = TransformType::New();
		finalTransform->SetParameters( this->m_registration->GetLastTransformParameters() );
		finalTransform->SetFixedParameters( this->m_transform->GetFixedParameters() );

		std::cout << "Angle: " << finalTransform->GetVersor().GetAngle() << std::endl;
		std::cout << "Translation: " << finalTransform->GetTranslation() << std::endl;
		std::cout << "Scaling: " << finalTransform->GetScale() << std::endl;

		return finalTransform;
	}

} // end namespace

#endif