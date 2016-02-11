/*
insert comments here
*/

#ifndef __itkRegistrationFramework_h
#define __itkRegistrationFramework_h

// include files
#include "itkImage.h"

namespace itk
{
/* class RegistrationFramework
 *
 * 
 
*/
class RegistrationFramework: public Object
{
public:
	typedef RegistrationFramework		Self;
	typedef Object						Superclass;
	typedef SmartPointer< Self >		Pointer;
	typedef SmartPointer< const Self >	ConstPointer;
	
	typedef itk::Image< unsigned short, 3 >		ImageType;

	// method for creation
	itkNewMacro(Self);

	// run-time type information and related methods
	itkTypeMacro(RegistrationFramework, Object);

	// declare functions
	void SetImages( ImageType::Pointer fixedImage, ImageType::Pointer movingImage );

protected:
	// declare the constructor and define default parameters
	RegistrationFramework() {}
	
	// destructor
	virtual ~RegistrationFramework() {}

	// this method is required to allocation memory for the output
	void GenerateData();
	
private:
	// declare variables
	ImageType::Pointer m_fixedImage;
	ImageType::Pointer m_movingImage;
	
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRegistrationFramework.hxx"
#endif

#endif
