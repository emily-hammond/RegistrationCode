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
			this->m_IsOpen = false;
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
				else if( line.find("Write") != std::string::npos )
				{
					if( line.find("True") != std::string::npos )
					{
						this->m_WriteImage = true;
					}
					else if( line.find("False") != std::string::npos )
					{
						this->m_WriteImage = false;
					}
					else
					{
						std::cout << "Write option is not recognized." << std::endl;
						std::cout << "   Please choose True or False" << std::endl;
					}
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
		// determine the base fixed and moving image names
		std::string baseMovingFilename = this->m_MovingImageFilename.substr( this->m_MovingImageFilename.find_last_of("/\\") + 1 );
		baseMovingFilename = baseMovingFilename.substr(0, baseMovingFilename.find_last_of('.'));

		std::string baseFixedFilename = this->m_FixedImageFilename.substr( this->m_FixedImageFilename.find_last_of("/\\") + 1 );
		baseFixedFilename = baseFixedFilename.substr(0, baseFixedFilename.find_last_of('.'));

		// create output filenames for the various outputs
		this->m_MovingHistogramFilename = this->m_OutputDirectory + "\\" + baseMovingFilename + "_Histogram.txt";
		this->m_FixedHistogramFilename = this->m_OutputDirectory + "\\" + baseFixedFilename + "_Histogram.txt";
		this->m_InitGeomFilename = this->m_OutputDirectory + "\\" + baseMovingFilename + "_InitGeomTransform.tfm";
		this->m_InitMetricFilename = this->m_OutputDirectory + "\\" + baseMovingFilename + "_InitMetricTransform.tfm";
		this->m_RigidTransformFilename = this->m_OutputDirectory + "\\" + baseMovingFilename + "_RigidTransform.tfm";
		this->m_JointHistogramFilename = this->m_OutputDirectory + "\\" + baseMovingFilename + "_FinalJointHistogram.tif";
		this->m_TransformedImageFilename = this->m_OutputDirectory + "\\" + baseMovingFilename + "_Transformed.mhd";
		this->m_PrematureTransformFilename = this->m_OutputDirectory + "\\" + baseMovingFilename + "_PrematureTransform.mhd";

		return;
	}
	
	void ParseInputFile::SetDefaults()
	{
		// set the default parameters for each input value
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
		this->m_WriteImage = false;

		return;
	}

	void ParseInputFile::Print()
	{
		// print out all the member variables in the class
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
		std::cout << "WriteImage " << m_WriteImage << std::endl;
		std::cout << "***** OUTPUTS *****" << std::endl;
		std::cout << "MovingHistogramFilename " << m_MovingHistogramFilename << std::endl;
		std::cout << "FixedHistogramFilename " << m_FixedHistogramFilename << std::endl;
		std::cout << "InitialGeomTransform " << m_InitGeomFilename << std::endl;
		std::cout << "InitialMetricTransform " << m_InitMetricFilename << std::endl;
		std::cout << "RigidTransformFilename " << m_RigidTransformFilename << std::endl;
		std::cout << "JointHistogramFilename " << m_JointHistogramFilename << std::endl;
		std::cout << "TransformedImageFilename " << m_TransformedImageFilename << std::endl;
	}

} // end namespace

#endif