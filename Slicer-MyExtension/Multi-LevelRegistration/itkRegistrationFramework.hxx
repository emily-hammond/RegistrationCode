#ifndef __itkRegistrationFramework_hxx
#define __itkRegistrationFramework_hxx

#include "itkRegistrationFramework.h"

namespace itk
{
	// set up defaults in constructor
	template< typename TPixelType >
	RegistrationFramework< TPixelType >::RegistrationFramework() :
		// images
		m_FixedImage(ITK_NULLPTR),	// provided by user
		m_MovingImage(ITK_NULLPTR),	// provided by user

		// transforms
		m_InitialTransform(ITK_NULLPTR),	// provided by user

		// metric
		m_PercentageOfSamples(0.01),
		m_HistogramBins(50),

		// optimizer
		m_MinimumStepLength(0.001),
		m_MaximumStepLength(1),
		m_NumberOfIterations(500),
		m_RelaxationFactor(0.5),
		m_GradientMagnitudeTolerance(0.001),
		m_RotationScale(0.001),
		m_TranslationScale(10),
		m_ScalingScale(0.001),
		m_ObserveOn(false),
		m_ObserverSet(true)
	{
		// observer
		m_Transform = TransformType::New();
		m_FinalTransform = TransformType::New();

		// registration components
		m_Interpolator = InterpolatorType::New();
		m_Metric = MetricType::New();
		m_Optimizer = OptimizerType::New();
		m_Registration = RegistrationType::New();
		m_Observer = RigidCommandIterationUpdate::New();
	}

	// run through registration process
	template< typename TPixelType >
	void RegistrationFramework< TPixelType >::Update()
	{
		//set up components
		this->Initialize();

		// input components to registration object
		this->m_Registration->SetMetric( this->m_Metric );
		this->m_Registration->SetOptimizer( this->m_Optimizer );
		this->m_Registration->SetTransform( this->m_Transform );
		this->m_Registration->SetInterpolator( this->m_Interpolator );

		// input images and transform to registration class
		this->m_Registration->SetFixedImage( this->m_FixedImage );
		this->m_Registration->SetMovingImage( this->m_MovingImage );
		this->m_Registration->SetFixedImageRegion( this->m_FixedImage->GetBufferedRegion() );
		
		// initial transform
		TransformType::ParametersType identityParameters( this->m_Transform->GetNumberOfParameters() );
		if( !m_InitialTransform )
		{
			// create identity transform parameters
			identityParameters[0] = 0;	// rotation
			identityParameters[1] = 0;
			identityParameters[2] = 0;
			identityParameters[3] = 0;	// translation
			identityParameters[4] = 0;
			identityParameters[5] = 0;
			identityParameters[6] = 1;	// scaling
			identityParameters[7] = 1;
			identityParameters[8] = 1;
			
			// insert into initial transform parameters
			this->m_Registration->SetInitialTransformParameters( identityParameters );
		}
		else
		{
			this->m_Registration->SetInitialTransformParameters( this->m_InitialTransform->GetParameters() );
		}

		std::cout << m_Registration->GetInitialTransformParameters() << std::endl;

		// begin registration
		std::cout << "Begin registration." << std::endl;
		try
		{
			this->m_Registration->Update();
			std::cout << m_Optimizer->GetCurrentIteration() << " " << m_Optimizer->GetCurrentStepLength();// << " " << optimizer->GetGradientMagnitude();
			std::cout << " " << m_Optimizer->GetValue() << " " << m_Optimizer->GetCurrentPosition();
			std::cout << std::endl;
		}
		catch( itk::ExceptionObject & err )
		{
			std::cerr << "ExceptionObjectCaught!" << std::endl;
			std::cerr << err << std::endl;
			return;
		}

		// get final transform
		this->m_FinalTransform->SetParameters( this->m_Registration->GetLastTransformParameters() );
		this->m_FinalTransform->SetFixedParameters( this->m_Transform->GetFixedParameters() );
		
		std::cout << "Registration performed." << std::endl;
		return;
	}

