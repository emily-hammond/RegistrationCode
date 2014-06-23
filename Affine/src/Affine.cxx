
/*
 *Author: Emily Hammond
 *Date 6. 11. 2014
 *
 *The purpose of this registration is to simply perform an affine registration and determine how well it performed
 *based on input fiducials.
 *
 */

// reading/writing images
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

// perform the registration
#include "itkMultiResolutionImageRegistrationMethod.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkAffineTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkRegularStepGradientDescentOptimizer.h"

// additional components
#include "itkMultiResolutionPyramidImageFilter.h"
#include "itkCenteredTransformInitializer.h"
#include "itkResampleImageFilter.h"

// additional c headers
#include <fstream>
#include <stdio.h>

int main( int argc, char * argv[] )
{
    // obtain the necessary inputs from the command line
    char * fixedFilename = argv[1];
    char * movingFilename = argv[2];
    char * outputFilename = argv[3];
    char * fiducialFilename = argv[4];

    // set up image parameters
    const unsigned int  Dimension = 3;
    typedef float       PixelType;
    typedef itk::Image<PixelType, Dimension>    FixedImageType;
    typedef itk::Image<PixelType, Dimension>    MovingImageType;

    // read in images
    typedef itk::ImageFileReader<FixedImageType>    FixedImageReaderType;
    FixedImageReaderType::Pointer fixedReader = FixedImageReaderType::New();
    fixedReader->SetFileName( fixedFilename );
    fixedReader->Update();

    typedef itk::ImageFileReader<MovingImageType>   MovingImageReaderType;
    MovingImageReaderType::Pointer movingReader = MovingImageReaderType::New();
    movingReader->SetFileName( movingFilename );
    movingReader->Update();

    // set up registration
    typedef itk::AffineTransform<double, Dimension>                                             AffineTransformType;
    typedef itk::MattesMutualInformationImageToImageMetric<FixedImageType, MovingImageType>     MetricType;
    typedef itk::RegularStepGradientDescentOptimizer                                            OptimizerType;
    typedef itk::LinearInterpolateImageFunction<MovingImageType, double>                        InterpolatorType;
    typedef itk::MultiResolutionImageRegistrationMethod<FixedImageType, MovingImageType>        RegistrationType;

    // instantiate objects for registration components
    AffineTransformType::Pointer    affineTransform = AffineTransformType::New();
    MetricType::Pointer             metric = MetricType::New();
    OptimizerType::Pointer          optimizer = OptimizerType::New();
    InterpolatorType::Pointer       interpolator = InterpolatorType::New();
    RegistrationType::Pointer       registration = RegistrationType::New();

    // set up an image pyramid for multi-res
    typedef itk::MultiResolutionPyramidImageFilter<FixedImageType, FixedImageType>              FixedPyramidType;
    typedef itk::MultiResolutionPyramidImageFilter<MovingImageType, MovingImageType>            MovingPyramidType;

    FixedPyramidType::Pointer       fixedPyramid = FixedPyramidType::New();
    MovingPyramidType::Pointer      movingPyramid = MovingPyramidType::New();

    // plug in components of registration
    registration->SetTransform( affineTransform );
    registration->SetMetric( metric );
    registration->SetOptimizer( optimizer );
    registration->SetInterpolator( interpolator );
    registration->SetFixedImagePyramid( fixedPyramid );
    registration->SetMovingImagePyramid( movingPyramid );

    // denote the images for the registration
    registration->SetFixedImage( fixedReader->GetOutput() );
    registration->SetMovingImage( movingReader->GetOutput() );

    // update and set fixed image region
    // (must have the fixed reader updated before this!!)
    registration->SetFixedImageRegion( fixedReader->GetOutput()->GetBufferedRegion() );

    // initialize the two images
    typedef itk::CenteredTransformInitializer<AffineTransformType, FixedImageType, MovingImageType> InitializerType;
    InitializerType::Pointer initializer = InitializerType::New();

    // determine the parameters
    initializer->SetTransform( affineTransform );
    initializer->SetFixedImage( fixedReader->GetOutput() );
    initializer->SetMovingImage( movingReader->GetOutput() );

    // align by geometry and apply
    initializer->GeometryOn();
    initializer->InitializeTransform();

    // plug initialization results into registration
    registration->SetInitialTransformParameters( affineTransform->GetParameters() );

    // finish setting up transform by including the optimizer scales
    typedef OptimizerType::ScalesType OptimizerScalesType;
    OptimizerScalesType optimizerScales( affineTransform->GetNumberOfParameters() );
    const double translationScale = 1.0/1000.0;

    // 12 affine parameters
    optimizerScales[0] = 1.0;
    optimizerScales[1] = 1.0;
    optimizerScales[2] = 1.0;
    optimizerScales[3] = 1.0;
    optimizerScales[4] = 1.0;
    optimizerScales[5] = 1.0;
    optimizerScales[6] = 1.0;
    optimizerScales[7] = 1.0;
    optimizerScales[8] = 1.0;

    optimizerScales[9] = translationScale;
    optimizerScales[10] = translationScale;
    optimizerScales[11] = translationScale;

    optimizer->SetScales( optimizerScales );
    optimizer->SetMaximumStepLength( 0.2 );
    optimizer->SetMinimumStepLength( 0.0001 );
    optimizer->SetNumberOfIterations( 1000 );

    // set up metric
    // (insert masks here if using them!)
    metric->SetNumberOfHistogramBins( 128 );
    metric->SetNumberOfSpatialSamples( 50000 );

    // set up optimizer
    // (input parameter changes here if desired)

    // define number of pyramid levels
    registration->SetNumberOfLevels( 3 );

    try
    {
        registration->Update();
        std::cout << "Optimizer Stop Condition: " << registration->GetOptimizer()->GetStopConditionDescription() << std::endl;
    }
    catch(itk::ExceptionObject & err)
    {
        std::cout << "ExceptionObject caught!" << std::endl;
        std::cout << err << std::endl;
        return EXIT_FAILURE;
    }

    // resample moving image according to the transform
    typedef itk::ResampleImageFilter<MovingImageType, FixedImageType> ResampleFilterType;
    ResampleFilterType::Pointer resampler = ResampleFilterType::New();

    // obtain the final transform parameters and plug into new transform
    OptimizerType::ParametersType finalAffineParameters = registration->GetLastTransformParameters();
    AffineTransformType::Pointer finalAffineTransform = AffineTransformType::New();

    finalAffineTransform->SetCenter( affineTransform->GetCenter() );
    finalAffineTransform->SetParameters( finalAffineParameters );
    finalAffineTransform->SetFixedParameters( affineTransform->GetFixedParameters() );

    // input into resampler
    resampler->SetTransform( finalAffineTransform );
    resampler->SetInput( movingReader->GetOutput() );

    // input fixed image parameters into the resampler
    FixedImageType::Pointer fixedImage = fixedReader->GetOutput();
    resampler->SetSize( fixedImage->GetLargestPossibleRegion().GetSize() );
    resampler->SetOutputOrigin( fixedImage->GetOrigin() );
    resampler->SetOutputSpacing( fixedImage->GetSpacing() );
    resampler->SetOutputDirection( fixedImage->GetDirection() );

    // set pixel value to denote the area that the moving image is not mapped to in the fixed image
    resampler->SetDefaultPixelValue( 128 );

    // write image out to file
    typedef itk::Image<PixelType, Dimension>    OutputImageType;
    typedef itk::ImageFileWriter<OutputImageType> WriterType;

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( outputFilename );
    writer->SetInput( resampler->GetOutput() );
    writer->Update();

    // read in the fiducials and find the proper image
    // create necessary variables
    std::string line;
    char image[255];
    float one, two, three, four, five, six, seven, eight, nine;

    // fixed image instantiation of points
    FixedImageType::PointType fixedCorina;
    FixedImageType::PointType fixedBaseHeart;
    FixedImageType::PointType fixedAorta;

    // moving image instantiations of points
    MovingImageType::PointType movingCorina;
    MovingImageType::PointType movingBaseHeart;
    MovingImageType::PointType movingAorta;

    // open file
    std::fstream fidFile;
    fidFile.open( fiducialFilename );

    // parse through file and find the proper fixed and moving image
    do
    {
        std::getline(fidFile, line);
        sscanf(line.c_str(), "%s", image);

        // check if it matches the fixedFilename
        if(strcmp(image, fixedFilename) == 0)
        {
            sscanf(line.c_str(), "%s %f %f %f %f %f %f %f %f %f",image, &one, &two, &three, &four, &five, &six, &seven, &eight, &nine);

            fixedCorina[0] = -one;
            fixedCorina[1] = -two;
            fixedCorina[2] = three;
            fixedBaseHeart[0] = -four;
            fixedBaseHeart[1] = -five;
            fixedBaseHeart[2] = six;
            fixedAorta[0] = -seven;
            fixedAorta[1] = -eight;
            fixedAorta[2] = nine;
        }
        else if(strcmp(image, movingFilename) == 0)
        {
            sscanf(line.c_str(), "%s %f %f %f %f %f %f %f %f %f",image, &one, &two, &three, &four, &five, &six, &seven, &eight, &nine);

            movingCorina[0] = -one;
            movingCorina[1] = -two;
            movingCorina[2] = three;
            movingBaseHeart[0] = -four;
            movingBaseHeart[1] = -five;
            movingBaseHeart[2] = six;
            movingAorta[0] = -seven;
            movingAorta[1] = -eight;
            movingAorta[2] = nine;
        }
        else{}
    }while( fidFile.good() );

    // close file
    fidFile.close();

    // transform moving fiducials
    MovingImageType::PointType movingTransformedCorina = finalAffineTransform->TransformPoint( movingCorina );
    MovingImageType::PointType movingTransformedBaseHeart = finalAffineTransform->TransformPoint( movingBaseHeart );
    MovingImageType::PointType movingTransformedAorta = finalAffineTransform->TransformPoint( movingAorta );

    std::cout << std::endl;
    std::cout << "Pre: " << movingCorina << std::endl;
    std::cout << std::endl;
    std::cout << "Post: " << movingTransformedCorina << std::endl;
    std::cout << std::endl;
    std::cout << "Fin: " << fixedCorina << std::endl;
    std::cout << std::endl;
    std::cout << finalAffineTransform << std::endl;
    std::cout << std::endl;

    return EXIT_SUCCESS;
}
