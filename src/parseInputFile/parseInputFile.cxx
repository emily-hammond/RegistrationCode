/*
 * Emily Hammond
 * 2015/03/27
 *
 * Goals: test the functionality of the ParseInputFile class
 *
 */

#include "itkParseInputFile.h"
#include <windows.h>

// test all functionality
int main( int argc, char * argv[] )
{
	const char * filename = "H:\\Results\\MR_CT_RigidRegistrations\\2015.04.02_Results\\AP30372_TP1_results\\AP30372_TP1_Vibe3D_BH_pre_parameters.txt";

	// test itkLandmarkAnalysis class
	typedef itk::ParseInputFile		ParsingType;
	ParsingType::Pointer parseFile = ParsingType::New();

	// set filename
	parseFile->SetFilename( filename );
	parseFile->Print();

	// create directory at results directory
	if(!CreateDirectory( parseFile->OutputDirectory().c_str(), NULL ))
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
