/*
Emily Hammond
2015/03/27

This is the class definition for ParseInputFile

*/

#ifndef __itkParseInputFile_hxx
#define __itkParseInputFile_hxx

#include "itkParseInputFile.h"

namespace itk
{
	void ParseInputFile::SetFilename( std::string filename )
	{
		// read in file and derive the proper variables
		SetDefaults();
		ReadInFile( filename );
		DeriveVariables();

		return;
	}

	void ParseInputFile::ReadInFile( std::string filename )
	{
		// open file
		std::ifstream myfile( filename.c_str() );

		// error checking
		if( !myfile.is_open() )
		{
			std::cout << "Error: Failed to open input file! " << std::endl;
			return;
		}

		// read in file line by line
		std::string line;
		while( getline( myfile, line ) )
		{
			std::cout << line << std::endl;
			std::size_t first;
			// search for the correct input
			if( line.compare( 0, 1, "#" ) != 0 ) // skip commented lines
			{
				if( line.find("FixedImageFilename") != std::string::npos )
				{

				}
				else if( line.find("MovingImageFilename") != std::string::npos )
				{
					// allocate variable
				}
				else if( line.find("FixedImageMaskFilename") != std::string::npos )
				{
					// allocate variable
				}
				else if( line.find("MovingImageMaskFilename") != std::string::npos )
				{
					// allocate variable
				}
				else if( line.find("OutputDirectory") != std::string::npos )
				{
					// allocate variable
				}
				else if( line.find("NumberOfHistogramBins") != std::string::npos )
				{
					// allocate variable
				}
				else if( line.find("FixedFiducialList") != std::string::npos )
				{
					// allocate variable
				}
				else if( line.find("MovingFiducialList") != std::string::npos )
				{
					// allocate variable
				}
				else if( line.find("PercentageOfSamples") != std::string::npos )
				{
					// allocate variable
				}
				else if( line.find("MaximumStepLength") != std::string::npos )
				{
					// allocate variable
				}
				else if( line.find("NumberOfIterations") != std::string::npos )
				{
					// allocate variable
				}
				else if( line.find("RelaxationFactor") != std::string::npos )
				{
					// allocate variable
				}
				else if( line.find("GradientMagnitudeTolerance") != std::string::npos )
				{
					// allocate variable
				}
				else if( line.find("RotationScale") != std::string::npos )
				{
					// allocate variable
				}
				else if( line.find("TranslationScale") != std::string::npos )
				{
					// allocate variable
				}
				else if( line.find("ScalingScale") != std::string::npos )
				{
					// allocate variable
				}
				else if( line.find("DefaultPixelValue") != std::string::npos )
				{
					// allocate variable
				}
				else
				{
					std::cout << "Error! Input variable not found." << std::endl;
				}
			}
		}

		return;
	}

	void ParseInputFile::DeriveVariables()
	{
		return;
	}
	
	void ParseInputFile::SetDefaults()
	{
		this->m_FixedImageFilename = "";
		this->m_MovingImageFilename = "";
		this->m_FixedImageMaskFilename = "";
		this->m_MovingImageMaskFilename = "";
		this->m_OutputDirectory = "";
		this->m_FixedFiducialList = "";
		this->m_MovingFiducialList = "";
		//metric
		this->m_NumberOfHistogramBins = 50;
		this->m_PercentageOfSamples = 0.01;
		//optimizer
		this->m_MaximumStepLength = 1.0;
		this->m_NumberOfIterations = 200;
		this->m_RelaxationFactor = 0.5;
		this->m_GradientMagnitudeTolerance = 0.01;
		this->m_RotationScale = 0.01;
		this->m_TranslationScale = 10.0;
		this->m_ScalingScale = 1.0;
		//resamples
		this->m_DefaultPixelValue = 0;

		return;
	}


} // end namespace

#endif