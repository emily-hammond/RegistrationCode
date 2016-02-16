/*
insert comments here
*/

#ifndef __itkInitializationFilter_h
#define __itkInitializationFilter_h

// include files
#include "itkCenteredTransformInitializer.h"

namespace itk
{
/* class InitializationFilter
 *
 * 
 
*/
class InitializationFilter: public Object
{
public:
	typedef InitializationFilter		Self;
	typedef Object						Superclass;
	typedef SmartPointer< Self >		Pointer;
	typedef SmartPointer< const Self >	ConstPointer;

	typedef itk::Image< unsigned short, 3 >			ImageType;
	typedef itk::ScaleVersor3DTransform< double >	TransformType;
	
	// method for creation
	itkNewMacro(Self);

	// run-time type information and related methods
	itkTypeMacro(InitializationFilter, Object);

	// declare functions
	void SetImages( ImageType::Pointer fixedImage, ImageType::Pointer movingImage );

	// set flags
	void CenteredOnGeometry() { this->m_centeredOnGeometryFlag = true; }
	void MetricAlignment( int axis ) 
	{ 
		this->m_metricAlignmentFlag = true;
		this->m_metricAxis = axis;
	}

	// get initialization
	void PerformInitialization();
	TransformType::Pointer GetOutput();

protected:
	// declare the constructor and define default parameters
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

	// centered initialization
	bool m_centeredOnGeometryFlag;
	
	// metric initialization
	bool m_metricAlignmentFlag;
	int m_metricAxis;
	
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkInitializationFilter.hxx"
#endif

#endif
