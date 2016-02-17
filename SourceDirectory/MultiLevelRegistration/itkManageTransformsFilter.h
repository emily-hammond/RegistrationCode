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

#ifndef __itkManageTransformsFilter_h
#define __itkManageTransformsFilter_h

// include files

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
	
	// method for creation
	itkNewMacro(Self);

	// run-time type information and related methods
	itkTypeMacro(ManageTransformsFilter, Object);

	// declare functions

	
protected:
	// constructor
	ManageTransformsFilter();
	
	// destructor
	virtual ~ManageTransformsFilter() {}

	// this method is required to allocation memory for the output
	void GenerateData();
	
private:
	// declare variables
	
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkManageTransformsFilter.hxx"
#endif

#endif
