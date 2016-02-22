#ifndef __itkValidationFilter_hxx
#define __itkValidationFilter_hxx_hxx

#include "itkValidationFilter.h"

namespace itk
{
	// constructor
	ValidationFilter::ValidationFilter()
	{
	}

	// perform desired measures
	void ValidationFilter::Update()
	{
		if( this->m_computeLabelMapOverlapMeasures )
		{
			ComputeLabelOverlapMeasures();
		}

		return;
	}

	// set image and label map values 
	void ValidationFilter::SetImageAndLabelMap1( ImageType::Pointer image, LabelMapType::Pointer label )
	{
		this->m_labelMap1 = label;
		this->m_image1 = image;
		return;
	}

	// set image and label map values 
	void ValidationFilter::SetImageAndLabelMap2( ImageType::Pointer image, LabelMapType::Pointer label )
	{
		this->m_labelMap2 = label;
		this->m_image2 = image;
		return;
	}

	// calculate overlap measures by dividing up the labels in each image first
	void ValidationFilter::ComputeLabelOverlapMeasures()
	{
		// allocate images
		this->m_labelMap1 = LabelMapType::New();
		this->m_labelMap2 = LabelMapType::New();

		// create temp images
		LabelMapType::Pointer source = LabelMapType::New();
		LabelMapType::Pointer target = LabelMapType::New();

		// find range of values in images
		typedef itk::MinimumMaximumImageCalculator< LabelMapType >	MinMaxCalculatorType;
		MinMaxCalculatorType::Pointer calculator = MinMaxCalculatorType::New();
		calculator->SetImage( this->m_labelMap1 );
		calculator->ComputeMaximum();
		int sMax = calculator->GetMaximum();

		// repeat for target image
		calculator->SetImage( this->m_labelMap2 );
		calculator->ComputeMaximum();
		int tMax = calculator->GetMaximum();

		std::cout << "Source max: " << sMax << std::endl;
		std::cout << "Target max: " << tMax << std::endl;

		// get number of labels in labelMaps
		int numberOfSourceLabels = GetStatistics( this->m_image1, this->m_labelMap1 );
		int numberOfTargetLabels = GetStatistics( this->m_image2, this->m_labelMap2 );

		// check if the label maps agree
		if( numberOfSourceLabels != numberOfTargetLabels || sMax != tMax )
		{
			std::cout << "Err: LabelMap images do not agree" << std::endl;
			return;
		}
		
		// find overlap measures if there is more than one label
		if( numberOfSourceLabels > 0 )
		{
			for( int i = 1; i < sMax; ++i )
			{
				source = IsolateLabel( this->m_labelMap1, i );
				target = IsolateLabel( this->m_labelMap2, i );
				LabelOverlapMeasuresByLabel( source, target, i );
			}
		}
		// or if there is not
		else
		{
			LabelOverlapMeasuresByLabel( this->m_labelMap1, this->m_labelMap2, sMax );
		}

		return;
	}

	// calculate overlap measures according to the label in the image
	void ValidationFilter::LabelOverlapMeasuresByLabel( LabelMapType::Pointer source, LabelMapType::Pointer target, int label )
	{
		// declare and input images
		typedef itk::LabelOverlapMeasuresImageFilter< LabelMapType >	OverlapFilterType;
		OverlapFilterType::Pointer overlapFilter = OverlapFilterType::New();
		overlapFilter->SetSourceImage( source );
		overlapFilter->SetTargetImage( target );

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
		distanceFilter->SetInput1( source );
		distanceFilter->SetInput2( target );
		
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

	int ValidationFilter::GetStatistics( ImageType::Pointer image, LabelMapType::Pointer label )
	{
		// convert label map to image type
		typedef itk::CastImageFilter< LabelMapType, ImageType >	ConvertLabelMapFilterType;
		ConvertLabelMapFilterType::Pointer convert = ConvertLabelMapFilterType::New();
		convert->SetInput( label );
		convert->Update();

		// set up statistics filter
		typedef itk::LabelStatisticsImageFilter< ImageType, ImageType > StatisticsFilterType;
		StatisticsFilterType::Pointer statistics = StatisticsFilterType::New();
		statistics->SetLabelInput( convert->GetOutput() );
		statistics->SetInput( image );
		statistics->Update();

		// print out basic statistics to output
		typedef StatisticsFilterType::ValidLabelValuesContainerType ValidLabelValuesType;
	    typedef StatisticsFilterType::LabelPixelType                LabelPixelType;

		for( ValidLabelValuesType::const_iterator vIt = statistics->GetValidLabelValues().begin(); vIt != statistics->GetValidLabelValues().end(); ++vIt )
		{
			if( statistics->HasLabel( *vIt ) )
			{
				LabelPixelType value = *vIt;
				std::cout << "Label value: " << value << std::endl;
				std::cout << "  Mean: " << statistics->GetMean( value ) << std::endl;
				std::cout << "  St. Dev: " << statistics->GetSigma( value ) << std::endl;
				std::cout << "  Count: " << statistics->GetCount( value ) << std::endl;
			}
		}

		return statistics->GetNumberOfLabels();
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