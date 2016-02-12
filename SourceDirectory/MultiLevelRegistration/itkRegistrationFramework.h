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
#include "itkVersorTransformOptimizer.h"

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
	typedef itk::ScaleVersor3DTransform< double >	RigidTransformType;
	typedef itk::CompositeTransform< double, 3 >	CompositeTransformType;

	typedef itk::LinearInterpolateImageFunction< ImageType, double >	InterpolatorType;
	typedef itk::MattesMutualInformationImageToImageMetricv4< ImageType, ImageType >	MetricType;
	typedef itk::VersorTransform					OptimizerType;

	// method for creation
	itkNewMacro(Self);

	// run-time type information and related methods
	itkTypeMacro(RegistrationFramework, Object);

	// declare functions
	void SetImages( ImageType::Pointer fixedImage, ImageType::Pointer movingImage );
	void SetInitialTransform( RigidTransformType::Pointer initialTransform );
	void SetInitialTransform( CompositeTransformType::Pointer initialTransform );

	void PerformRegistration();

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
	
	CompositeTransformType::Pointer m_transforms;
	RigidTransformType::Pointer m_transform;
	InterpolatorType::Pointer m_interpolator;

	// optimizer
	OptimizerType::Pointer m_optimizer;
	float m_minimumStepLength;
	float m_maximumStepLength;
	int m_numberOfIterations;
	float m_relaxationFactor;
	float m_gradientMagnitudeTolerance;
	RigidOptimizerType::ScalesType m_scales;
	float m_rotationScale;
	float m_translationScale;
	float m_scalingScale;

	// metric
	MetricType::Pointer m_metric;
	float m_percentageOfSamples;
	int m_pistogramBins;

	void SetDefaults();
	void SetUpMetric();
	void SetUpOptimizer();
	
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRegistrationFramework.hxx"
#endif

#endif
