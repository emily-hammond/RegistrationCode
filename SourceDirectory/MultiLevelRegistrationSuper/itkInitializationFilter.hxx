#ifndef __itkInitializationFilter_hxx
#define __itkInitializationFilter_hxx

#include "itkInitializationFilter.h"

namespace itk
{
	// contructor to set up initializations and transform
	InitializationFilter::InitializationFilter():
		m_CenteredOnGeometry( false ),
		m_MetricTranslation0Flag(false),
		m_MetricTranslation1Flag(false),
		m_MetricTranslation2Flag(false),
		m_MetricRotation0Flag(false),
		m_MetricRotation1Flag(false),
		m_MetricRotation2Flag(false),
		m_ObserveOn( false ),
		m_IterativeAlignment( false )
	{
		m_Transform = TransformType::New();
		TransformType::AxisType axis;
		axis[0] = 0; axis[1] = 0; axis[2] = 1;
		m_MinRotation.Set(axis, 0.0);
	}

	// set the flags for metric translation alignment by axis
	void InitializationFilter::MetricTranslationOn(int axis)
	{
		// x-axis
		if( axis == 0 )
		{
			this->m_MetricTranslation0Flag = true;
		}
		// y-axis
		else if( axis == 1 )
		{
			this->m_MetricTranslation1Flag = true;
		}
		// z-axis
		else if( axis == 2 )
		{
			this->m_MetricTranslation2Flag = true;
		}
		// error handling
		else
		{
			std::cout << "Axis number invalid. 0 = x, 1 = y, 2 = z" << std::endl;
		}
		return;
	}

	// set the flags for metric rotation alignment by axis
	void InitializationFilter::MetricRotationOn(int axis)
	{
		// x-axis
		if (axis == 0)
		{
			this->m_MetricRotation0Flag = true;
		}
		// y-axis
		else if (axis == 1)
		{
			this->m_MetricRotation1Flag = true;
		}
		// z-axis
		else if (axis == 2)
		{
			this->m_MetricRotation2Flag = true;
		}
		// error handling
		else
		{
			std::cout << "Axis number invalid. 0 = x, 1 = y, 2 = z" << std::endl;
		}
		return;
	}

	// set the flags for metric translation alignment by axis - turn off
	void InitializationFilter::MetricTranslationOff(int axis)
	{
		// x-axis
		if( axis == 0 )
		{
			this->m_MetricTranslation0Flag = false;
		}
		// y-axis
		else if( axis == 1 )
		{
			this->m_MetricTranslation1Flag = false;
		}
		// z-axis
		else if( axis == 2 )
		{
			this->m_MetricTranslation2Flag = false;
		}
		// error handling
		else
		{
			std::cout << "Axis number invalid. 0 = x, 1 = y, 2 = z" << std::endl;
		}
		return;
	}

