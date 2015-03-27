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
	ParseInputFile::ParseInputFile( std::string filename )
	{
		ReadInFile( filename );
	}

	void ParseInputFile::ReadInFile( std::string filename )
	{
		// open file
		std::ifstream myfile( filename.c_str() );

		// error checking
		if( !myfile.is_open() )
		{
			std::cerr << "Error: Failed to open input file! " << std::endl;
			return;
		}



} // end namespace

#endif