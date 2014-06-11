
/*
 *Author: Emily Hammond
 *Date 6. 11. 2014
 *
 *The purpose of this registration is to simply perform an affine registration and determine how well it performed
 *based on input fiducials.
 *
 */

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

int main( int argc, char * argv[] )
{
    // obtain the necessary inputs from the command line
    char * fisedFilename = argv[1];
    char * movingFilename = argv[2];
    char * outputFilename = argv[3];
    char * fiducialFilename = argv[4];

    // set up image parameters
    const unsigned int  Dimension = 3;
    typedef float       PixelType;
    typedef itk::Image<PixelType, Dimension>    FixedImageType;
    typedef itk::Image<FixelType, Dimension>    MovingImageType;

    // read in images
    typedef itk::ImageFileReader<FixedImageType>    FixedImageReaderType;
    FixedImageReaderType::Pointer fixedReader = FixedImageReaderType::New();
    fixedReader->SetFileName( fixedFilename );
    fixedReader->Update();

    typedef itk::ImageFileReader<MovingImageType>   MovingImageReaderType;
    MovingImageReaderType::Pointer movingReader = MovingImageReaderType::New();
    movingReader->SetFilenName( movingFilename );
    movingReader->Update();


    return EXIT_SUCCESS;
}