	// set the flags for metric rotation alignment by axis - turn off
	void InitializationFilter::MetricRotationOff(int axis)
	{
		// x-axis
		if (axis == 0)
		{
			this->m_MetricRotation0Flag = false;
		}
		// y-axis
		else if (axis == 1)
		{
			this->m_MetricRotation1Flag = false;
		}
		// z-axis
		else if (axis == 2)
		{
			this->m_MetricRotation2Flag = false;
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
		// error checking
		if( !m_FixedImage )
		{
			itkExceptionMacro( << "FixedImage not present" );
		}
		if( !m_MovingImage )
		{
			itkExceptionMacro( << "MovingImage not present" );
		}

		// iterative alignment
		if (this->m_IterativeAlignment)
		{
			IterativeAlignment();
		}

		// center images based on geometry
		if( this->m_CenteredOnGeometry )
		{
			CenterOnGeometry();
		}
		// x-axis
		if (this->m_MetricTranslation0Flag)
		{
			MetricTranslationAlignment( 0 );
		}
		// y-axis
		if (this->m_MetricTranslation1Flag)
		{
			MetricTranslationAlignment( 1 );
		}
		// z-axis
		if (this->m_MetricTranslation2Flag)
		{
			MetricTranslationAlignment( 2 );
		}
		// x-axis
		if (this->m_MetricRotation0Flag)
		{
			MetricRotationAlignment(0);
		}
		// y-axis
		if (this->m_MetricRotation1Flag)
		{
			MetricRotationAlignment(1);
		}
		// z-axis
		if (this->m_MetricRotation2Flag)
		{
			MetricRotationAlignment(2);
		}

		return;
	}

	// read in initial transform and convert the affine transform to a ScaleVersorTransform
	void InitializationFilter::Update( AffineTransformType::Pointer transform )
	{
		// transfer parameters
		TransformType::ParametersType parameters = this->m_Transform->GetParameters();

		// versor
		TransformType::VersorType versor;
		versor.Set( transform->GetMatrix() );
		parameters[0] = versor.GetX();
		parameters[1] = versor.GetY();
		parameters[2] = versor.GetZ();

		// translation
		AffineTransformType::TranslationType affineTranslation = transform->GetTranslation();
		parameters[3] = affineTranslation[0];
		parameters[4] = affineTranslation[1];
		parameters[5] = affineTranslation[2];

		// DO NOT ALLOW SCALING ON AN INITIAL TRANSFORM
		parameters[6] = 1.0;
		parameters[7] = 1.0;
		parameters[8] = 1.0;

		//this->m_Transform->SetMatrix( matrix ); METHOD NOT SUPPORTED WITH ITKv4.8.0
		this->m_Transform->SetParameters( parameters );
	
		return;
	}

	// obtain the proper range to translate the moving image for metric alignment depending on the given axis
	void InitializationFilter::GetRange( int axis )
	{
		// moving image parameters
		const ImageType::SizeType & movingSize = this->m_MovingImage->GetLargestPossibleRegion().GetSize();
		const ImageType::SpacingType & movingSpacing = this->m_MovingImage->GetSpacing();

		// fixed image parameters
		const ImageType::SizeType & fixedSize = this->m_FixedImage->GetLargestPossibleRegion().GetSize();
		const ImageType::SpacingType & fixedSpacing = this->m_FixedImage->GetSpacing();

		// determine range
		this->m_TranslationRange = abs(fixedSize[ axis ]*fixedSpacing[ axis ] - movingSize[ axis ]*movingSpacing[ axis ]);

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
		initializer->SetTransform( this->m_Transform );
		initializer->SetFixedImage( this->m_FixedImage );
		initializer->SetMovingImage( this->m_MovingImage );
		
		// perform initialization
		initializer->InitializeTransform();

		std::cout << "Centered on geometry initialization complete." << std::endl;
	}

	// metric alignment based on the desired axis
	void InitializationFilter::MetricTranslationAlignment( int axis )
	{
		// instantiate metric to use
		typedef itk::MattesMutualInformationImageToImageMetric< ImageType, ImageType > MetricType;
		MetricType::Pointer mmi = MetricType::New();

		// connect interpolator
		typedef itk::LinearInterpolateImageFunction< ImageType, double >	InterpolatorType;
		InterpolatorType::Pointer interpolator = InterpolatorType::New();

		// set parameters
		mmi->SetFixedImage( this->m_FixedImage );
		mmi->SetMovingImage( this->m_MovingImage );
		mmi->SetFixedImageRegion( this->m_FixedImage->GetLargestPossibleRegion() );
		mmi->SetTransform( this->m_Transform );
		mmi->SetInterpolator( interpolator );

		// initialize metric
		mmi->Initialize();

		// obtain current transform parameters
		TransformType::ParametersType parameters = this->m_Transform->GetParameters();

		// initializations
		this->m_MinMetric = 100000.0;
		this->m_MinParameters = parameters;

		// get desired translation range and calculate start and end parameters
		this->GetRange( axis );
		float start = parameters[ axis + 3 ] - this->m_TranslationRange/1.5;
		float end = parameters[ axis + 3 ] + this->m_TranslationRange/1.5;

		// header for section
		if( this->m_ObserveOn )
		{
			std::cout << "\nMetric Initialization on " << axis << " axis:\n";
			std::cout << "Translation range: " << m_TranslationRange << std::endl;
		}

		// parse through translation range and determine smallest metric value
		for( float i = start; i < end; i = i - (start-end)/20.0)
		{
			// change parameters
			parameters[ axis + 3 ] = i;
			// store parameters and corresponding metric into array
			if( mmi->GetValue( parameters ) < this->m_MinMetric )
			{
				this->m_MinMetric = mmi->GetValue( parameters );
				this->m_MinParameters = parameters;
			}

			// print out results if observing on
			if( this->m_ObserveOn )
			{
				std::cout << "Metric: " << mmi->GetValue(parameters)  << "    Parameters: ";
				for( int j = 0; j < 9; ++j )
				{
					std::cout << parameters[j] << ", ";
				}
				std::cout << std::endl;
			}
		}

		// save results into transform
		this->m_Transform->SetParameters( this->m_MinParameters );
		//std::cout << this->m_transform << std::endl;
	
		if( this->m_ObserveOn ){ std::cout << std::endl; }
		std::cout << "Metric initialization on " << axis << " complete." << std::endl;
		return;
	}

	// apply rotation
	void InitializationFilter::MetricRotationAlignment(int axis)
	{
		// instantiate metric to use
		typedef itk::MattesMutualInformationImageToImageMetric< ImageType, ImageType > MetricType;
		MetricType::Pointer mmi = MetricType::New();

		// connect interpolator
		typedef itk::LinearInterpolateImageFunction< ImageType, double >	InterpolatorType;
		InterpolatorType::Pointer interpolator = InterpolatorType::New();

		// set parameters
		mmi->SetFixedImage(this->m_FixedImage);
		mmi->SetMovingImage(this->m_MovingImage);
		mmi->SetFixedImageRegion(this->m_FixedImage->GetLargestPossibleRegion());
		mmi->SetTransform(this->m_Transform);
		mmi->SetInterpolator(interpolator);

		// initialize metric
		mmi->Initialize();

		TransformType::ParametersType parameters = this->m_Transform->GetParameters();

		// create axis of rotation and set desired axis to 1;
		TransformType::VersorType rotation;
		TransformType::AxisType rotAxis;
		rotAxis[0] = 0;
		rotAxis[1] = 0;
		rotAxis[2] = 0;
		rotAxis[axis] = 1;

		// initialization
		this->m_MinMetric = 1000000.0;
		this->m_MinParameters = m_Transform->GetParameters();
		for (int j = 0; j < 9; ++j)
		{
			std::cout << parameters[j] << ", ";
		}
		std::cout << std::endl;

		// header for section
		if (this->m_ObserveOn)
		{
			std::cout << "\nMetric Initialization on " << axis << " axis:\n";
			std::cout << "Rotation range: -45 deg to 45 deg" << std::endl;
		}

		float start = -45.0 * (3.141592653589793238463 / 180.0);
		float end = 45.0 * (3.141592653589793238463 / 180.0);

		// parse through rotation range and determine smallest metric value
		for (float i = start; i < end; i = i + std::abs(start - end) / 20)
		{
			// create rotation and set parameters
			rotation.Set(rotAxis, i);
			this->m_MinRotation = rotation*this->m_MinRotation;

			this->m_Transform->SetRotation(rotation);
			double versor[4] = { rotation.GetX(), rotation.GetY(), rotation.GetZ(), rotation.GetW() };
			parameters[axis] = versor[axis];
			
			// store parameters and corresponding metric into array
			try
			{
				MetricType::MeasureType value = mmi->GetValue(parameters);
				if (value < this->m_MinMetric)
				{
					this->m_MinMetric = mmi->GetValue(parameters);
					this->m_MinParameters = parameters;
				}
				// print out results if observing on
				if (this->m_ObserveOn)
				{
					std::cout << "Metric : " << mmi->GetValue(parameters) << "    Parameters: ";
					for (int j = 0; j < 9; ++j)
					{
						std::cout << m_Transform->GetParameters()[j] << ", ";
					}
					std::cout << std::endl;
				}
			}
			catch (itk::ExceptionObject & err)
			{
			}
		}

		// save results into transform
		this->m_Transform->SetParameters(this->m_MinParameters);

		if (this->m_ObserveOn){ std::cout << std::endl; }
		std::cout << "Metric initialization on " << axis << " complete." << std::endl;
		
		return;
	}

	void InitializationFilter::IterativeAlignment()
	{
		// instantiate metric to use
		typedef itk::MattesMutualInformationImageToImageMetric< ImageType, ImageType > MetricType;
		MetricType::Pointer mmi = MetricType::New();

		// connect interpolator
		typedef itk::LinearInterpolateImageFunction< ImageType, double >	InterpolatorType;
		InterpolatorType::Pointer interpolator = InterpolatorType::New();

		// set parameters
		mmi->SetFixedImage(this->m_FixedImage);
		mmi->SetMovingImage(this->m_MovingImage);
		mmi->SetFixedImageRegion(this->m_FixedImage->GetLargestPossibleRegion());
		mmi->SetTransform(this->m_Transform);
		mmi->SetInterpolator(interpolator);

		// initialize metric
		mmi->Initialize();

		// initialization
		this->m_MinMetric = 1000000.0;

		// header for section
		if (this->m_ObserveOn)
		{
			std::cout << "\nIterative alignment\n";
		}

		// determine size of fixed image
		ImageType::SizeType fixedSize = this->m_FixedImage->GetLargestPossibleRegion().GetSize();

		// find index of the middle of moving image
		ImageType::SizeType movingSize = this->m_MovingImage->GetLargestPossibleRegion().GetSize();
		ImageType::IndexType middleIndex;
		middleIndex[0] = movingSize[0] / 2;
		middleIndex[1] = movingSize[1] / 2;
		middleIndex[2] = movingSize[2] / 2;

		// find physical location of the middle index
		ImageType::PointType middlePoint;
		this->m_MovingImage->TransformIndexToPhysicalPoint(middleIndex, middlePoint);

		// iterate and find metric at 125 specific points
		int n = 25; int o = 5;
		for (int i = o; i < (n-o); i++)
		{
			for (int j = o; j < (n - o); j++)
			{
				for (int k = o; k < (n - o); k++)
				{
					// find fixed index
					ImageType::IndexType fixedIndex;
					fixedIndex[0] = i*fixedSize[0] / n;
					fixedIndex[1] = j*fixedSize[1] / n;
					fixedIndex[2] = k*fixedSize[2] / n; 

					// find fixed point
					ImageType::PointType fixedPoint;
					this->m_FixedImage->TransformIndexToPhysicalPoint(fixedIndex, fixedPoint);

					// determine translation
					TransformType::ParametersType parameters = this->m_Transform->GetParameters();
					parameters[3] = middlePoint[0] - fixedPoint[0];
					parameters[4] = middlePoint[1] - fixedPoint[1];
					parameters[5] = middlePoint[2] - fixedPoint[2];

					// calculate metric
					if (mmi->GetValue(parameters) < this->m_MinMetric)
					{
						this->m_MinMetric = mmi->GetValue(parameters);
						this->m_MinParameters = parameters;
					}

					/*// print out results if observing on
					if (this->m_ObserveOn)
					{
						std::cout << "Metric: " << mmi->GetValue(parameters) << "    Parameters: ";
						for (int j = 0; j < 9; ++j)
						{
							std::cout << parameters[j] << ", ";
						}
						std::cout << std::endl;
					}*/
				}
			}
		}

		// save results into transform
		this->m_Transform->SetParameters(this->m_MinParameters);
		//std::cout << this->m_transform << std::endl;

		if (this->m_ObserveOn)
		{ 
			for (int j = 0; j < 9; ++j)
			{
				std::cout << m_Transform->GetParameters()[j] << ", ";
			}
			std::cout << std::endl; 
		}
		std::cout << "Iterative alignment complete." << std::endl;
		return;

	}

} // end namespace

#endif