/*
Author: Emily Hammond
Date: 2016 February 

Purpose: 

*/

#ifndef __itkValidation_h
#define __itkValidation_h

// include files

namespace itk
{
// class Validation
class Validation: public Object
{
public:
	// default ITK
	typedef Validation		Self;
	typedef Object						Superclass;
	typedef SmartPointer< Self >		Pointer;
	typedef SmartPointer< const Self >	ConstPointer;

	// definitions
	
	
	// method for creation
	itkNewMacro(Self);

	// run-time type information and related methods
	itkTypeMacro(InitializationFilter, Object);

	// declare functions
	
protected:
	// constructor
	InitializationFilter();
	
	// destructor
	virtual ~Validation() {}

	// this method is required to allocation memory for the output
	void GenerateData();
	
private:
	// declare variables
	
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkValidation.hxx"
#endif

#endif
