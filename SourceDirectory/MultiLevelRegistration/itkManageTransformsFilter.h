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
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkExtractImageFilter.h"

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
	typedef itk::Image< unsigned short, 3 >			ImageType;
	
	// method for creation
	itkNewMacro(Self);

	// run-time type information and related methods
	itkTypeMacro(ManageTransformsFilter, Object);

	// declare functions
	// manage transforms
	void AddTransform( TransformType::Pointer transform );
	itkSetObjectMacro( InitialTransform, TransformType );
	itkSetObjectMacro( Transform, TransformType );
	
	// create/set images
	itkSetObjectMacro( FixedImage, ImageType );
	itkSetObjectMacro( MovingImage, ImageType );
	itkSetObjectMacro( MovingLabelMap, ImageType );
	void SetROIFilename( char * filename )
	{
		this->m_ROIFilename = filename;
	}

	// get results
	itkGetObjectMacro( TransformedImage, ImageType );
	itkGetObjectMacro( TransformedLabelMap, ImageType );
	itkGetObjectMacro( CompositeTransform, CompositeTransformType );
	itkGetObjectMacro( FixedCroppedImage, ImageType );
	itkGetObjectMacro( MovingCroppedImage, ImageType );

	// Harden transform flag
	void HardenTransformOn()
	{
		m_ResampleImage = false;
		m_HardenTransform = true;
	}
	void HardenTransformOff()
	{
		m_HardenTransform = false;
	}

	// resample image flag
	void ResampleImageOn()
	{
		m_ResampleImage = true;
		m_HardenTransform = false;
	}
	void ResampleImageOff()
	{
		m_ResampleImage = false;
	}

	// crop image
	void CropImageOn()
	{
		m_CropImage = true;
	}
	void CropImageOff()
	{
		m_CropImage = false;
	}

	// perform function
	void Update();

protected:
	// constructor
	ManageTransformsFilter();
	
	// destructor
	virtual ~ManageTransformsFilter() {}

	// this method is required to allocation memory for the output
	void GenerateData();
	
private:
	// transforms
	CompositeTransformType::Pointer m_CompositeTransform;
	TransformType::Pointer m_InitialTransform;
	TransformType::Pointer m_Transform;

	// images
	ImageType::Pointer m_FixedImage;
	ImageType::Pointer m_MovingImage;
	ImageType::Pointer m_MovingLabelMap;
	ImageType::Pointer m_TransformedImage;
	ImageType::Pointer m_TransformedLabelMap;
	ImageType::Pointer m_FixedCroppedImage;
	ImageType::Pointer m_MovingCroppedImage;

	// flags
	bool m_HardenTransform;
	bool m_ResampleImage;
	bool m_NearestNeighbor;
	bool m_CropImage;
	ImageType::RegionType m_CropRegion;

	// ROI
	char * m_ROIFilename;
	ImageType::Pointer CropImage( ImageType::Pointer image );
	double * ExtractROIPoints();


	// use NN interpolation during resampling
	void NearestNeighborInterpolateOn()
	{
		m_NearestNeighbor = true;
	}
	void NearestNeighborInterpolateOff()
	{
		m_NearestNeighbor = false;
	}

	// applying transform
	void HardenTransform();
	ImageType::Pointer ResampleImage( ImageType::Pointer image );
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkManageTransformsFilter.hxx"
#endif

#endif
