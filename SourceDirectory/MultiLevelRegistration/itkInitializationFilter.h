/*
insert comments here
*/

#ifndef __itkInitializationFilter_h
#define __itkInitializationFilter_h

// include files

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
	
	// method for creation
	itkNewMacro(Self);

	// run-time type information and related methods
	itkTypeMacro(InitializationFilter, Object);

	// declare functions

protected:
	// declare the constructor and define default parameters
	InitializationFilter();
	
	// destructor
	virtual ~InitializationFilter() {}

	// this method is required to allocation memory for the output
	void GenerateData();
	
private:
	// declare variables
	
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkInitializationFilter.hxx"
#endif

#endif
