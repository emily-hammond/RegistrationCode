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
			std::string name;
			// search for the correct input
			if( line.compare( 0, 1, "#" ) != 0 ) // skip commented lines
			{
				if( line.find("FixedImageFilename") != std::string::npos )
				{
					std::stringstream convertor(line);
					convertor >> name >> this->m_FixedImageFilename;
				}
				else if( line.find("MovingImageFilename") != std::string::npos )
				{
					std::stringstream convertor(line);
					convertor >> name >> this->m_MovingImageFilename;
				}
				else if( line.find("FixedImageMaskFilename") != std::string::npos )
				{
					std::stringstream convertor(line);
					convertor >> name >> this->m_FixedImageMaskFilename;
				}
				else if( line.find("MovingImageMaskFilename") != std::string::npos )
				{
					std::stringstream convertor(line);
					convertor >> name >> this->m_MovingImageMaskFilename;
				}
				else if( line.find("OutputDirectory") != std::string::npos )
				{
					std::stringstream convertor(line);
					convertor >> name >> this->m_OutputDirectory;
				}
				else if( line.find("NumberOfHistogramBins") != std::string::npos )
				{
					std::stringstream convertor(line);
					convertor >> name >> this->m_NumberOfHistogramBins;
				}
				else if( line.find("FixedFiducialList") != std::string::npos )
				{
					std::stringstream convertor(line);
					convertor >> name >> this->m_FixedFiducialList;
				}
				else if( line.find("MovingFiducialList") != std::string::npos )
				{
					std::stringstream convertor(line);
					convertor >> name >> this->m_MovingFiducialList;
				}
				else if( line.find("PercentageOfSamples") != std::string::npos )
				{
					std::stringstream convertor(line);
					convertor >> name >> this->m_PercentageOfSamples;
				}
				else if( line.find("MaximumStepLength") != std::string::npos )
				{
					std::stringstream convertor(line);
					convertor >> name >> this->m_MaximumStepLength;
				}
				else if( line.find("NumberOfIterations") != std::string::npos )
				{
					std::stringstream convertor(line);
					convertor >> name >> this->m_NumberOfIterations;
				}
				else if( line.find("RelaxationFactor") != std::string::npos )
				{
					std::stringstream convertor(line);
					convertor >> name >> this->m_RelaxationFactor;
				}
				else if( line.find("GradientMagnitudeTolerance") != std::string::npos )
				{
					std::stringstream convertor(line);
					convertor >> name >> this->m_GradientMagnitudeTolerance;
				}
				else if( line.find("RotationScale") != std::string::npos )
				{
					std::stringstream convertor(line);
					convertor >> name >> this->m_RotationScale;
				}
				else if( line.find("TranslationScale") != std::string::npos )
				{
					std::stringstream convertor(line);
					convertor >> name >> this->m_TranslationScale;
				}
				else if( line.find("ScalingScale") != std::string::npos )
				{
					std::stringstream convertor(line);
					convertor >> name >> this->m_ScalingScale;
				}
				else if( line.find("DefaultPixelValue") != std::string::npos )
				{
					std::stringstream convertor(line);
					convertor >> name >> this->m_DefaultPixelValue;
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

	void ParseInputFile::Print()
	{
		std::cout << std::endl;
		std::cout << "***** INPUTS *****" << std::endl;
		std::cout << "FixedImageFilename " << m_FixedImageFilename << std::endl;
		std::cout << "MovingImageFilename " << m_MovingImageFilename << std::endl;
		std::cout << "FixedImageMaskFilename " << m_FixedImageMaskFilename << std::endl;
		std::cout << "MovingImageMaskFilename " << m_MovingImageMaskFilename << std::endl;
		std::cout << "FixedFiducialList " << m_FixedFiducialList << std::endl;
		std::cout << "MovingFiducialList " << m_MovingFiducialList << std::endl;
		std::cout << "OutputDirectory " << m_OutputDirectory << std::endl;
		std::cout << "NumberOfHistogramBins " << m_NumberOfHistogramBins << std::endl;
		std::cout << "PercentageOfSamples " << m_PercentageOfSamples << std::endl;
		std::cout << "MaximumStepLength " << m_MaximumStepLength << std::endl;
		std::cout << "NumberOfIterations " << m_NumberOfIterations << std::endl;
		std::cout << "RelaxationFactor " << m_RelaxationFactor << std::endl;
		std::cout << "GradientMagnitudeTolerance " << m_GradientMagnitudeTolerance << std::endl;
		std::cout << "RotationScale " << m_RotationScale << std::endl;
		std::cout << "TranslationScale " << m_TranslationScale << std::endl;
		std::cout << "ScalingScale " << m_ScalingScale << std::endl;
		std::cout << "DefaultPixelValue " << m_DefaultPixelValue << std::endl;
	}

} // end namespace

#endif