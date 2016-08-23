#ifndef __itkPreprocessingFilter_hxx
#define __itkPreprocessingFilter_hxx

#include "itkPreprocessingFilter.h"

namespace itk
{
	// constructor
	PreprocessingFilter::PreprocessingFilter()
	{}
	
	// implement member functions
	ImageType::Pointer PreprocessingFilter::UpperThresholdImage( ImageType::Pointer image, int upperThreshold)
	{
		typedef itk::ThresholdImageFilter< ImageType > 		ThresholdImageFilterType;
		ThresholdImageFilterType::Pointer threshold = ThresholdImageFilterType::New();
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
	
	ImageType::Pointer PreprocessingFilter::LowerThresholdImage( ImageType::Pointer image, int lowerThreshold)
	{
		typedef itk::ThresholdImageFilter< ImageType > 		ThresholdImageFilterType;
		ThresholdImageFilterType::Pointer threshold = ThresholdImageFilterType::New();
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
	
	ImageType::Pointer PreProcessingFilter::SmoothImage( ImageType::Pointer image, int sigma )
	{
		typedef itk::DiscreteGaussianImageFilter< ImageType, ImageType >		SmoothingImageFilterType;
		SmoothingImageFilterType::Pointer smooth = SmoothingImageFilterType::New();
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
	
		
} // end namespace

#endif