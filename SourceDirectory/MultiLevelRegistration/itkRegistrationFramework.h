/*
insert comments here
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
/* class RegistrationFramework
 *
 * 
 
*/
class RegistrationFramework: public Object
{
public:
	typedef RegistrationFramework		Self;
	typedef Object						Superclass;
	typedef SmartPointer< Self >		Pointer;
	typedef SmartPointer< const Self >	ConstPointer;
	
	typedef itk::Image< unsigned short, 3 >			ImageType;
	typedef itk::ScaleVersor3DTransform< double >	TransformType;

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
	void ObserveOn() { this->m_observeFlag = true; }

protected:
	// declare the constructor and define default parameters
	RegistrationFramework();
	
	// destructor
	virtual ~RegistrationFramework() {}

	// this method is required to allocation memory for the output
	void GenerateData();
	
private:
	// declare variables
	ImageType::Pointer m_fixedImage;
	ImageType::Pointer m_movingImage;
	
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

	void SetDefaults();
	void SetUpMetric();
	void SetUpOptimizer();
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRegistrationFramework.hxx"
#endif

#endif
