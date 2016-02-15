/*
insert comments here
*/

#ifndef __itkRegistrationFramework_h
#define __itkRegistrationFramework_h

// include files
#include "itkImage.h"
#include "itkScaleVersor3DTransform.h"
#include "itkCompositeTransform.h"

#include "itkMattesMutualInformationImageToImageMetricv4.h"
#include "itkRegularStepGradientDescentOptimizerv4.h"
#include "itkImageRegistrationMethodv4.h"

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
	
	typedef itk::Image< float, 3 >			ImageType;
	typedef itk::ScaleVersor3DTransform< double >	RigidTransformType;

	typedef itk::MattesMutualInformationImageToImageMetricv4< ImageType, ImageType >	MetricType;
	typedef itk::RegularStepGradientDescentOptimizerv4< double >		OptimizerType;
	typedef itk::ImageRegistrationMethodv4< ImageType, ImageType >		RegistrationType;

	// method for creation
	itkNewMacro(Self);

	// run-time type information and related methods
	itkTypeMacro(RegistrationFramework, Object);

	// declare functions
	void SetImages( ImageType::Pointer fixedImage, ImageType::Pointer movingImage );
	void SetInitialTransform( RigidTransformType::Pointer initialTransform );
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
	
	// transforms
	RigidTransformType::Pointer m_transform;
	RigidTransformType::Pointer m_initialTransform;
	
	// other components
	RegistrationType::Pointer m_registration;

	// optimizer
	OptimizerType::Pointer m_optimizer;
	float m_minimumStepLength;
	int m_numberOfIterations;
	float m_relaxationFactor;
	float m_gradientMagnitudeTolerance;
	//OptimizerType::ScalesType m_scales;
	float m_rotationScale;
	float m_translationScale;
	float m_scalingScale;
	float m_learningRate;

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
