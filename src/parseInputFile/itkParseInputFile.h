/*
Emily Hammond
2015/03/25

This is the declaration to parse an input file and gather all the inputs. Its state
variables are the inputs taken from the file and those derived from the file, as well
as the desired outputs. The methods include those required to read the text file and
derived the additional information and print the inputs as well.

*/

#ifndef __itkParseInputFile_h
#define __itkParseInputFile_h

#include "itkImage.h"
#include <itksys/SystemTools.hxx>
#include <fstream>
#include <cmath>
#include <sstream>

namespace itk
{
/* class ParseInputFile
 *
 * This filter parses an input file and allocates the proper variables
 
*/
class ParseInputFile: public Object
{
public:
	typedef ParseInputFile				Self;
	typedef Object						Superclass;
	typedef SmartPointer< Self >		Pointer;
	typedef SmartPointer< const Self >	ConstPointer;

	// method for creation
	itkNewMacro(Self);

	// run-time type information and related methods
	itkTypeMacro(ParseInputFile, Object);

	void SetFilename( std::string filename );
	void Print();

	// get output variables
	std::string MovingHistogramFilename() const { return this->m_MovingHistogramFilename; };
	std::string FixedHistogramFilename() const { return this->m_FixedHistogramFilename; };
	std::string RigidInitGeomFilename() const { return this->m_RigidInitGeomFilename; };
	std::string RigidInitMetricFilename() const { return this->m_RigidInitMetricFilename; };
	std::string FinalRigidTransformFilename() const { return this->m_FinalRigidTransformFilename; };
	std::string JointHistogramFilename() const { return this->m_JointHistogramFilename; };
	std::string TransformedImageFilename() const { return this->m_TransformedImageFilename; };
	
protected:
	// declare the constructor and define default parameters
	ParseInputFile() {}
	
	// destructor
	virtual ~ParseInputFile() {}

	// this method is required to allocation memory for the output
	void GenerateData();
	
private:
	// declare variables
	std::string m_filename;
	//input files
	std::string m_FixedImageFilename;
	std::string m_MovingImageFilename;
	std::string m_FixedImageMaskFilename;
	std::string m_MovingImageMaskFilename;
	std::string m_OutputDirectory;
	std::string m_FixedFiducialList;
	std::string m_MovingFiducialList;
	//metric
	int m_NumberOfHistogramBins;
	float m_PercentageOfSamples;
	//optimizer
	float m_MaximumStepLength;
	int m_NumberOfIterations;
	float m_RelaxationFactor;
	float m_GradientMagnitudeTolerance;
	float m_RotationScale;
	float m_TranslationScale;
	float m_ScalingScale;
	//resamples
	int m_DefaultPixelValue;
	//output files
	std::string m_MovingHistogramFilename;
	std::string m_FixedHistogramFilename;
	std::string m_RigidInitGeomFilename;
	std::string m_RigidInitMetricFilename;
	std::string m_FinalRigidTransformFilename;
	std::string m_JointHistogramFilename;
	std::string m_TransformedImageFilename;
	//derived
	std::string m_BaseMovingFilename;
	std::string m_BaseFixedFilename;

	// private member functions
	void ReadInFile( std::string filename );
	void DeriveVariables();
	void SetDefaults();
};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkParseInputFile.hxx"
#endif

#endif
