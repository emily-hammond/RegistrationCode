#ifndef __itkRegistrationFramework_hxx
#define __itkRegistrationFramework_hxx

#include "itkRegistrationFramework.h"

namespace itk
{
	// set up defaults in constructor
	RegistrationFramework::RegistrationFramework():
		// images
		m_FixedImage( ITK_NULLPTR ),	// provided by user
		m_MovingImage( ITK_NULLPTR ),	// provided by user

		// mask
		m_ROIFilename( NULL ),			// provided by user
		m_MaskObject( ITK_NULLPTR ),	// generated from ROI 
		m_MaskImage( ITK_NULLPTR ),		// generated from ROI

		// transforms
		m_InitialTransform( ITK_NULLPTR ),	// provided by user

		// metric
		m_PercentageOfSamples( 0.01 ),
		m_HistogramBins( 50 ),

		// optimizer
		m_MinimumStepLength( 0.0025 ),
		m_MaximumStepLength( 0.5 ),
		m_NumberOfIterations( 50 ),
		m_RelaxationFactor( 0.8 ),
		m_GradientMagnitudeTolerance( 0.001 ),
		m_RotationScale( 0.01 ),
		m_TranslationScale( 5 ),
		m_ScalingScale( 0.01 ),
		m_ObserveOn( false )
	{
		// observer
		m_Transform = TransformType::New();

		// registration components
		m_Interpolator = InterpolatorType::New();
		m_Metric = MetricType::New();
		m_Optimizer = OptimizerType::New();
		m_Registration = RegistrationType::New();
		m_Observer = RigidCommandIterationUpdate::New();
	}

	// run through registration process
	void RegistrationFramework::Update()
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
		this->m_Registration->SetInitialTransformParameters( this->m_InitialTransform->GetParameters() );
		this->m_Registration->SetFixedImageRegion( this->m_FixedImage->GetBufferedRegion() );

		// begin registration
		std::cout << "Begin registration." << std::endl;
		try
		{
			this->m_Registration->Update();
			std::cout << "OptimizerStopCondition: " << this->m_Registration->GetOptimizer()->GetStopConditionDescription() << std::endl;
		}
		catch( itk::ExceptionObject & err )
		{
			std::cerr << "ExceptionObjectCaught!" << std::endl;
			std::cerr << err << std::endl;
			return;
		}

		// get final transform
		m_FinalTransform = TransformType::New();
		this->m_FinalTransform->SetParameters( this->m_Registration->GetLastTransformParameters() );
		this->m_FinalTransform->SetFixedParameters( this->m_Transform->GetFixedParameters() );
		
