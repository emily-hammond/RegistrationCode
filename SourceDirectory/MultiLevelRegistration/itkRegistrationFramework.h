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
class RegistrationFramework: public Object
{
public:
	// default ITK
	typedef RegistrationFramework		Self;
	typedef Object						Superclass;
	typedef SmartPointer< Self >		Pointer;
	typedef SmartPointer< const Self >	ConstPointer;
	
	// definitions
	typedef itk::Image< unsigned short, 3 >			ImageType;
	typedef itk::ScaleVersor3DTransform< double >	TransformType;

	// registration components
	typedef itk::LinearInterpolateImageFunction< ImageType, double >	InterpolatorType;
	typedef itk::MattesMutualInformationImageToImageMetric< ImageType, ImageType >	MetricType;
	typedef itk::VersorTransformOptimizer								OptimizerType;
	typedef itk::ImageRegistrationMethod< ImageType, ImageType >		RegistrationType;

	// method for creation
	itkNewMacro(Self);

	// run-time type information and related methods
	itkTypeMacro(RegistrationFramework, Object);

	// declare functions
	void SetImages( ImageType::Pointer fixedImage, ImageType::Pointer movingImage );
	void SetInitialTransform( TransformType::Pointer initialTransform );

	void PerformRegistration();
	TransformType::Pointer GetOutput();
	void ObserveOn()
	{ 
		this->m_observeFlag = true;
		return;
	}

protected:
	// constructor
	RegistrationFramework();
	
	// destructor
	virtual ~RegistrationFramework() {}

	// this method is required to allocation memory for the output
	void GenerateData();
	
private:
	// declare variables
	ImageType::Pointer m_fixedImage;
	ImageType::Pointer m_movingImage;
	
	// registration components
	TransformType::Pointer m_initialTransform;
	TransformType::Pointer m_transform;
	InterpolatorType::Pointer m_interpolator;
	RegistrationType::Pointer m_registration;
	
	// observer
	RigidCommandIterationUpdate::Pointer m_observer;
	bool m_observeFlag;

	// optimizer
	OptimizerType::Pointer m_optimizer;
	float m_minimumStepLength;
	float m_maximumStepLength;
	int m_numberOfIterations;
	float m_relaxationFactor;
	float m_gradientMagnitudeTolerance;
	float m_rotationScale;
	float m_translationScale;
	float m_scalingScale;

	// metric
	MetricType::Pointer m_metric;
	float m_percentageOfSamples;
	int m_histogramBins;

	// private functions
	void SetDefaults();
	void SetUpMetric();
	void SetUpOptimizer();
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRegistrationFramework.hxx"
#endif

#endif
