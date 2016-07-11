/*
Author: Emily Hammond
Date: 2016 February 

Purpose: This class performs registration validation with three different types:
1. Fiducial alignment given two corresponding landmark lists
2. Overlap measures given two corresponding mask files
	a. target overlap
	b. jaccard overlap
	c. dice overlap
	d. hausdorff distance
	e. average hausdorff distance
3. Checkerboard image creation given two registered images

NOTE: This filter requires that the final transform for alignment has already been applied

Remaining to implement:
1. fiducial comparison
2. checkerboard image thresholding

*/

#ifndef __itkValidationFilter_h
#define __itkValidationFilter_h

// include files
#include "itkLabelOverlapMeasuresImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkHausdorffDistanceImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkCastImageFilter.h"

#include "itkCheckerBoardImageFilter.h"

namespace itk
{
// class Validation
class ValidationFilter: public Object
{
public:
	// default ITK
	typedef ValidationFilter		Self;
	typedef Object						Superclass;
	typedef SmartPointer< Self >		Pointer;
	typedef SmartPointer< const Self >	ConstPointer;

	// definitions
	typedef itk::Image< short, 3 >	ImageType;
	
	// method for creation
	itkNewMacro(Self);

	// run-time type information and related methods
	itkTypeMacro(ValidationFilter, Object);

	// declare functions
	void Update();

	// fiducial comparison
	void SetFixedFiducialFilename( char * filename )
	{
		m_FixedFiducialFilename = filename;
	}
	void SetMovingFiducialFilename( char * filename )
	{
		m_MovingFiducialFilename = filename;
	}
	void FiducialAlignmentOn()
	{
		m_FiducialAlignment = true;
	}
	void FiducialAlignmentOff()
	{
		m_FiducialAlignment = false;
	}

	// set flags for computing label overlap measures
	itkSetObjectMacro( Image1, ImageType );
	itkSetObjectMacro( LabelMap1, ImageType );
	itkSetObjectMacro( Image2, ImageType );
	itkSetObjectMacro( LabelMap2, ImageType );
	void LabelOverlapMeasuresOn()
	{ 
		m_LabelMapOverlapMeasures = true; 
	}
	void LabelOverlapMeasuresOff()
	{
		m_LabelMapOverlapMeasures = false;
	}

	// checkerboard images
	itkGetObjectMacro( CBImage, ImageType );
	void CheckerboardImageOn()
	{
		m_CheckerboardImage = true;
	}
	void CheckerboardImageOff()
	{
		m_CheckerboardImage = false;
	}

protected:
	// constructor
	ValidationFilter();
	
	// destructor
	virtual ~ValidationFilter() {}

	// this method is required to allocation memory for the output
	void GenerateData();
	
private:
	// declare variables
	ImageType::Pointer m_Image1;
	ImageType::Pointer m_LabelMap1;
	ImageType::Pointer m_Image2;
	ImageType::Pointer m_LabelMap2;
	char * m_FixedFiducialFilename;
	char * m_MovingFiducialFilename;

	// fiducial alignment
	bool m_FiducialAlignment;
	void FiducialAlignment();

	// overlap measures
	bool m_LabelMapOverlapMeasures;
	void LabelOverlapMeasures();
	void LabelOverlapMeasuresByLabel( ImageType::Pointer source, ImageType::Pointer target, int label );
	ImageType::Pointer IsolateLabel( ImageType::Pointer image, int label );
	int GetStatistics( ImageType::Pointer image, ImageType::Pointer label );
	
	// checkerboard images
	bool m_CheckerboardImage;
	ImageType::Pointer m_CBImage;
	void CheckerboardImage();	
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkValidationFilter.hxx"
#endif

#endif
