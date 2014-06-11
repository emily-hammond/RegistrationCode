
/*
 *Author: Emily Hammond
 *Date 6. 11. 2014
 *
 *The purpose of this registration is to simply perform an affine registration and determine how well it performed
 *based on input fiducials.
 *
 */

// reading/writing images
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

// perform the registration
#include "itkMultiResolutionImageRegistrationMethod.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkAffineTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkRegularStepGradientDescentOptimizer.h"

// additional components
#include "itkCenteredTransformInitializer.h"
#include "itkResampleImageFilter.h"

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

    // set up registration
    typedef itk::AffineTransform<double, Dimension>                                             AffineTransformType;
    typedef itk::MattesMutualInformationImageToImageMetric<FixedImageType, MovingImageType>     MetricType;
    typedef itk::RegularStepGradientDescentOptimizer                                            OptimiaerType;
    typedef itk::LinearInterpolateImageFunction<MovingImageType, double>                        InterpolatorType;
    typedef itk::MultiResolutionImageRegistrationMethod<FixedImageType, MovingImageType>        RegistrationType;

    // instantiate objects for registration components
    AffineTransformType::Pointer    affineTransform = AffineTransformType::New();
    MetricType::Pointer             metric = MetricType::New();
    OptimizerType::Pointer          optimizer = OptimizerType::New();
    InterpolatorType::Pointer       interpolator = InterpolatorType::New();
    RegistrationType::Pointer       registration = RegistrationType::New();



    return EXIT_SUCCESS;
}
