/*
 * Emily Hammond
 * 2015/01/21
 *
 * Goals: test the functionality of the LandmarkAnalysis class
 *
 */

#include "itkLandmarkAnalysis.h"
#include "itkTranslationTransform.h"
#include "itkTransformFileWriter.h"

// test all functionality and perform statistics on landmarks
int main( int argc, char * argv[] )
{
	// test itkLandmarkAnalysis class
	typedef itk::LandmarkAnalysis	AnalysisType;
	typedef itk::LandmarkAnalysis::LandmarksType LandmarksType;
	AnalysisType::Pointer lmAnalysis = AnalysisType::New();

	// input fiducial filenames
	lmAnalysis->SetFixedLandmarksFilename( argv[1] );
	lmAnalysis->SetMovingLandmarksFilename( argv[2] );
	lmAnalysis->SetTransformFilename( argv[3] );
	//lmAnalysis->SetTransformFilename( argv[4] );

	//lmAnalysis->PrintTransformList();

	lmAnalysis->PerformStatistics();
	lmAnalysis->PrintResults( argv[4], argv[5] );

	//lmAnalysis->ConvertTransforms();

	return EXIT_SUCCESS;
}
