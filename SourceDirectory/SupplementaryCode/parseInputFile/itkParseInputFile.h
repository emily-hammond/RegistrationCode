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

	// error
	bool IsOpen() const { return this->m_IsOpen; };
	// get output variables
	std::string MovingHistogramFilename() const { return this->m_MovingHistogramFilename; };
	std::string FixedHistogramFilename() const { return this->m_FixedHistogramFilename; };
	std::string InitGeomFilename() const { return this->m_InitGeomFilename; };
	std::string InitMetricFilename() const { return this->m_InitMetricFilename; };
	std::string RigidTransformFilename() const { return this->m_RigidTransformFilename; };
	std::string JointHistogramFilename() const { return this->m_JointHistogramFilename; };
	std::string TransformedImageFilename() const { return this->m_TransformedImageFilename; };
	std::string PrematureTransformFilename() const { return this->m_PrematureTransformFilename; };
	// get input variables
	std::string FixedImageFilename() const { return this->m_FixedImageFilename; };
	std::string MovingImageFilename() const { return this->m_MovingImageFilename; };
	std::string FixedImageMaskFilename() const { return this->m_FixedImageMaskFilename; };
	std::string MovingImageMaskFilename() const { return this->m_MovingImageMaskFilename; };
	std::string OutputDirectory() const { return this->m_OutputDirectory; };
	std::string FixedFiducialList() const { return this->m_FixedFiducialList; };
	std::string MovingFiducialList() const { return this->m_MovingFiducialList; };
	std::string InitialTransformFilename() const { return this->m_InitialTransformFilename; };
	//metric
	int NumberOfHistogramBins() const { return this->m_NumberOfHistogramBins; };
	float PercentageOfSamples() const { return this->m_PercentageOfSamples; };
	//optimizer
	float MaximumStepLength() const { return this->m_MaximumStepLength; };
	int NumberOfIterations() const { return this->m_NumberOfIterations; };
	float RelaxationFactor() const { return this->m_RelaxationFactor; };
	float GradientMagnitudeTolerance() const { return this->m_GradientMagnitudeTolerance; };
	float RotationScale() const { return this->m_RotationScale; };
	float TranslationScale() const { return this->m_TranslationScale; };
	float ScalingScale() const { return this->m_ScalingScale; };
	//resamples
	int DefaultPixelValue() const { return this->m_DefaultPixelValue; };
	bool WriteImage() const { return this->m_WriteImage; };
	bool MetricInitialization() const { return this->m_MetricInitialization; };
	bool PerformOverlapMeasures() const { return this->m_PerformOverlapMeasures; };
	
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
	bool m_IsOpen;
	//input files
	std::string m_FixedImageFilename;
	std::string m_MovingImageFilename;
	std::string m_FixedImageMaskFilename;
	std::string m_MovingImageMaskFilename;
	std::string m_OutputDirectory;
	std::string m_FixedFiducialList;
	std::string m_MovingFiducialList;
	std::string m_InitialTransformFilename;
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
	bool m_WriteImage;
	bool m_MetricInitialization;
	bool m_PerformOverlapMeasures;
	//output files
	std::string m_MovingHistogramFilename;
	std::string m_FixedHistogramFilename;
	std::string m_InitGeomFilename;
	std::string m_InitMetricFilename;
	std::string m_RigidTransformFilename;
	std::string m_JointHistogramFilename;
	std::string m_TransformedImageFilename;
	std::string m_PrematureTransformFilename;

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
