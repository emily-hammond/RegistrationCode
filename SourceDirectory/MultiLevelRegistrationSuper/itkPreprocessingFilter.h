/*
Author: Emily Hammond
Date: 2016 August 23

Purpose: This class is to implement different initialization techniques to the 
moving/fixed images prior to multi-level registration. These include thresholding,
bias correction, and gaussian smoothing

*/

#ifndef_itkProprocessingFilter_h
#define_itkPreprocessingFilter_h

// include files not defined elsewhere
#include "itkDiscreteGaussianImageFilter.h"
#include "itkN4BiasFieldCorrectionImageFilter.h"
#include "itkThresholdImageFilter.h"

namespace itk
{
// class ProprocessingFilter
class PreprocessingFilter: public Object
{
public:
	// default ITK
	typedef PreprocessingFilter			Self;
	typedef Object						Superclass;
	typedef SmartPointer<Self>			Pointer;
	typedef SmartPointer<const Self>	ConstPointer;
	
	// definitions
	typedef itk::Image< short, 3 >		ImageType;
	
	// method for creation
	itkNewMacro(Self)
	
	// run-time type information and related methods
	itkTypeMacro(PreprocessingFilter, Object)
	
	// member functions
	ImageType::Pointer UpperThresholdImage( ImageType::Pointer image, int upperThreshold);
	ImageType::Pointer LowerThresholdImage( ImageType::Pointer image, int upperThreshold);
	ImageType::Pointer BiasCorrection( ImageType::Pointer image );
	ImageType::Pointer SmoothImage( ImageType::Pointer image, int sigma );

protected:
	// constructor
	PreprocessingFilter();
	
	// destructor
	virtual ~PreprocessingFilter() {}
	
	// this method is required to allocate memory for the output
	void GenerateData();
	
private:
	// private member functions
	
};
} // end namespace
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPreprocessingFilter.hxx"
#endif

#endif