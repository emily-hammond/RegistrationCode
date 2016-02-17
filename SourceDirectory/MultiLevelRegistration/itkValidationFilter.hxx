#ifndef __itkValidationFilter_hxx
#define __itkValidationFilter_hxx_hxx

#include "itkValidationFilter.h"

namespace itk
{
	// constructor
	ValidationFilter::ValidationFilter()
	{
	}

	// calculate overlap measures by dividing up the labels in each image first
	void ValidationFilter::LabelOverlapMeasures( LabelMapType::Pointer source, LabelMapType::Pointer target )
	{
		// allocate images
		this->m_source = LabelMapType::New();
		this->m_target = LabelMapType::New();

		// find range of values in images
		typedef itk::MinimumMaximumImageCalculator< LabelMapType >	MinMaxCalculatorType;
		MinMaxCalculatorType::Pointer mms = MinMaxCalculatorType::New();
		mms->SetImage( source );
		mms->Compute();

		// repeat for target image
		MinMaxCalculatorType::Pointer mmt = MinMaxCalculatorType::New();
		mmt->SetImage( target );
		mmt->Compute();

		// check range of values for both images
		int sMin = mms->GetMinimum();
		int sMax = mms->GetMaximum();
		int tMin = mms->GetMinimum();
		int tMax = mms->GetMaximum();

		std::cout << "Source range: " << sMax - sMin << std::endl;
		std::cout << "Target range: " << tMax - tMin << std::endl;

		// check if the label maps agree
		if( sMax != tMax || sMin != tMin )
		{
			std::cout << "Err: LabelMap images do not agree" << std::endl;
			return;
		}

		// find overlap measures if there is more than one label
		if( sMax - sMin > 0 )
		{
			for( int i = sMin; i < sMax; ++i )
			{
				this->m_source = IsolateLabel( source, i );
				this->m_target = IsolateLabel( target, i );
				LabelOverlapMeasuresByLabel( i );
			}
		}
		// or if there is not
		else
		{
			this->m_source = source;
			this->m_target = target;
		}

		return;
	}

	// calculate overlap measures according to the label in the image
	void ValidationFilter::LabelOverlapMeasuresByLabel( int label )
	{
		// declare and input images
		typedef itk::LabelOverlapMeasuresImageFilter< LabelMapType >	OverlapFilterType;
		OverlapFilterType::Pointer overlapFilter = OverlapFilterType::New();
		overlapFilter->SetSourceImage( this->m_source );
		overlapFilter->SetTargetImage( this->m_target );

		// update filter
		try
		{
			overlapFilter->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}

		// calculate Hausdorff distances
		typedef itk::HausdorffDistanceImageFilter< LabelMapType, LabelMapType >	DistanceFilterType;
		DistanceFilterType::Pointer distanceFilter = DistanceFilterType::New();
		distanceFilter->SetInput1( this->m_source );
		distanceFilter->SetInput2( this->m_target );
		
		// update filter
		try
		{
			distanceFilter->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}

		// write out results to screen
		std::cout << "Overlap Measures for label: " << label << std::endl;
		std::cout << "Total overlap: " << overlapFilter->GetTotalOverlap();
		std::cout << "Union(Jaccard) overlap: " << overlapFilter->GetUnionOverlap();
		std::cout << "Mean(Dice) overlap: " << overlapFilter->GetMeanOverlap();
		std::cout << "Hausdorff distance: " << distanceFilter->GetHausdorffDistance();
		std::cout << "Average HD: " << distanceFilter->GetAverageHausdorffDistance();
		std::cout << std::endl;

		return;
	}

	ValidationFilter::LabelMapType::Pointer ValidationFilter::IsolateLabel( LabelMapType::Pointer image, int label )
	{
		// set up thresholding
		typedef itk::BinaryThresholdImageFilter< LabelMapType, LabelMapType >	ThresholdType;
		ThresholdType::Pointer threshold = ThresholdType::New();
		threshold->SetInput( image );
		threshold->SetLowerThreshold( label );
		threshold->SetUpperThreshold( label );
		threshold->SetInsideValue( 1 );
		threshold->SetOutsideValue( 0 );

		// update filter
		try
		{
			threshold->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}

		// get isolated label
		return threshold->GetOutput();
	}

	ValidationFilter::ImageType::Pointer ValidationFilter::CheckerboardImage( ImageType::Pointer fixedImage, ImageType::Pointer movingImage )
	{
		//declare and insert inputs
		typedef itk::CheckerBoardImageFilter< ImageType >	CheckerboardFilterType;
		CheckerboardFilterType::Pointer checker = CheckerboardFilterType::New();
		checker->SetInput1( movingImage );
		checker->SetInput2( fixedImage );

		// define pattern type
		CheckerboardFilterType::PatternArrayType pattern;
		pattern[0] = 4;
		pattern[1] = 4;
		pattern[2] = 4;
		checker->SetCheckerPattern( pattern );

		return checker->GetOutput();
	}

} // end namespace

#endif