/*
Emily Hammond
2015/01/23

This is the declaration for the itkLandmarkAssessment class that performs statistics on 
fiducials from a fixed and moving image by applying a transform to the moving image landmarks
and performing statistics.

It contains these state variables:
	m_FixedLandmarks
	m_MovingLandmarks
	m_TransformedMovingLandmarks
	m_TransformList

It has these methods:
	ReadLandmarks( landmarkFilename )
	WriteLandmarks
	WriteLandmarkAnalysisResults
	ReadTransform
	ApplyTransform
	PerformStatistics

*/

#ifndef __itkLandmarkAnalysis_h
#define __itkLandmarkAnalysis_h

#include "itkImage.h"
#include "itkTransformFileReader.h"
#include "itkAffineTransform.h"
#include "itkBSplineTransform.h"
#include "itkIdentityTransform.h"

#include "vtkSmartPointer.h"
#include "vtkMatrix4x4.h"
#include "itkImageRegionIterator.h"
//#include "vtkITKTransformConverter.h"

#include <itksys/SystemTools.hxx>
#include <fstream>
#include <cmath>

namespace itk
{
/* class LandmarkAnalysis
 *
 * This filter analyzes fiducials between fixed/transformed moving images
 
*/

class LandmarkAnalysis: public Object
{
public:
	typedef LandmarkAnalysis								Self;
	typedef Object											Superclass;
	typedef SmartPointer< Self >							Pointer;
	typedef SmartPointer< const Self >						ConstPointer;
	
	// Method for creation through the object factory
	itkNewMacro(Self);
	
	// Run-time type information and related methods
	itkTypeMacro(LandmarkAnalysis, Object);

	// create typedefs for components
	typedef itk::Image< float, 3 >							FloatImageType;
	typedef FloatImageType::PointType						PointType;
	typedef std::map<std::string, PointType>				LandmarksType;

	// declarations for reading template files
	typedef itk::TransformFileReaderTemplate< double >		TransformReaderType;
	typedef TransformReaderType::TransformListType			TransformListType;

	// declarations for potential transforms
	typedef itk::Transform< double >						TransformType;
	typedef itk::AffineTransform< double >					AffineTransformType;
	typedef itk::BSplineTransform< double >					BSplineTransformType;
	typedef itk::IdentityTransform< double >				IdentityTransformType;

	// get for the desired state variables
	LandmarksType GetFixedLandmarks() const { return this->m_FixedLandmarks; };
	LandmarksType GetMovingLandmarks() const { return this->m_MovingLandmarks; };
	LandmarksType GetTransformedMovingLandmarks() const { return this->m_TransformedMovingLandmarks; };

	// declare functions to read in landmark lists and transforms
	void SetFixedLandmarksFilename( const char * fixedLandmarksFilename );
	void SetMovingLandmarksFilename( const char * movingLandmarksFilename );
	void SetTransformFilename( const char * transformFilename );

	// print out data
	void PrintLandmarks( LandmarksType landmarks ) const;
	void PrintTransformList() const;

	// perform the statistics
	void PerformStatistics();
	void PrintResults( const char * resultsFilename, const char * transformedFilename );
	
protected:
	// declare the constructor and define default parameters
	LandmarkAnalysis() 
	{
		// initialize transform list with an identity transform
		std::string identityFilename = "C:\\Users\\ehammond\\Documents\\ITK\\ITK-src\\Examples\\Data\\IdentityTransform.tfm";
		SetTransformFilename( identityFilename.c_str() );
	}
	
	// destructor
	virtual ~LandmarkAnalysis() {}
	
	void GenerateData();
	
private:
	// purposefully not implemented copy constructor and assignment operator
	// prevent instantiating the filter without the factory methods
	LandmarkAnalysis(const Self &);
	void operator=(const Self &);
	
	// declare variables
	LandmarksType m_FixedLandmarks;
	LandmarksType m_MovingLandmarks;
	LandmarksType m_TransformedMovingLandmarks;
	TransformListType m_TransformList;
	std::list< double > m_ssd;
	std::list< double > m_diffx;
	std::list< double > m_diffy;
	std::list< double > m_diffz;

	// private function that do most of the functionality
	LandmarksType ReadLandmarksFile( const char * filename );
	TransformListType ReadTransformFile( const char * transformFilename );
	void TransformLandmarks();
	void CalculateSSD();
	double mean( std::list< double > numbers );
	double stdev( std::list< double > numbers );
	AffineTransformType::Pointer ConvertTransform( AffineTransformType::Pointer affine);
	BSplineTransformType::Pointer ConvertTransform( BSplineTransformType::Pointer bspline);

};
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLandmarkAnalysis.hxx"
#endif

#endif
