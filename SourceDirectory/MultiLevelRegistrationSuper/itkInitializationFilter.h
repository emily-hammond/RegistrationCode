/*
Author: Emily Hammond
Date: 2016 February 17

Purpose: This class is incorporated in the multi-level registration framework and is used 
for initialization of two images. It allows for initialization via center of geometry, or 
metric initialization along a specified axis. Multiple initialization methods are allowed
on the same two images.

Remaining to implement:
1. unset flags

*/

#ifndef __itkInitializationFilter_h
#define __itkInitializationFilter_h

// include files not defined in itkRegistrationFramework.h
#include "itkCenteredTransformInitializer.h"
#include "itkAffineTransform.h"

namespace itk
{
// class InitializationFilter
class InitializationFilter: public Object
{
public:
	// default ITK
	typedef InitializationFilter		Self;
	typedef Object						Superclass;
	typedef SmartPointer< Self >		Pointer;
	typedef SmartPointer< const Self >	ConstPointer;

	// definitions
	typedef itk::Image< short, 3 >			ImageType;
	typedef itk::ScaleVersor3DTransform< double >	TransformType;
	typedef itk::AffineTransform< double >			AffineTransformType;
	
	// method for creation
	itkNewMacro(Self);

	// run-time type information and related methods
	itkTypeMacro(InitializationFilter, Object);

	// images
	itkSetObjectMacro( FixedImage, ImageType );
	itkSetObjectMacro( MovingImage, ImageType );

	// set flags
	void ObserveOn()
	{ 
		this->m_ObserveOn = true; 
	}
	void ObserveOff()
	{
		this->m_ObserveOn = false;
	}
	void MetricTranslationOn(int axis);
	void MetricTranslationOff(int axis);
	void MetricRotationOn(int axis);
	void MetricRotationOff(int axis);
	void CenteredOnGeometryOn() 
	{ 
		this->m_CenteredOnGeometry = true;
	}
	void CenteredOnGeometryOff() 
	{ 
		this->m_CenteredOnGeometry = false;
	}

	// perform function
	void Update();
	void Update( AffineTransformType::Pointer transform );

	// get result
	itkGetObjectMacro( Transform, TransformType );

protected:
	// constructor
	InitializationFilter();
	
	// destructor
	virtual ~InitializationFilter() {}

	// this method is required to allocation memory for the output
	void GenerateData();
	
private:
	// declare variables
	ImageType::Pointer m_FixedImage;
	ImageType::Pointer m_MovingImage;
	TransformType::Pointer m_Transform;
	bool m_ObserveOn;

	// centered initialization
	bool m_CenteredOnGeometry;
	
	// metric translational initialization
	bool m_MetricTranslation0Flag;
	bool m_MetricTranslation1Flag;
	bool m_MetricTranslation2Flag;
	float m_TranslationRange;
	float m_MinMetric;
	TransformType::ParametersType m_MinParameters;

	// metric rotational initialization
	bool m_MetricRotation0Flag;
	bool m_MetricRotation1Flag;
	bool m_MetricRotation2Flag;

	// private functions
	void GetRange( int axis );
	void CenterOnGeometry();
	void MetricTranslationAlignment( int axis );
	void MetricRotationAlignment(int axis);
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkInitializationFilter.hxx"
#endif

#endif
