#include "itkDiscreteGaussianImageFilter.h"
#include "itkN4BiasFieldCorrectionImageFilter.h"
#include "itkThresholdImageFilter.h"

// templated function to threshold an image with an upper value
template< typename ImageType > 
typename ImageType::Pointer UpperThresholdImage( typename ImageType::Pointer image, int upperThreshold)
{
	typedef itk::ThresholdImageFilter< ImageType > 		ThresholdImageFilterType;
	typename ThresholdImageFilterType::Pointer threshold = ThresholdImageFilterType::New();
	threshold->SetInput( image );
	threshold->ThresholdAbove( upperThreshold );
	threshold->SetOutsideValue( upperThreshold );
	try
	{
		threshold->Update();
	}
	catch (itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}
	
	return threshold->GetOutput();
}

// write a function to threshold an image with a lower value
template< typename ImageType >
typename ImageType::Pointer PreprocessingFilter::LowerThresholdImage( typename ImageType::Pointer image, int lowerThreshold)
{
	typedef itk::ThresholdImageFilter< ImageType > 		ThresholdImageFilterType;
	typename ThresholdImageFilterType::Pointer threshold = ThresholdImageFilterType::New();
	threshold->SetInput( image );
	threshold->ThresholdBelow( lowerThreshold );
	threshold->SetOutsideValue( lowerThreshold );
	try
	{
		threshold->Update();
	}
	catch (itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}
	
	return threshold->GetOutput();
}

// write a function to smooth an image with a gaussian image filter with specified sigma
template< typename ImageType >
typename ImageType::Pointer PreprocessingFilter::SmoothImage( typename ImageType::Pointer image, int sigma )
{
	typedef itk::DiscreteGaussianImageFilter< ImageType, ImageType >		SmoothingImageFilterType;
	typename SmoothingImageFilterType::Pointer smooth = SmoothingImageFilterType::New();
	smooth->SetInput( image );
	smooth->SetVariance( sigma );
	
	try
	{
		smooth->Update();
	}
	catch (itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}
	
	return smooth->GetOutput();
}
