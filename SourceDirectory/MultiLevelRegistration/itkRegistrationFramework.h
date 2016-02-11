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
	typedef itk::MattesMutualInformationImageToImageMetric< ImageType, ImageType >	MetricType;

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
	InterpolatorType::Pointer m_interpolator;
	MetricType::Pointer m_metric;
	float m_PercentageOfSamples;
	int m_HistogramBins;

	void SetDefaults();
	void SetUpMetric();
	
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRegistrationFramework.hxx"
#endif

#endif
