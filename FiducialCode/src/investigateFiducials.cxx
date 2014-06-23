/*
 * Emily Hammond
 * 5/29/14
 *
 * The purpose of this code is to experiment with fiducials and how they coordinate between Slicer
 * and ITK.
 *
 */

#include "itkImage.h"
#include "itkImageFileReader.h"

int main( int argc, char * argv[] )
{
    char * filename = argv[1];

    // load in an image
    const unsigned int  Dimension = 3;
    typedef float       PixelType;
    typedef itk::Image< PixelType, Dimension >  ImageType;
    typedef itk::ImageFileReader< ImageType >   ReaderType;
    ReaderType::Pointer reader = ReaderType::New();

    reader->SetFileName( filename );
    reader->Update();

    return 0;
}

