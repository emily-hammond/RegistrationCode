/*
 * Emily Hammond
 * 2015/03/27
 *
 * Goals: test the functionality of the ParseInputFile class
 *
 */

#include "itkParseInputFile.h"

// test all functionality
int main( int argc, char * argv[] )
{
	std::string filename = "C:\\Users\\ehammond\\Documents\\ITKprojects\\RegistrationCode\\src\\parseInputFile\\sampleInputFile.txt";

	// test itkLandmarkAnalysis class
	typedef itk::ParseInputFile		ParsingType;
	ParsingType::Pointer parseFile = ParsingType::New();

	// set filename
	parseFile->SetFilename( filename );

	return EXIT_SUCCESS;
}
