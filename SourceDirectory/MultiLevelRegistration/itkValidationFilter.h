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

Remaining to implement:
1. fiducial comparison
2. checkerboard image creation
3. overlap measures

*/

#ifndef __itkValidationFilter_h
#define __itkValidationFilter_h

// include files
#include "itkLabelOverlapMeasuresImageFilter.h"
#include "itkCheckerBoardImageFilter.h"
#include "itkHausdorffDistanceImageFilter.h"

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
	void LabelOverlapMeasures( LabelMapType::Pointer source, LabelMapType::Pointer target );
	ImageType::Pointer CheckerboardImage( ImageType::Pointer fixedImage, ImageType::Pointer movingImage );
	void FiducialComparison( char * fixedFilename, char * movingFilename );
	
protected:
	// constructor
	ValidationFilter();
	
	// destructor
	virtual ~ValidationFilter() {}

	// this method is required to allocation memory for the output
	void GenerateData();
	
private:
	// declare variables

	// fiducial alignment

	// overlap measures

	// checkerboard images
	
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkValidationFilter.hxx"
#endif

#endif
