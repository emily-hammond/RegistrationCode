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
	typedef itk::Image< unsigned int, 3 >	LabelMapType;
	typedef itk::Image< unsigned short, 3 >	ImageType;
	
	// method for creation
	itkNewMacro(Self);

	// run-time type information and related methods
	itkTypeMacro(ValidationFilter, Object);

	// declare functions
	void Update();
	void LabelOverlapMeasuresOn() { this->m_computeLabelMapOverlapMeasures = true; };
	ImageType::Pointer CheckerboardImage( ImageType::Pointer fixedImage, ImageType::Pointer movingImage );
	void FiducialComparison( char * fixedFilename, char * movingFilename );
	void SetImageAndLabelMap1( ImageType::Pointer image, LabelMapType::Pointer label );
	void SetImageAndLabelMap2( ImageType::Pointer image, LabelMapType::Pointer label );
	
protected:
	// constructor
	ValidationFilter();
	
	// destructor
	virtual ~ValidationFilter() {}

	// this method is required to allocation memory for the output
	void GenerateData();
	
private:
	// declare variables
	ImageType::Pointer m_image1;
	LabelMapType::Pointer m_labelMap1;
	ImageType::Pointer m_image2;
	LabelMapType::Pointer m_labelMap2;

	// fiducial alignment

	// overlap measures
	void ComputeLabelOverlapMeasures();
	bool m_computeLabelMapOverlapMeasures;
	void LabelOverlapMeasuresByLabel( LabelMapType::Pointer source, LabelMapType::Pointer target, int label );
	LabelMapType::Pointer IsolateLabel( LabelMapType::Pointer image, int label );
	int GetStatistics( ImageType::Pointer image, LabelMapType::Pointer label );
	
	// checkerboard images
	
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkValidationFilter.hxx"
#endif

#endif
