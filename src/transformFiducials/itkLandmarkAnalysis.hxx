/*
Emily Hammond
2015/01/23

This is the class definition for LandmarkAnalysis

*/

#ifndef __itkLandmarkAnalysis_hxx
#define __itkLandmarkAnalysis_hxx

#include "itkLandmarkAnalysis.h"

namespace itk
{
	void LandmarkAnalysis::SetFixedLandmarksFilename( const char * fixedLandmarksFilename )
	{
		this->m_FixedLandmarks = ReadLandmarksFile( fixedLandmarksFilename );
		return;
	}
	
	void LandmarkAnalysis::SetMovingLandmarksFilename( const char * movingLandmarksFilename )
	{
		this->m_MovingLandmarks = ReadLandmarksFile( movingLandmarksFilename );
		this->m_TransformedMovingLandmarks = this->m_MovingLandmarks;

		return;
	}

	itk::LandmarkAnalysis::LandmarksType LandmarkAnalysis::ReadLandmarksFile( const char * filename )
	{
		// open file and 
		std::ifstream myfile( filename );
		LandmarksType landmarks;

		// error checking
		if( !myfile.is_open() )
		{
			std::cerr << "Error: Failed to open landmarks file! " << std::endl;
			return landmarks;
		}

		// read in the lines from file
		std::string line;
		int count = 0;
		while( getline( myfile, line ) )
		{
			if( line.compare( 0, 1, "#" ) != 0 ) // skip lines starting with #
			{
				// determine where the first coordinate lives
				size_t pos1 = line.find( ',',0 );
				PointType	pointPos;

				// grab the coordinates from the string
				for( unsigned int i = 0; i<3; ++i )
				{
					const size_t pos2 = line.find( ',', pos1+1);
					pointPos[i] = atof( line.substr( pos1+1, pos2-pos1-1 ).c_str() );
					if( i > 2 ) // negate first two components for RAS->LPS
					{
						pointPos[i] *= -1;
					}
					pos1 = pos2;
				}

				// walk through the string until the "label" is reached
				for( unsigned int i = 3; i < 10; ++i )
				{
					const size_t pos2 = line.find( ',', pos1+1 );
					pos1 = pos2;
				}

				// read in label and place the label with point in landmarks map
				const size_t pos2 = line.find( ',', pos1+1 );
				std::string label = line.substr( pos1+1, pos2-pos1-1 ).c_str();
				landmarks[label] = pointPos;
			}
		}

		std::cout << filename << " has been read in!" << std::endl;
		return landmarks;
	}

	void LandmarkAnalysis::PrintLandmarks( LandmarksType landmarks ) const
	{
		// iterate through the landmarks map
		LandmarksType::const_iterator it = landmarks.begin();
		for(; it!=landmarks.end(); it++)
		{
			// write out components within the map
			std::cout << it->first << " " << it->second << std::endl;
		}
		return;
	}

	void LandmarkAnalysis::SetTransformFilename( const char * transformFilename )
	{
		this->m_TransformList.merge( ReadTransformFile( transformFilename ) );
		return;
	}

	itk::LandmarkAnalysis::TransformListType LandmarkAnalysis::ReadTransformFile( const char * transformFilename )
	{
		// read in transforms
		TransformReaderType::Pointer reader = TransformReaderType::New();
		reader->SetFileName( transformFilename );

		// update reader
		try
		{
			reader->Update();
		}
		catch( itk::ExceptionObject & err )
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}

		// put output into member variable and place into full list
		TransformListType transformListTemp = *(reader->GetTransformList());

