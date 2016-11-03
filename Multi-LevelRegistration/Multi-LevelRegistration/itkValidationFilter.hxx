#ifndef __itkValidationFilter_hxx
#define __itkValidationFilter_hxx_hxx

#include "itkValidationFilter.h"

namespace itk
{
	// constructor
	template< typename TPixelType >
	ValidationFilter< TPixelType >::ValidationFilter():
		m_LabelMap1( ITK_NULLPTR ),	// defined by user
		m_LabelMap2( ITK_NULLPTR ),	// defined by user
		m_Image1( ITK_NULLPTR ),	// defined by user
		m_Image2( ITK_NULLPTR ),	// defined by user
		m_FixedFiducialFilename( ITK_NULLPTR ),	// defined by user
		m_MovingFiducialFilename( ITK_NULLPTR ),	// defined by user
		m_FiducialAlignment( false ),
		m_LabelMapOverlapMeasures( false ),
		m_CheckerboardImage( false )
		{}

	// perform desired measures
	template< typename TPixelType >
	void ValidationFilter< TPixelType >::Update()
	{
		// compute overlap measures
		if( this->m_LabelMapOverlapMeasures )
		{
			if( !m_LabelMap1 )
			{
				std::cout << "Label map 1 not present" << std::endl;
			}
			if( !m_LabelMap2 )
			{
				std::cout << "Label map 2 not present" << std::endl;
			}
			if( !m_Image1 )
			{
				std::cout << "Image 1 not present" << std::endl;
			}
			if( !m_Image2 )
			{
				std::cout << "Image 2 not present" << std::endl;
			}
			std::cout << "Computing overlap measures." << std::endl;
			LabelOverlapMeasures();
		}

		// compute fiducial alignment
		if( this->m_FiducialAlignment )
		{
			if( !m_FixedFiducialFilename )
			{
				std::cout << "Fixed fiducials not present" << std::endl;
			}
			if( !m_MovingFiducialFilename )
			{
				std::cout << "Moving fiducials not present" << std::endl;
			}
			std::cout << "Computing fiducial alignment." << std::endl;
			FiducialAlignment();
		}

		// compute checkerboard image
		if( this->m_CheckerboardImage )
		{
			if( !m_Image1 )
			{
				std::cout << "Image 1 not present" << std::endl;
			}
			if( !m_Image2 )
			{
				std::cout << "Image 2 not present" << std::endl;
			}
			std::cout << "Computing checkerboard image." << std::endl;
			CheckerboardImage();
		}

		return;
	}

	// calculate overlap measures by dividing up the labels in each image first
	template< typename TPixelType >
	void ValidationFilter< TPixelType >::LabelOverlapMeasures()
	{
		// create temp images
		MaskImageType::Pointer source = MaskImageType::New();
		MaskImageType::Pointer target = MaskImageType::New();

		// find range of values in images
		typedef itk::MinimumMaximumImageCalculator< MaskImageType >		MinMaxCalculatorType;
		MinMaxCalculatorType::Pointer calculator = MinMaxCalculatorType::New();
		calculator->SetImage( this->m_LabelMap1 );
		calculator->Compute();
		int sMax = calculator->GetMaximum();

		// repeat for target image
		calculator->SetImage( this->m_LabelMap2 );
		calculator->Compute();
		int tMax = calculator->GetMaximum();

		// get number of labels in labelMaps
		std::cout << "\nImage #1: " << std::endl;
		int numberOfSourceLabels = GetStatistics( this->m_Image1, this->m_LabelMap1 );
		std::cout << "\nImage #2: " << std::endl;
		int numberOfTargetLabels = GetStatistics( this->m_Image2, this->m_LabelMap2 );

		// check if the label maps agree
		if( numberOfSourceLabels != numberOfTargetLabels || sMax != tMax )
		{
			std::cout << "Err: LabelMap images do not agree" << std::endl;
			return;
		}
		
		// find overlap measures if there is more than one label
		if( numberOfSourceLabels-1 > 1 )
		{
			for( int i = 1; i < sMax; ++i )
			{
				source = IsolateLabel( this->m_LabelMap1, i );
				target = IsolateLabel( this->m_LabelMap2, i );
				LabelOverlapMeasuresByLabel( source, target, i );
			}
		}
		// or if there is not
		else
		{
			LabelOverlapMeasuresByLabel( this->m_LabelMap1, this->m_LabelMap2, sMax );
		}

		return;
	}