	// set up MMI metric for defaults
	template< typename TPixelType >
	void RegistrationFramework< TPixelType >::Initialize()
	{
		// ****DETERMINE IMAGES/TRANSFORMS****
		if( !m_FixedImage )
		{
			itkExceptionMacro( << "FixedImage not present" );
		}
		if( !m_MovingImage )
		{
			itkExceptionMacro( << "MovingImage not present" );
		}

		// ****SET UP METRIC****
		// determine number of samples to use
		ImageType::SizeType size = this->m_FixedImage->GetLargestPossibleRegion().GetSize();
		int NumOfPixels = size[0]*size[1]*size[2];
		this->m_Metric->SetNumberOfSpatialSamples( NumOfPixels*(this->m_PercentageOfSamples) );
		// define number of histogram bins
		this->m_Metric->SetNumberOfHistogramBins( this->m_HistogramBins );


		// ****SET UP OPTIMIZER****
		// set defaults
		this->m_Optimizer->SetMinimumStepLength( this->m_MinimumStepLength );
		this->m_Optimizer->SetMaximumStepLength( this->m_MaximumStepLength );
		this->m_Optimizer->SetNumberOfIterations( this->m_NumberOfIterations );
		this->m_Optimizer->SetRelaxationFactor( this->m_RelaxationFactor );
		this->m_Optimizer->SetGradientMagnitudeTolerance( this->m_GradientMagnitudeTolerance );

		// insert optimizer scales
		OptimizerType::ScalesType optimizerScales( this->m_Transform->GetNumberOfParameters() );
		// rotation
		optimizerScales[0] = 1.0/this->m_RotationScale;
		optimizerScales[1] = 1.0/this->m_RotationScale;
		optimizerScales[2] = 1.0/this->m_RotationScale;
		// translation
		optimizerScales[3] = 1.0/this->m_TranslationScale;
		optimizerScales[4] = 1.0/this->m_TranslationScale;
		optimizerScales[5] = 1.0/this->m_TranslationScale;
		// scaling
		optimizerScales[6] = 1.0/this->m_ScalingScale;
		optimizerScales[7] = 1.0/this->m_ScalingScale;
		optimizerScales[8] = 1.0/this->m_ScalingScale;

		// set the scales
		this->m_Optimizer->SetScales( optimizerScales );

		// insert into observer if desired
		if( this->m_ObserveOn )
		{
			this->m_Observer->Observe();
			std::cout << "Set to observe registration process." << std::endl;
		}
		if (this->m_DebugOn)
		{
			this->m_Observer->Debug(this->m_DebugDirectory);
			std::cout << "Writing out every 50 iterations" << std::endl;
		}
		if ((this->m_ObserveOn || this->m_DebugOn) && this->m_ObserverSet)
		{
			this->m_Optimizer->AddObserver(itk::IterationEvent(), this->m_Observer);
			this->m_ObserverSet = false;
		}

		return;
	}

	// print out results
	template< typename TPixelType >
	void RegistrationFramework< TPixelType >::Print()
	{
		// Set up values
		std::cout << "\nMetric values " << std::endl;
		std::cout << "  % of samples      : " << m_PercentageOfSamples << std::endl;
		std::cout << "  #of histogram bins: " << m_HistogramBins << std::endl;

		// optimizer
		std::cout << "Optimizer values" << std::endl;
		std::cout << "  Min step length   : " << m_MinimumStepLength << std::endl;
		std::cout << "  Max step length   : " << m_MaximumStepLength << std::endl;
		std::cout << "  Max iterations    : " << m_NumberOfIterations << std::endl;
		std::cout << "  Relaxation factor : " << m_RelaxationFactor << std::endl;
		std::cout << "  Grad mag tolerance: " << m_GradientMagnitudeTolerance << std::endl;

		// scaling
		std::cout << "Expected scaling values" << std::endl;
		std::cout << "  Rotation scale    : " << m_RotationScale << std::endl;
		std::cout << "  Translation scale : " << m_TranslationScale << std::endl;
		std::cout << "  Scaling scale     : " << m_ScalingScale << std::endl;
		
		// print out final optimizer parameters
		std::cout << "\nFinal Parameters" << std::endl;
		std::cout << "  Iterations    : " << this->m_Optimizer->GetCurrentIteration() << std::endl;
		std::cout << "  Metric        : " << this->m_Optimizer->GetValue() << std::endl;
		std::cout << "  Stop Condition: " << this->m_Registration->GetOptimizer()->GetStopConditionDescription() << std::endl;

		// final transform
		std::cout << "Transform " << std::endl;
		std::cout << "  Angle         : " << (this->m_FinalTransform->GetVersor().GetAngle())*180.0/3.141592653589793238463 << std::endl;
		std::cout << "  Axis          : " << this->m_FinalTransform->GetVersor().GetAxis() << std::endl;
		std::cout << "  Translation   : " << this->m_FinalTransform->GetTranslation() << std::endl;
		std::cout << "  Scaling       : " << this->m_FinalTransform->GetScale() << std::endl;
		std::cout << "  Matrix        : " << this->m_FinalTransform->GetMatrix() << std::endl;
		std::cout << std::endl;

		return;
	}

} // end namespace

#endif