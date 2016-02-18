/*
Author: Emily Hammond
Date: 2016 February 

Purpose: This class is to hold all the transforms throughout the multi-level registration process.
It will store them in a composite transform and will account for all the application of each 
transform prior to validation.

*/

#ifndef __itkManageTransformsFilter_h
#define __itkManageTransformsFilter_h

// include files
#include "itkCompositeTransform.h"
#include "itkImageMaskSpatialObject.h"

namespace itk
{
// class Validation
class ManageTransformsFilter: public Object
{
public:
	// default ITK
	typedef ManageTransformsFilter		Self;
	typedef Object						Superclass;
	typedef SmartPointer< Self >		Pointer;
	typedef SmartPointer< const Self >	ConstPointer;

	// definitions
	typedef itk::CompositeTransform< double, 3 >	CompositeTransformType;
	typedef itk::ScaleVersor3DTransform< double >	TransformType;
	typedef itk::ImageMaskSpatialObject< 3 >		MaskType;
	typedef itk::Image< unsigned char, 3 >			MaskImageType;
	
	// method for creation
	itkNewMacro(Self);

	// run-time type information and related methods
	itkTypeMacro(ManageTransformsFilter, Object);

	// declare functions
	void AddTransform( TransformType::Pointer transform );
	void Print();
	void SaveTransform();
	void SetImages( ImageType::Pointer fixedImage, ImageType::Pointer movingImage );
	void GenerateMaskFromROI( const char * filename );
	
protected:
	// constructor
	ManageTransformsFilter();
	
	// destructor
	virtual ~ManageTransformsFilter() {}

	// this method is required to allocation memory for the output
	void GenerateData();
	
private:
	// declare variables
	CompositeTransformType::Pointer m_compositeTransform;
	ImageType::Pointer m_fixedImage;
	ImageType::Pointer m_movingImage;

	double * ExtractROIPoints( const char * filename );
	MaskImageType::Pointer CreateMask( double * roi );
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkManageTransformsFilter.hxx"
#endif

#endif