		std::cout << transformFilename << " has been read in!" << std::endl;
		return transformListTemp;
	}

	void LandmarkAnalysis::PrintTransformList() const
	{
		// create iterator and output what is at that location
		TransformListType::const_iterator it = this->m_TransformList.begin();
		for(; it != this->m_TransformList.end(); it++ )
		{
			std::cout << (*it) << std::endl;
		}

		return;
	}

	void LandmarkAnalysis::PerformStatistics()
	{
		std::cout << "Calculating statistics!" << std::endl;
		this->TransformLandmarks();
		this->CalculateSSD();
	}
	
	itk::LandmarkAnalysis::AffineTransformType::Pointer LandmarkAnalysis::ConvertTransform( AffineTransformType::Pointer lpsTransform)
	{
		// Convert from LPS (ITK) to RAS (Slicer)
		// input: transformVtk_LPS matrix in vtkMatrix4x4 in resampling convention in LPS
		// output: transformVtk_RAS matrix in vtkMatrix4x4 in modeling convention in RAS
		AffineTransformType::MatrixType matrix = lpsTransform->GetMatrix();
		AffineTransformType::TranslationType trans = lpsTransform->GetTranslation();

		// initialize transforms (LPS should be taken from the affine transform)
		vtkSmartPointer< vtkMatrix4x4 > lps = vtkSmartPointer< vtkMatrix4x4 >::New();
		lps->Identity();
		lps->SetElement(0, 0, matrix(0,0));
		lps->SetElement(0, 1, matrix(0,1));
		lps->SetElement(0, 2, matrix(0,2));
		lps->SetElement(1, 0, matrix(1,0));
		lps->SetElement(1, 1, matrix(1,1));
		lps->SetElement(1, 2, matrix(1,2));
		lps->SetElement(2, 0, matrix(2,0));
		lps->SetElement(2, 1, matrix(2,1));
		lps->SetElement(2, 2, matrix(2,2));
		lps->SetElement(0, 3, trans[0]);
		lps->SetElement(1, 3, trans[1]);
		lps->SetElement(2, 3, trans[2]);

		// initialize the RAS matrix to identity
		vtkSmartPointer< vtkMatrix4x4 > ras = vtkSmartPointer< vtkMatrix4x4 >::New();
		ras->Identity();

		// Tras = lps2ras * Tlps * ras2lps
		vtkSmartPointer<vtkMatrix4x4> lps2ras = vtkSmartPointer<vtkMatrix4x4>::New();
		lps2ras->SetElement(0,0,-1);
		lps2ras->SetElement(1,1,-1);

		vtkMatrix4x4 * ras2lps = lps2ras; // lps2ras is diagonal therefore the inverse is identical
		vtkMatrix4x4::Multiply4x4(lps2ras, lps, lps);
		vtkMatrix4x4::Multiply4x4(lps, ras2lps, ras);

		// Convert the sense of the transform (from ITK resampling to Slicer modeling transform)
		vtkMatrix4x4::Invert(ras,ras);

		// insert parameters into a new transform
		AffineTransformType::Pointer rasTransform = AffineTransformType::New();
		AffineTransformType::ParametersType parameters( rasTransform->GetNumberOfParameters() );
		parameters[0] = ras->GetElement(0,0);
		parameters[1] = ras->GetElement(0,1);
		parameters[2] = ras->GetElement(0,2);
		parameters[9] = ras->GetElement(0,3);
		parameters[3] = ras->GetElement(1,0);
		parameters[4] = ras->GetElement(1,1);
		parameters[5] = ras->GetElement(1,2);
		parameters[10] = ras->GetElement(1,3);
		parameters[6] = ras->GetElement(2,0);
		parameters[7] = ras->GetElement(2,1);
		parameters[8] = ras->GetElement(2,2);
		parameters[11] = ras->GetElement(2,3);
		// set parameters
		rasTransform->SetParameters( parameters );
		
		// return the new transform
		return rasTransform;
	}

	itk::LandmarkAnalysis::BSplineTransformType::Pointer LandmarkAnalysis::ConvertTransform( BSplineTransformType::Pointer bspline)
	{
		//std::cout << "ORIGINAL BSPLINE TRANSFORM" << std::endl;
		
		BSplineTransformType::ParametersType fixedParams = bspline->GetFixedParameters();
		BSplineTransformType::OriginType origin = bspline->GetTransformDomainOrigin();
		BSplineTransformType::PhysicalDimensionsType dimensions = bspline->GetTransformDomainPhysicalDimensions();

		//std::cout << bspline << std::endl;

		// convert from LPS to RAS
		// set origin and spacing
		fixedParams[3] = -fixedParams[3];
		fixedParams[4] = -fixedParams[4];
		fixedParams[5] = fixedParams[5];
		fixedParams[6] = -fixedParams[6];
		fixedParams[7] = -fixedParams[7];
		fixedParams[8] = fixedParams[8];

		bspline->SetFixedParameters( fixedParams );

		// change the deformation at each location
		
		BSplineTransformType::CoefficientImageArray images = bspline->GetCoefficientImages();
				
		// change the sign of the deformation field for the first two images
		// create image iterator for the largest possible region
		for( int i = 0; i < 2; ++i )
		{
			itk::ImageRegionIterator< BSplineTransformType::ImageType > itImage( images[i], images[i]->GetLargestPossibleRegion() );
			while( !itImage.IsAtEnd() )
			{
				// get value and negate it
				itImage.Set( -itImage.Get() );
				++itImage;
			}
		}

		// set the coefficient images in the bspline transform
		bspline->SetTransformDomainOrigin( origin );
		bspline->SetTransformDomainPhysicalDimensions( dimensions );
		bspline->SetCoefficientImages( images );

		std::cout << "NEW BSPLINE TRANSFORM" << std::endl;
		std::cout << bspline << std::endl;
		

		//return rasTransform;
		return bspline;
	}

	void LandmarkAnalysis::TransformLandmarks()
	{
		// create iterator to parse through the transform list
		TransformListType::iterator itTrans = this->m_TransformList.begin();
		for(; itTrans != this->m_TransformList.end(); itTrans++ )
		{
			//bool disp = true;
			// determine what type of transform it is
			if( !strcmp( (*itTrans)->GetNameOfClass(), "AffineTransform" ) )
			{
				// extract transform
				AffineTransformType::Pointer affineTrans = static_cast< AffineTransformType * >( (*itTrans).GetPointer() );

				// convert to ras transform (from lps)
				affineTrans = ConvertTransform( affineTrans );

				// apply to landmarks
				LandmarksType::const_iterator itLM = this->m_TransformedMovingLandmarks.begin();
				for(; itLM != this->m_TransformedMovingLandmarks.end(); itLM++ )
				{
					//std::cout << "Original   : " << itLM->second << std::endl; 
					
					this->m_TransformedMovingLandmarks[ itLM->first ] = affineTrans->TransformPoint( itLM->second );
					
					//std::cout << "Transformed: " << itLM->second << std::endl;
					//std::cout << std::endl;
				}

				std::cout << "Affine transform has been applied!" << std::endl;
			}
			else if( !strcmp( (*itTrans)->GetNameOfClass(), "BSplineTransform" ) )
			{
				// extract transforms
				/*
				BSplineTransformType::Pointer bsplineTrans = static_cast< BSplineTransformType * >( (*itTrans).GetPointer() );
				std::cout << bsplineTrans->GetInverseTransform() << std::endl;

				// convert transform
				//bsplineTrans = ConvertTransform( bsplineTrans );

				// apply to landmarks
				LandmarksType::const_iterator itLM = this->m_TransformedMovingLandmarks.begin();
				for(; itLM != this->m_TransformedMovingLandmarks.end(); itLM++ )
				{
					std::cout << "Original   : " << itLM->second << std::endl;
					PointType in = itLM->second;
					in[0] = -in[0];
					in[1] = -in[1];

					PointType out = bsplineTrans->TransformPoint( in );

					out[0] = -out[0];
					out[1] = -out[1];

					this->m_TransformedMovingLandmarks[ itLM->first ] = out;

					//this->m_TransformedMovingLandmarks[ itLM->first ] = bsplineTrans->TransformPoint( itLM->second );
					std::cout << "Transformed: " << itLM->second << std::endl;
					std::cout << std::endl;
				}

				std::cout << "BSpline transform has been applied!" << std::endl;
				*/

				std::cout << "BSpline transform has not yet been implemented." << std::endl;
			}
			else
			{
				std::cerr << "TransformType is not recognized." << std::endl;
			}
		}
		return;
	}

	void LandmarkAnalysis::CalculateSSD()
	{
		// iterators to go through the fixed and transformed lists
		LandmarksType::const_iterator itFixed = this->m_FixedLandmarks.begin();
		LandmarksType::const_iterator itTransMoving = this->m_TransformedMovingLandmarks.begin();
		for(; itFixed != this->m_FixedLandmarks.end(); itFixed++ )
		{
			// calculate the difference between points
			PointType diff = itFixed->second - itTransMoving->second;
			m_diffx.push_back(diff[0]);
			m_diffy.push_back(diff[1]);
			m_diffz.push_back(diff[2]);

			// calculate ssd
			m_ssd.push_back( sqrt( pow(diff[0], 2.0) + pow(diff[1], 2.0) + pow(diff[2], 2.0) ) );
			itTransMoving++;
		}
		return;
	}

	double LandmarkAnalysis::mean( std::list< double > numbers )
	{
		// initialize variables
		double sum = 0.0;

		// create iterator and parse through list adding up all the numbers
		std::list< double >::iterator it = numbers.begin();
		for(; it != numbers.end(); it++)
		{
			sum += *it;
		}
		
		// divide by the number of numbers
		double mean = sum/numbers.size();
		return mean;
	}

	double LandmarkAnalysis::stdev( std::list< double > numbers )
	{
		// initialize variables and calculate mean
		double sum = 0.0;
		double mean = this->mean( numbers );

		// iterate through list and add up the sum of squared residuals
		std::list< double >::iterator it = numbers.begin();
		for(; it != numbers.end(); it++)
		{
			sum += pow((mean - *it), 2.0);
		}

		// take sqrt of sum of residuals/n-1
		double stdev = sqrt( sum/(numbers.size() - 1 ) );
		return stdev;
	}

	void LandmarkAnalysis::PrintResults( const char * resultsFilename, const char * transformedFilename )
	{
		std::cout << "\nPrinting Results to file." << std::endl;

		// open results file to append and write out only
		std::ofstream resultsFile;
		resultsFile.open( resultsFilename );
		
		// print out transform landmarks as a slicer file (negating the first two coordinates!!!)
		std::ofstream transFile;
		transFile.open( transformedFilename );

		// output first line of results file
		resultsFile << "Label,Fixedx,Fixedy,Fixedz,Movingx,Movingy,Movingz,Transx,Transy,Transz,Diffx,Diffy,Diffz,SSD\n";

		// output header of transformed list file
		transFile << "# Markups fiducial file version = 4.3\n";
		transFile << "# CoordinateSystem = 0\n";
		transFile << "# columns = id,x,y,z,ow,ox,oy,oz,vis,sel,lock,label,desc,associatedNodeID\n";
		
		// create iterator to parse through landmark lists and difference/ssd lists
		LandmarksType::const_iterator itF = this->m_FixedLandmarks.begin();
		LandmarksType::const_iterator itM = this->m_MovingLandmarks.begin();
		LandmarksType::const_iterator itT = this->m_TransformedMovingLandmarks.begin();
		std::list< double >::const_iterator itSSD = this->m_ssd.begin();
		std::list< double >::const_iterator itDx = this->m_diffx.begin();
		std::list< double >::const_iterator itDy = this->m_diffy.begin();
		std::list< double >::const_iterator itDz = this->m_diffz.begin();
		int i = 0;

		for(; itF != this->m_FixedLandmarks.end(); itF++ )
		{
			// output label
			resultsFile << itF->first << ",";

			// output fixedlandmarks
			PointType fixed = itF->second;
			resultsFile << fixed[0] << "," << fixed[1] << "," << fixed[2] << ",";

			// output movinglandmarks
			PointType moving = itM->second;
			resultsFile << moving[0] << "," << moving[1] << "," << moving[2] << ",";
			itM++;

			// output transformedlandmarks
			PointType trans = itT->second;
			resultsFile << trans[0] << "," << trans[1] << "," << trans[2] << ",";
			// determine new label for landmarks
			transFile << "vtkMRMLMarkupsFiducialNode_" << i << "," << trans[0] << "," << trans[1] << "," << trans[2] << ",0,0,0,1,1,1,0," << itT->first << "trans,,vtkMRMLModelNode5\n";
			itT++; i++;

			// output differencelandmarks
			//std::cout << *itDz << std::endl;
			resultsFile << *itDx << "," << *itDy << "," << *itDz << ",";
			itDx++; itDy++; itDz++;

			// output the ssd and end line
			resultsFile << *itSSD << "\n";
			itSSD++;
		}

		// output the mean of differences/ssd
		resultsFile << "mean,,,,,,,,,,";
		resultsFile << mean( this->m_diffx ) << "," << mean( this->m_diffy ) << "," << mean( this->m_diffz ) << "," << mean( this->m_ssd ) << "\n";

		// output the stdev of differences/ssd
		resultsFile << "std,,,,,,,,,,";
		resultsFile << stdev( this->m_diffx ) << "," << stdev( this->m_diffy ) << "," << stdev( this->m_diffz ) << "," << stdev( this->m_ssd ) << "\n";

		// close results file
		resultsFile.close();
		transFile.close();
		return;
	}


} // end namespace

#endif
