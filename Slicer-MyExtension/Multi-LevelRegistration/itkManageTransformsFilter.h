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
#include "itkScaleVersor3DTransform.h"
#include "itkChangeInformationImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkExtractImageFilter.h"

namespace itk
{
// class Validation
template< typename TPixelType >
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
	typedef itk::Image< TPixelType, 3 >				ImageType;
	typedef itk::Image< unsigned char, 3 >			MaskImageType;
	
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
	itkSetObjectMacro( FixedLabelMap, MaskImageType );
	itkSetObjectMacro( MovingImage, ImageType );
	itkSetObjectMacro( MovingLabelMap, MaskImageType );
	void SetROIFilename(const char * filename);
	void SetROI(std::vector<float> roi);

	// get results
	itkGetObjectMacro( TransformedImage, ImageType );
	itkGetObjectMacro( TransformedLabelMap, MaskImageType );
	itkGetObjectMacro( CompositeTransform, CompositeTransformType );
	itkGetObjectMacro( FixedCroppedImage, ImageType );
	itkGetObjectMacro( FixedCroppedLabelMap, MaskImageType );
	itkGetObjectMacro( MovingCroppedImage, ImageType );
	itkGetObjectMacro( MovingCroppedLabelMap, MaskImageType );

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
	template< typename TImageType > 
	typename TImageType::Pointer ResampleImage(typename TImageType::Pointer image, TransformType::Pointer transform)
	{
		// set up resampling object
		typedef itk::ResampleImageFilter< TImageType, TImageType >	ResampleFilterType;
		ResampleFilterType::Pointer resample = ResampleFilterType::New();

		if (!this->m_FixedImage)
		{
			std::cout << "Fixed Image not defined. " << std::endl;
			return image;
		}

		// define image resampling with respect to fixed image
		resample->SetSize(this->m_FixedImage->GetLargestPossibleRegion().GetSize());
		resample->SetOutputOrigin(this->m_FixedImage->GetOrigin());
		resample->SetOutputSpacing(this->m_FixedImage->GetSpacing());
		resample->SetOutputDirection(this->m_FixedImage->GetDirection());

		// input parameters
		resample->SetInput(image);
		resample->SetTransform(transform);

		// define interpolator
		ResampleFilterType::InterpolatorType * linInterpolator = resample->GetInterpolator();
		typedef itk::NearestNeighborInterpolateImageFunction< TImageType, double > NearestNeighborType;
		NearestNeighborType::Pointer nnInterpolator = NearestNeighborType::New();

		if (this->m_NearestNeighbor)
		{
			resample->SetInterpolator(nnInterpolator);
			std::cout << "Nearest neighbor interpolator." << std::endl;
		}
		else
		{
			resample->SetInterpolator(linInterpolator);
		}

		// apply
		resample->Update();

		return resample->GetOutput();
	};

	template< typename TImageType >
	typename TImageType::Pointer ResampleImage(typename TImageType::Pointer image, CompositeTransformType::Pointer transform)
	{
		// set up resampling object
		typedef itk::ResampleImageFilter< TImageType, TImageType >	ResampleFilterType;
		ResampleFilterType::Pointer resample = ResampleFilterType::New();

		if (!this->m_FixedImage)
		{
			std::cout << "Fixed Image not defined. " << std::endl;
			return image;
		}

		// define image resampling with respect to fixed image
		resample->SetSize(this->m_FixedImage->GetLargestPossibleRegion().GetSize());
		resample->SetOutputOrigin(this->m_FixedImage->GetOrigin());
		resample->SetOutputSpacing(this->m_FixedImage->GetSpacing());
		resample->SetOutputDirection(this->m_FixedImage->GetDirection());

		// input parameters
		resample->SetInput(image);
		resample->SetTransform(transform);

		// define interpolator
		ResampleFilterType::InterpolatorType * linInterpolator = resample->GetInterpolator();
		typedef itk::NearestNeighborInterpolateImageFunction< TImageType, double > NearestNeighborType;
		NearestNeighborType::Pointer nnInterpolator = NearestNeighborType::New();

		if (this->m_NearestNeighbor)
		{
			resample->SetInterpolator(nnInterpolator);
			std::cout << "Nearest neighbor interpolator." << std::endl;
		}
		else
		{
			resample->SetInterpolator(linInterpolator);
		}

		// apply
		resample->Update();

		return resample->GetOutput();
	};

	// use NN interpolation during resampling
	void NearestNeighborInterpolateOn()
	{
		m_NearestNeighbor = true;
	}
	void NearestNeighborInterpolateOff()
	{
		m_NearestNeighbor = false;
	}

