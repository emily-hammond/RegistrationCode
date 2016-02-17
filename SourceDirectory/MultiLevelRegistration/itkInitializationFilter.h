/*
Author: Emily Hammond
Date: 2016 February 17

Purpose: This class is incorporated in the multi-level registration framework and is used 
for initialization of two images. It allows for initialization via center of geometry, or 
metric initialization along a specified axis. Multiple initialization methods are allowed
on the same two images.

*/

#ifndef __itkInitializationFilter_h
#define __itkInitializationFilter_h

// include files
#include "itkCenteredTransformInitializer.h"

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
	typedef itk::Image< unsigned short, 3 >			ImageType;
	typedef itk::ScaleVersor3DTransform< double >	TransformType;
	
	// method for creation
	itkNewMacro(Self);

	// run-time type information and related methods
	itkTypeMacro(InitializationFilter, Object);

	// declare functions
	void SetImages( ImageType::Pointer fixedImage, ImageType::Pointer movingImage );

	// set flags
	void ObserveOn() { this->m_observeFlag = true; }
	void MetricAlignmentOn( int axis );	
	void CenteredOnGeometryOn() 
	{ 
		this->m_centeredOnGeometryFlag = true;
		return;
	}

	// get initialization
	void PerformInitialization();
	TransformType::Pointer GetOutput();

protected:
	// constructor
	InitializationFilter();
	
	// destructor
	virtual ~InitializationFilter() {}

	// this method is required to allocation memory for the output
	void GenerateData();
	
private:
	// declare variables
	ImageType::Pointer m_fixedImage;
	ImageType::Pointer m_movingImage;
	TransformType::Pointer m_transform;
	bool m_observeFlag;

	// centered initialization
	bool m_centeredOnGeometryFlag;
	
	// metric initialization
	bool m_metricAlignment0Flag;
	bool m_metricAlignment1Flag;
	bool m_metricAlignment2Flag;
	float m_translationRange;
	float m_minMetric;
	TransformType::ParametersType m_minParameters;

	// private functions
	void GetRange( int axis );
	void CenterOnGeometry();
	void MetricAlignment( int axis );
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkInitializationFilter.hxx"
#endif

#endif
