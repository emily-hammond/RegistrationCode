/*
Author: Emily Hammond
Date: 2016 February 17

Purpose: This class is to wrap the registration framework used in multi-level registration.
It reads in a fixed and moving image and initial transform and performs registration on the
two images. The registration is composed of an MMI metric and a ScaleVersor3DTransform. The
transform is optimized with a RSGD optimizer fit for versor optimization. It currently uses 
the ITKv3 registration framework.

Remaining to implement:
1. unset flags
2. define different defaults

*/

#ifndef __itkRegistrationFramework_h
#define __itkRegistrationFramework_h

// include files
#include "itkImage.h"
#include "itkScaleVersor3DTransform.h"
#include "itkCompositeTransform.h"

#include "itkLinearInterpolateImageFunction.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkImageRegistrationMethod.h"
#include "RigidCommandIterationUpdate.h"

namespace itk
{
// class RegistrationFramework
template< typename TPixelType >
class RegistrationFramework: public Object
{
public:
	// default ITK
	typedef RegistrationFramework		Self;
	typedef Object						Superclass;
	typedef SmartPointer< Self >		Pointer;
	typedef SmartPointer< const Self >	ConstPointer;
	
	// definitions
	typedef itk::Image< TPixelType, 3 >				ImageType;
	typedef itk::Image< unsigned char, 3 >			MaskImageType;
	typedef itk::ScaleVersor3DTransform< double >	TransformType;

	// registration components
	typedef itk::LinearInterpolateImageFunction< typename ImageType, double >	InterpolatorType;
	typedef itk::MattesMutualInformationImageToImageMetric< typename ImageType, typename ImageType >	MetricType;
	typedef itk::VersorTransformOptimizer								OptimizerType;
	typedef itk::ImageRegistrationMethod< typename ImageType, typename ImageType >		RegistrationType;

	// method for creation
	itkNewMacro(Self);

	// run-time type information and related methods
	itkTypeMacro(RegistrationFramework, Object);

	// set variables
	itkSetObjectMacro( FixedImage, ImageType );
	itkSetObjectMacro( MovingImage, ImageType );
	itkSetObjectMacro( InitialTransform, TransformType );

	// set variables that might want to change
	itkSetMacro( MinimumStepLength, float );
	itkSetMacro( MaximumStepLength, float );
	itkSetMacro( NumberOfIterations, int );
	itkSetMacro( RelaxationFactor, float );
	itkSetMacro( GradientMagnitudeTolerance, float );
	itkSetMacro( RotationScale, float );
	itkSetMacro( TranslationScale, float );
	itkSetMacro( ScalingScale, float );
	itkSetMacro( DebugDirectory, std::string );

	// observer
	void ObserveOn()
	{
		this->m_ObserveOn = true;
		return;
	}
	void ObserveOff()
	{
		this->m_ObserveOn = false;
		return;
	}
	void DebugOn()
	{
		this->m_DebugOn = true;
		return;
	}
	void DebugOff()
	{
		this->m_DebugOn = false;
		return;
	}

	// get results
	itkGetObjectMacro( FinalTransform, TransformType );

	void Update();
	void Print();

protected:
	// constructor
	RegistrationFramework();
	
	// destructor
	virtual ~RegistrationFramework() {}

	// this method is required to allocation memory for the output
	void GenerateData();
	
private:
	// images
	typename ImageType::Pointer m_FixedImage;
	typename ImageType::Pointer m_MovingImage;

	// transforms
	TransformType::Pointer m_FinalTransform;
	TransformType::Pointer m_InitialTransform;

	// registration components
	typename TransformType::Pointer m_Transform;
	typename InterpolatorType::Pointer m_Interpolator;
	typename RegistrationType::Pointer m_Registration;
	
	// observer
	RigidCommandIterationUpdate::Pointer m_Observer;
	bool m_ObserveOn;
	bool m_DebugOn;
	std::string m_DebugDirectory;
	bool m_ObserverSet;

	// optimizer
	OptimizerType::Pointer m_Optimizer;
	float m_MinimumStepLength;
	float m_MaximumStepLength;
	int m_NumberOfIterations;
	float m_RelaxationFactor;
	float m_GradientMagnitudeTolerance;
	float m_RotationScale;
	float m_TranslationScale;
	float m_ScalingScale;

	// metric
	typename MetricType::Pointer m_Metric;
	float m_PercentageOfSamples;
	int m_HistogramBins;

	// private functions
	void Initialize();
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRegistrationFramework.hxx"
#endif

#endif