		std::cout << "Registration performed." << std::endl;
		return;
	}

	// set up MMI metric for defaults
	void RegistrationFramework::Initialize()
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

		// mask
		if( m_ROIFilename )
		{
			CreateMask(  );
			m_Metric->SetFixedImageMask( m_MaskObject );
			std::cout << "Mask object created." << std::endl;
		}

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
			this->m_Optimizer->AddObserver( itk::IterationEvent(), this->m_Observer );
			std::cout << "Set to observe registration process." << std::endl;
		}

		return;
	}

	// create the mask given an ROI filename
	void RegistrationFramework::CreateMask()
	{
		// extract ROI points from the file
		double * roi = ExtractROIPoints();

		// input fixed image properties into mask image
		m_MaskImage->SetRegions( this->m_FixedImage->GetLargestPossibleRegion() );
		m_MaskImage->SetOrigin( this->m_FixedImage->GetOrigin() );
		m_MaskImage->SetSpacing( this->m_FixedImage->GetSpacing() );
		m_MaskImage->SetDirection( this->m_FixedImage->GetDirection() );
		m_MaskImage->Allocate();

		// extract center and radius
		double c[3] = {-*(roi),-*(roi+1),*(roi+2)};
		double r[3] = {*(roi+3),*(roi+4),*(roi+5)};
		
		// create size of mask according to the roi array
		// set start index of mask according to the roi array
		MaskImageType::PointType startPoint, endPoint;
		startPoint[0] = c[0] - r[0];
		startPoint[1] = c[1] - r[1];
		startPoint[2] = c[2] - r[2];

		// find end index
		endPoint[0] = c[0] + r[0];
		endPoint[1] = c[1] + r[1];
		endPoint[2] = c[2] + r[2];

		// convert to indices
		MaskImageType::IndexType startIndex, endIndex;
		m_MaskImage->TransformPhysicalPointToIndex( startPoint, startIndex );
		m_MaskImage->TransformPhysicalPointToIndex( endPoint, endIndex );

		// plug into region
		MaskImageType::SizeType regionSize;
		regionSize[0] = abs( startIndex[0] - endIndex[0] );
		regionSize[1] = abs( startIndex[1] - endIndex[1] );
		regionSize[2] = abs( startIndex[2] - endIndex[2] );

		m_MaskRegion.SetSize( regionSize );
		m_MaskRegion.SetIndex( startIndex );

		// iterate over region and set pixels to white
		itk::ImageRegionIterator< MaskImageType > it( m_MaskImage, m_MaskRegion );
		while( !it.IsAtEnd() )
		{
			it.Set( 255 );
			++it;
		}

		m_MaskObject->SetImage( m_MaskImage );

		return;
	}

	// extract point values from the slicer ROI file
	double * RegistrationFramework::ExtractROIPoints()
	{
		// instantiate ROI array
		static double roi[] = {0.0,0.0,0.0,0.0,0.0,0.0};
		bool fullROI = false; // denotes that ROI array is full
		int numberOfPoints = 0;

		// open file and extract lines
		std::ifstream file( this->m_ROIFilename );
		std::string line;
		// iterate through file
		while( getline( file,line ) )
		{
			// look at uncommented lines only unless point is found
			if( line.compare(0,1,"#") != 0 && !fullROI )
			{
				// allocate position array and indices
				int positionsOfBars[4] = {0};
				int numberOfBars = 0;
				int positionOfBar = 0;
				
				// iterate through the line in the file to find the | (bar) locations
				// example line: point|18.8396|305.532|-458.046|1|1
				for( std::string::iterator it = line.begin(); it != line.end(); ++it )
				{
					// only note the first 4 locations
					if( (*it == '|') && numberOfBars < 4 )
					{
						positionsOfBars[numberOfBars] = positionOfBar;
						++numberOfBars;
					}
					++positionOfBar;
				}

				// extract each point and place into ROI array
				for( int i = 0; i < 3; ++i )
				{
					roi[numberOfPoints] = atof( line.substr( positionsOfBars[i]+1, positionsOfBars[i+1]-positionsOfBars[i]-1 ).c_str() );
					++numberOfPoints;

					// set flag to false if the roi array is filled
					if( numberOfPoints > 5 )
					{
						fullROI = true;
					}
				}
			}
		}


		// array is output as [ centerx, centery, centerz, radiusx, radiusy, radiusz ]
		return roi;
	}

	// get final transform
	void RegistrationFramework::Print()
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

		// mask generation
		if( m_ROIFilename )
		{
			std::cout << "\nMask generation" << std::endl;
			std::cout << m_MaskRegion << std::endl;
		}
		
		// print out final optimizer parameters
		std::cout << "\nFinal Parameters" << std::endl;
		std::cout << "  Iterations    : " << this->m_Optimizer->GetCurrentIteration() << std::endl;
		std::cout << "  Metric        : " << this->m_Optimizer->GetValue() << std::endl;
		std::cout << "  Stop Condition: " << this->m_Registration->GetOptimizer()->GetStopConditionDescription() << std::endl;

		// final transform
		std::cout << "Transform " << std::endl;
		std::cout << "  Angle         : " << this->m_FinalTransform->GetVersor().GetAngle() << std::endl;
		//std::cout << "  Axis          : " << this->m_FinalTransform->GetVersor().GetAxis() << std::endl;
		std::cout << "  Translation   : " << this->m_FinalTransform->GetTranslation() << std::endl;
		std::cout << "  Scaling       : " << this->m_FinalTransform->GetScale() << std::endl;
		std::cout << std::endl;

		return;
	}

} // end namespace

#endif