	// calculate overlap measures according to the label in the image
	template< typename TPixelType >
	void ValidationFilter< TPixelType >::LabelOverlapMeasuresByLabel( MaskImageType::Pointer sourceLabel, MaskImageType::Pointer targetLabel, int label)
	{
		// declare and input images
		typedef itk::LabelOverlapMeasuresImageFilter< MaskImageType >	OverlapFilterType;
		OverlapFilterType::Pointer overlapFilter = OverlapFilterType::New();
		overlapFilter->SetSourceImage(sourceLabel);
		overlapFilter->SetTargetImage(targetLabel);

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
		typedef itk::HausdorffDistanceImageFilter< MaskImageType, MaskImageType >	DistanceFilterType;
		DistanceFilterType::Pointer distanceFilter = DistanceFilterType::New();
		distanceFilter->SetInput1(sourceLabel);
		distanceFilter->SetInput2(targetLabel);
		
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
		std::cout << "\nOverlap Measures for label: " << label << std::endl;
		std::cout << "  Total overlap         : " << overlapFilter->GetTotalOverlap() << std::endl;
		std::cout << "  Union(Jaccard) overlap: " << overlapFilter->GetUnionOverlap() << std::endl;
		std::cout << "  Mean(Dice) overlap    : " << overlapFilter->GetMeanOverlap() << std::endl;
		std::cout << "  Hausdorff distance    : " << distanceFilter->GetHausdorffDistance() << std::endl;
		std::cout << "  Average HD            : " << distanceFilter->GetAverageHausdorffDistance() << std::endl;
		std::cout << std::endl;

		return;
	}

	template< typename TPixelType >
	typename ValidationFilter< TPixelType >::MaskImageType::Pointer ValidationFilter< TPixelType >::IsolateLabel(MaskImageType::Pointer labelMap, int label)
	{
		// set up thresholding
		typedef itk::BinaryThresholdImageFilter< MaskImageType, MaskImageType >	ThresholdType;
		ThresholdType::Pointer threshold = ThresholdType::New();
		threshold->SetInput( labelMap );
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

		std::cout << "Label " << label << " isolated." << std::endl;

		// get isolated label
		return threshold->GetOutput();
	}

	template< typename TPixelType >
	int ValidationFilter< TPixelType >::GetStatistics(typename ImageType::Pointer image, MaskImageType::Pointer label)
	{
		// convert label map to image type
		typedef itk::CastImageFilter< MaskImageType, MaskImageType >	ConvertLabelMapFilterType;
		ConvertLabelMapFilterType::Pointer convert = ConvertLabelMapFilterType::New();
		convert->SetInput( label );
		convert->Update();

		// set up statistics filter
		typedef itk::LabelStatisticsImageFilter< ImageType, MaskImageType > StatisticsFilterType;
		StatisticsFilterType::Pointer statistics = StatisticsFilterType::New();
		statistics->SetLabelInput( convert->GetOutput() );
		statistics->SetInput( image );
		statistics->Update();

		std::cout << "Statistics calculated." << std::endl;

		// print out basic statistics to output
		typedef StatisticsFilterType::ValidLabelValuesContainerType ValidLabelValuesType;
	    typedef StatisticsFilterType::LabelPixelType                LabelPixelType;

		std::cout << "Number of labels: " << statistics->GetNumberOfLabels() << std::endl;

		for( ValidLabelValuesType::const_iterator vIt = statistics->GetValidLabelValues().begin(); vIt != statistics->GetValidLabelValues().end(); ++vIt )
		{
			if( statistics->HasLabel( *vIt ) )
			{
				LabelPixelType value = *vIt;
				if( value > 0 )
				{
					std::cout << "Label value: " << value << std::endl;
					std::cout << "  Mean   : " << statistics->GetMean( value ) << std::endl;
					std::cout << "  St. Dev: " << statistics->GetSigma( value ) << std::endl;
					std::cout << "  Count  : " << statistics->GetCount( value ) << std::endl;
				}
			}
		}

		return statistics->GetNumberOfLabels();
	}

	template< typename TPixelType >
	void ValidationFilter< TPixelType >::CheckerboardImage()
	{
		//declare and insert inputs
		typedef itk::CheckerBoardImageFilter< ImageType >	CheckerboardFilterType;
		CheckerboardFilterType::Pointer checker = CheckerboardFilterType::New();
		checker->SetInput1( this->m_Image1 );
		checker->SetInput2( this->m_Image2 );

		// define pattern type
		CheckerboardFilterType::PatternArrayType pattern;
		pattern[0] = 4;
		pattern[1] = 4;
		pattern[2] = 4;
		checker->SetCheckerPattern( pattern );

		std::cout << "Checkerboard image created." << std::endl;
		return;
	}

	template< typename TPixelType >
	void ValidationFilter< TPixelType >::FiducialAlignment()
	{
		std::cout << "Fiducial alignment complete." << std::endl;
		return;
	}
} // end namespace

#endif