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
#include "itkChangeInformationImageFilter.h"
#include "itkResampleImageFilter.h"

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
	typedef itk::Image< unsigned char, 3 >			MaskImageType;
	typedef itk::Image< unsigned short, 3 >			ImageType;
	
	// method for creation
	itkNewMacro(Self);

	// run-time type information and related methods
	itkTypeMacro(ManageTransformsFilter, Object);

	// declare functions
	// manage transforms
	void AddTransform( TransformType::Pointer transform );
	void Print();
	void SaveTransform();
	
	// create/set images
	void SetImages( ImageType::Pointer fixedImage, ImageType::Pointer movingImage );
	MaskImageType::Pointer GenerateMaskFromROI( const char * filename );
	ImageType::Pointer GetTransformedImage();

	// apply transform
	void HardenTransformOn();
	void ResampleImageOn();
	void Update();

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
	ImageType::Pointer m_transformedImage;

	// flags
	bool m_hardenTransform;
	bool m_resampleImage;

	// creating mask file
	double * ExtractROIPoints( const char * filename );
	MaskImageType::Pointer CreateMask( double * roi );

	// applying transform
	void HardenTransform();
	void ResampleImage();
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkManageTransformsFilter.hxx"
#endif

#endif
