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
    // obtain inputs
    char * filename = argv[1];
    int index0 = atoi( argv[2] );
    int index1 = atoi( argv[3] );
    int index2 = atoi( argv[4] );

    // load in an image
    const unsigned int  Dimension = 3;
    typedef float       PixelType;
    typedef itk::Image< PixelType, Dimension >  ImageType;
    typedef itk::ImageFileReader< ImageType >   ReaderType;
    ReaderType::Pointer reader = ReaderType::New();

    reader->SetFileName( filename );
    reader->Update();

    // create image
    ImageType::Pointer image = reader->GetOutput();
    std::cout << image << std::endl;

    // obtain an index of the image
    ImageType::IndexType indexLocation;

    indexLocation[0] = index0;
    indexLocation[1] = index1;
    indexLocation[2] = index2;

    std::cout << indexLocation << std::endl;

    // obtain the pixel value at that image coordinate
    ImageType::PixelType pixelValue = image->GetPixel( indexLocation );
    std::cout << pixelValue << std::endl;

    // see what the corresponding physical point is
    ImageType::PointType physicalPoint;
    image->TransformIndexToPhysicalPoint( indexLocation, physicalPoint );

    std::cout << std::endl;
    std::cout << physicalPoint << std::endl;

    return 0;
}