	std::vector<float> ExtractROIPoints( const char * filename);

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
	typename ImageType::Pointer m_FixedImage;
	MaskImageType::Pointer m_FixedLabelMap;
	typename ImageType::Pointer m_MovingImage;
	MaskImageType::Pointer m_MovingLabelMap;
	typename ImageType::Pointer m_TransformedImage;
	MaskImageType::Pointer m_TransformedLabelMap;
	typename ImageType::Pointer m_FixedCroppedImage;
	MaskImageType::Pointer m_FixedCroppedLabelMap;
	typename ImageType::Pointer m_MovingCroppedImage;
	MaskImageType::Pointer m_MovingCroppedLabelMap;

	// flags
	bool m_HardenTransform;
	bool m_ResampleImage;
	bool m_NearestNeighbor;
	bool m_CropImage;
	typename ImageType::RegionType m_CropRegion;

	// ROI
	const char * m_ROIFilename;
	std::vector<float> m_ROI;
	template< typename TImageType >
	typename TImageType::Pointer CropImage(typename TImageType::Pointer image)	// used with reading in ROI from *.ascv file
	{
		std::vector<float>::iterator it = m_ROI.begin();

		// extract center and radius
		double c[3] = { -*(it), -*(it + 1), *(it + 2) };
		double r[3] = { *(it + 3), *(it + 4), *(it + 5) };

		// create size of mask according to the roi array
		// set start index of mask according to the roi array
		TImageType::PointType startPoint, endPoint;
		startPoint[0] = c[0] - r[0];
		startPoint[1] = c[1] - r[1];
		startPoint[2] = c[2] - r[2];

		// find end index
		endPoint[0] = c[0] + r[0];
		endPoint[1] = c[1] + r[1];
		endPoint[2] = c[2] + r[2];

		// convert to indices
		TImageType::IndexType startIndex, endIndex;
		image->TransformPhysicalPointToIndex(startPoint, startIndex);
		image->TransformPhysicalPointToIndex(endPoint, endIndex);

		// plug into region
		TImageType::SizeType regionSize;
		regionSize[0] = abs(startIndex[0] - endIndex[0]);
		regionSize[1] = abs(startIndex[1] - endIndex[1]);
		regionSize[2] = abs(startIndex[2] - endIndex[2]);

		m_CropRegion.SetSize(regionSize);
		m_CropRegion.SetIndex(startIndex);
		m_CropRegion.Crop(image->GetLargestPossibleRegion());

		// extract cropped image
		typedef itk::ExtractImageFilter< TImageType, TImageType > ExtractFilterType;
		ExtractFilterType::Pointer extract = ExtractFilterType::New();
		extract->SetExtractionRegion(m_CropRegion);
		extract->SetInput(image);
		extract->SetDirectionCollapseToIdentity();

		// update filter
		try
		{
			extract->Update();
		}
		catch (itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}

		return extract->GetOutput();
	};

	void ExtractROIPoints();	// used with reading in ROI from *.ascv file

	typename ImageType::Pointer CropImage(typename ImageType::Pointer image, std::vector<float> roi);

	// applying transform
	void HardenTransform();
	template< typename TImageType >
	typename TImageType::Pointer ResampleImage(typename TImageType::Pointer image)
	{
		// set up resampling object
		typedef itk::ResampleImageFilter< TImageType, TImageType >	ResampleFilterType;
		ResampleFilterType::Pointer resample = ResampleFilterType::New();

		// define image resampling with respect to fixed image
		resample->SetSize(this->m_FixedImage->GetLargestPossibleRegion().GetSize());
		resample->SetOutputOrigin(this->m_FixedImage->GetOrigin());
		resample->SetOutputSpacing(this->m_FixedImage->GetSpacing());
		resample->SetOutputDirection(this->m_FixedImage->GetDirection());

		// input parameters
		resample->SetInput(image);
		if (m_CompositeTransform->IsTransformQueueEmpty())
		{
			resample->SetTransform(m_InitialTransform);
		}
		else
		{
			resample->SetTransform(m_CompositeTransform);
		}

		// define interpolator
		ResampleFilterType::InterpolatorType * linInterpolator = resample->GetInterpolator();
		typedef itk::NearestNeighborInterpolateImageFunction< TImageType, double > NearestNeighborType;
		NearestNeighborType::Pointer nnInterpolator = NearestNeighborType::New();

		if (this->m_NearestNeighbor)
		{
			resample->SetInterpolator(nnInterpolator);
			std::cout << "Nearest neighbor interpolator." << std::endl;
		}
		else
		{
			resample->SetInterpolator(linInterpolator);
		}

		// apply
		resample->Update();

		return resample->GetOutput();
	};
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkManageTransformsFilter.hxx"
#endif

#endif
