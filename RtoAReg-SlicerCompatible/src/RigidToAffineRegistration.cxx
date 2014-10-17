/*
Author: Emily Hammond
Date: 4.17.14

The purpose of this program is to perform a rigid registration to then initialize an affine registration.
The registration will then be tested on MR/CT lung images.

Goals:
1. Read in the images and print out to screen to make sure they are properly loaded
2. Perform preprocessing and apply that to the images
        - rough lung masks, intensity smoothing (MR)??

******* Perform testing to determine if mask files make a difference in registration********

3. Rigid registration
        - versor 3D rigid transform
        - This code follows the example code in Registration/ImageRegistration8.cxx from ITK v4.6.0
        - It makes use of v3 registration framework
4. Affine registration
        - This code follows the affine registration portion of Registration/DeformableRegistration15.cxx
          example code from ITK 4.6.0
5. Perform post-processing
        - apply final transform to images
6. Make the code slicer compatible to easily test algorithm and obtain results
        - write the .xml file
7. Obtain results
        - observe deformation field in Slicer module
        - track changes in lungs (TP1-4) in Slicer module
8. Write report

*/

// read and write images
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageMaskSpatialObject.h"

// components of the registration
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkVersorRigid3DTransform.h"
#include "itkImageRegistrationMethod.h"
#include "itkVersorRigid3DTransformOptimizer.h"

#include "itkAffineTransform.h"
#include "itkRegularStepGradientDescentOptimizer.h"

// secondary components
#include "itkCenteredTransformInitializer.h"
#include "itkResampleImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkTimeProbesCollectorBase.h"

#include "RigidToAffineRegistrationCLP.h"

int main(int argc, char * argv[])
{
    PARSE_ARGS;

    // set up timer to time various stages of code
    itk::TimeProbesCollectorBase timer;

    // time the entire process
    timer.Start("Complete Code");

    // set up image parameters
    const unsigned int                          Dimension = 3;
    typedef float                               PixelType;
    typedef itk::Image<PixelType, Dimension>    FixedImageType;
    typedef itk::Image<PixelType, Dimension>    MovingImageType;

    // read in images
    typedef itk::ImageFileReader<FixedImageType>    FixedImageReaderType;
    typedef itk::ImageFileReader<MovingImageType>   MovingImageReaderType;

    FixedImageReaderType::Pointer fixedImageReader = FixedImageReaderType::New();
    MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

    fixedImageReader->SetFileName( fixedVolume );
    movingImageReader->SetFileName( movingVolume );

    // print out which images are being registered
    std::cout << "Fixed: " << fixedVolume << std::endl;
    std::cout << "Moving: " << movingVolume << std::endl;

    /*
    // assign images to pointers
    FixedImageType::Pointer fixedImage = FixedImageType::New();
    fixedImage = fixedImageReader->GetOutput();

    MovingImageType::Pointer movingImage = MovingImageType::New();
    movingImage = movingImageReader->GetOutput();
    */

    fixedImageReader->Update();
    movingImageReader->Update();

    // perform a rigid registration
    //std::cout << std::endl << "Begin Rigid Registraiton" << std::endl;

    // instantiate the registration components
    typedef itk::VersorRigid3DTransform<double>            VersorTransformType;
    typedef itk::VersorRigid3DTransformOptimizer           VersorOptimizerType;
    typedef itk::LinearInterpolateImageFunction<MovingImageType, double>                  InterpolateType;
    typedef itk::MattesMutualInformationImageToImageMetric<FixedImageType, MovingImageType>       MetricType;
    typedef itk::ImageRegistrationMethod<FixedImageType, MovingImageType>                         RegistrationType;

    MetricType::Pointer metric = MetricType::New();
    VersorOptimizerType::Pointer versorOptimizer = VersorOptimizerType::New();
    InterpolateType::Pointer interpolator = InterpolateType::New();
    RegistrationType::Pointer registration = RegistrationType::New();

    // set parameters of the registration
    registration->SetMetric( metric );
    registration->SetOptimizer( versorOptimizer );
    registration->SetInterpolator( interpolator );

    // set image parameters
    registration->SetFixedImage( fixedImageReader->GetOutput() );
    registration->SetMovingImage( movingImageReader->GetOutput() );
    registration->SetFixedImageRegion( fixedImageReader->GetOutput()->GetBufferedRegion() );

    /*
     * USED WITH ITK V4 REGISTRATION FRAMEWORK
    // set levels of heirarchy for multi-resolution registration
    const unsigned int numberOfLevels = 3;
    RegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
    shrinkFactorsPerLevel.SetSize( 3 );
    shrinkFactorsPerLevel[0] = 4;
    shrinkFactorsPerLevel[1] = 2;
    shrinkFactorsPerLevel[2] = 1;
    // set the corresponding sigma at each level
    RegistrationType::SmoothingSigmasArrayType smoothingSigmas;
    smoothingSigmas.SetSize( 1 );
    smoothingSigmas[0] = 3;

    registration->SetNumberOfLevels( numberOfLevels );
    registration->SetSmoothingSigmasPerLevel( smoothingSigmas );
    registration->SetShrinkFactorsPerLevel( shrinkFactorsPerLevel );

    VersorTransformType::Pointer versorTransform = const_cast<TransformType *>( registration->GetOutput()->Get() );
    */

    // create the initialization of the two images
    typedef itk::CenteredTransformInitializer<VersorTransformType, FixedImageType, MovingImageType> TransformInitializerType;
    TransformInitializerType::Pointer initializer = TransformInitializerType::New();

    // get transform from registration method
    VersorTransformType::Pointer versorTransform = VersorTransformType::New();
    registration->SetTransform( versorTransform );
    // attach it to the initializer
    initializer->SetTransform( versorTransform );
    initializer->SetFixedImage( fixedImageReader->GetOutput() );
    initializer->SetMovingImage( movingImageReader->GetOutput() );

    // align via the center of geometry of the two images
    initializer->GeometryOn();
    // computation of center and translation passed to transform
    initializer->InitializeTransform();

    // finish setting up the transform (rotation)
    typedef VersorTransformType::VersorType VersorType;
    typedef VersorType::VectorType          VectorType;
    VersorType rotation;
    VectorType axis;
    axis[0] = 0.0;
    axis[1] = 0.0;
    axis[2] = 1.0;
    const double angle = 0;
    rotation.Set( axis, angle );
    versorTransform->SetRotation( rotation );

    // pass in initial parameters to registration
    registration->SetInitialTransformParameters( versorTransform->GetParameters() );
    //std::cout << "    Initialization Complete" << std::endl;

    // set up the metric to allow for masks to be used as well
    unsigned int numberOfBins = 24;
    const unsigned int numberOfSamples = 10000;
    metric->SetNumberOfHistogramBins( numberOfBins );
    metric->SetNumberOfSpatialSamples( numberOfSamples );
    //metric->SetUseMovingImageGradientFilter( false );
    //metric->SetUseFixedImageGradientFilter( false );
    //metric->SetUseFixedSampledPointSet( false );

    // read in image masks
    typedef unsigned char                           MaskPixelType;
    typedef itk::Image<MaskPixelType, Dimension>    FixedImageMaskType;
    typedef itk::Image<MaskPixelType, Dimension>    MovingImageMaskType;

    typedef itk::ImageFileReader<FixedImageMaskType>    FixedImageMaskReaderType;
    typedef itk::ImageFileReader<MovingImageMaskType>   MovingImageMaskReaderType;

    FixedImageMaskReaderType::Pointer fixedMaskReader = FixedImageMaskReaderType::New();
    MovingImageMaskReaderType::Pointer movingMaskReader = MovingImageMaskReaderType::New();

    // create mask objects
    typedef itk::ImageMaskSpatialObject<Dimension>  FixedMaskType;
    typedef itk::ImageMaskSpatialObject<Dimension>  MovingMaskType;

    FixedMaskType::Pointer fixedMask = FixedMaskType::New();
    MovingMaskType::Pointer movingMask = MovingMaskType::New();

    // insert into mask objects
    if( mask == "yes")
    {
        fixedMaskReader->SetFileName( fixedMaskVolume );
        movingMaskReader->SetFileName( movingMaskVolume );

        fixedMaskReader->Update();
        movingMaskReader->Update();

        fixedMask->SetImage( fixedMaskReader->GetOutput() );
        movingMask->SetImage( movingMaskReader->GetOutput() );

        fixedMask->Update();
        movingMask->Update();

        metric->SetFixedImageMask( fixedMask );
        metric->SetMovingImageMask( movingMask );

        // output mask file names
        std::cout << "Fixed Mask: " << fixedMaskVolume << std::endl;
        std::cout << "Moving Mask: " << fixedMaskVolume << std::endl;
    }

    // pass parameters into the optimizer
    typedef VersorOptimizerType::ScalesType VersorOptimizerScalesType;
    VersorOptimizerScalesType versorOptimizerScales( versorTransform->GetNumberOfParameters() );
    const double translationScale = 1.0/1000.0;
    versorOptimizerScales[0] = 1.0;
    versorOptimizerScales[1] = 1.0;
    versorOptimizerScales[2] = 1.0;
    versorOptimizerScales[3] = translationScale;
    versorOptimizerScales[4] = translationScale;
    versorOptimizerScales[5] = translationScale;

    versorOptimizer->SetScales( versorOptimizerScales );
    versorOptimizer->SetMaximumStepLength( 0.2 );
    versorOptimizer->SetMinimumStepLength( 0.0001 );
    versorOptimizer->SetNumberOfIterations( 1500 );

    /*
     * USED WITH ITK V4 REGISTRATION FRAMEWORK
    versorOptimizer->SetMaximumStepSizeInPhysicalUnits( 0.2 );
    versorOptimizer->SetNumberOfIterations( 200 );
    versorOptimizer->SetLearningRate( 0.2 );
    versorOptimizer->SetConvergenceWindowSize( 5 );
    versorOptimizer->SetMinimumConvergenceValue( 1e-3 );
    versorOptimizer->SetReturnBestParametersAndValue( true );
    */

    //std::cout << "    Perform Registration" << std::endl;

    // perform registration and time it
    try
    {
        timer.Start( "Rigid Registration" );
        registration->Update();
        timer.Stop( "Rigid Registration" );

        const VersorOptimizerType::ConstPointer outputOptimizer = dynamic_cast<const VersorOptimizerType *>( registration->GetOptimizer() );
        std::cout << std::endl << "    OptimizerStopCondition: " << outputOptimizer->GetStopConditionDescription() << std::endl;
    } catch( itk::ExceptionObject & err ){
        std::cerr << "ExceptionObject caught!" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    /*
    // output final parameters to the screen
    VersorOptimizerType::ParametersType finalParameters = registration->GetLastTransformParameters();
    std::cout << std::endl << "    Final Parameters" << std::endl;
    std::cout << "      Versor Matrix: " << versorTransform->GetMatrix() << std::endl;
    std::cout << "      Translation values: " << versorTransform->GetTranslation() << std::endl;
    std::cout << "      Iterations: " << versorOptimizer->GetCurrentIteration() << std::endl;
    std::cout << "      MetricValue: " << versorOptimizer->GetValue() << std::endl;

    std::cout << std::endl << "    Write results to a file" << std::endl;

    // apply the final transform to an image
    typedef itk::ResampleImageFilter<MovingImageType, FixedImageType> ResampleFilterType;
    ResampleFilterType::Pointer resampler = ResampleFilterType::New();

    // obtain the final transform
    VersorTransformType::Pointer finalVersorTransform = VersorTransformType::New();
    finalVersorTransform->SetCenter( versorTransform->GetCenter() );
    finalVersorTransform->SetParameters( finalParameters );
    finalVersorTransform->SetFixedParameters( versorTransform->GetFixedParameters() );

    // input final transform into resampler and other parameters
    resampler->SetTransform( finalVersorTransform );
    resampler->SetInput( movingImageReader->GetOutput() );

    // obtain parameters from fixed image
    FixedImageType::Pointer fixedImage = fixedImageReader->GetOutput();
    resampler->SetSize( fixedImage->GetLargestPossibleRegion().GetSize() );
    resampler->SetOutputOrigin( fixedImage->GetOrigin() );
    resampler->SetOutputSpacing( fixedImage->GetSpacing() );
    resampler->SetOutputDirection( fixedImage->GetDirection() );
    resampler->SetDefaultPixelValue( 150 );

    // write final image out to a file
    typedef itk::Image<PixelType, Dimension>      OutputImageType;
    typedef itk::ImageFileWriter<OutputImageType> WriterType;

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( argv[5] );

    writer->SetInput( resampler->GetOutput() );
    writer->Update();
    */

    // write the affine registration using the rigid final transform for initialization
    //std::cout << std::endl << "Begin Affine Registration" << std::endl;

    // create affine transform
    typedef itk::AffineTransform<double, Dimension> AffineTransformType;
    AffineTransformType::Pointer affineTransform = AffineTransformType::New();

    // place final rigid parameters into affine transform
    affineTransform->SetCenter( versorTransform->GetCenter() );
    affineTransform->SetTranslation( versorTransform->GetTranslation() );
    affineTransform->SetMatrix( versorTransform->GetMatrix() );

    // reinitialize registration with new affine parameters
    registration->SetTransform( affineTransform );
    registration->SetInitialTransformParameters( affineTransform->GetParameters() );

    //std::cout << "    Initialization Complete" << std::endl;

    // set up new optimizer
    typedef itk::RegularStepGradientDescentOptimizer AffineOptimizerType;
    AffineOptimizerType::Pointer affineOptimizer = AffineOptimizerType::New();

    // input into registration framework
    registration->SetOptimizer( affineOptimizer );

    // input required parameters
    typedef AffineOptimizerType::ScalesType AffineOptimizerScalesType;
    AffineOptimizerScalesType affineOptimizerScales( affineTransform->GetNumberOfParameters() );

    // 12 affine parameters
    affineOptimizerScales[0] = 1.0;
    affineOptimizerScales[1] = 1.0;
    affineOptimizerScales[2] = 1.0;
    affineOptimizerScales[3] = 1.0;
    affineOptimizerScales[4] = 1.0;
    affineOptimizerScales[5] = 1.0;
    affineOptimizerScales[6] = 1.0;
    affineOptimizerScales[7] = 1.0;
    affineOptimizerScales[8] = 1.0;

    affineOptimizerScales[9] = translationScale;
    affineOptimizerScales[10] = translationScale;
    affineOptimizerScales[11] = translationScale;

    affineOptimizer->SetScales( affineOptimizerScales );
    affineOptimizer->SetMaximumStepLength( 0.2 );
    affineOptimizer->SetMinimumStepLength( 0.0001 );
    affineOptimizer->SetNumberOfIterations( 1000 );

    // change the number of spatial samples in the metric to account for the larger
    // number of parameters in the affine transformation
    metric->SetNumberOfSpatialSamples( 50000 );

    //std::cout << "    Perform Registration" << std::endl;

    // perform the affine registration
    try
    {
        timer.Start( "Affine Registration" );
        registration->Update();
        timer.Stop( "Affine Registration" );

        const AffineOptimizerType::ConstPointer outputOptimizer = dynamic_cast<const AffineOptimizerType *>( registration->GetOptimizer() );
        std::cout << std::endl << "    OptimizerStopCondition: " << outputOptimizer->GetStopConditionDescription() << std::endl;
    } catch( itk::ExceptionObject & err ){
        std::cerr << "ExceptionObject caught!" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    //std::cout << "    Registration Process Complete!" << std::endl;
    //std::cout << std::endl << "Writing results to a file" << std::endl;
    timer.Start("Post-processing");

    // apply the final transform to an image
    typedef itk::ResampleImageFilter<MovingImageType, FixedImageType> ResampleFilterType;
    ResampleFilterType::Pointer resampler = ResampleFilterType::New();

    // obtain the final transform
    AffineOptimizerType::ParametersType finalAffineParameters = registration->GetLastTransformParameters();
    AffineTransformType::Pointer finalAffineTransform = AffineTransformType::New();
    finalAffineTransform->SetCenter( affineTransform->GetCenter() );
    finalAffineTransform->SetParameters( finalAffineParameters );
    finalAffineTransform->SetFixedParameters( affineTransform->GetFixedParameters() );

    // input final transform into resampler and other parameters
    resampler->SetTransform( finalAffineTransform );
    resampler->SetInput( movingImageReader->GetOutput() );

    // obtain parameters from fixed image
    FixedImageType::Pointer fixedImage = fixedImageReader->GetOutput();
    resampler->SetSize( fixedImage->GetLargestPossibleRegion().GetSize() );
    resampler->SetOutputOrigin( fixedImage->GetOrigin() );
    resampler->SetOutputSpacing( fixedImage->GetSpacing() );
    resampler->SetOutputDirection( fixedImage->GetDirection() );
    resampler->SetDefaultPixelValue( 150 );

    // write final image out to a file
    typedef itk::Image<PixelType, Dimension>      OutputImageType;
    typedef itk::ImageFileWriter<OutputImageType> WriterType;

    WriterType::Pointer writer = WriterType::New();

    writer->SetFileName( outputVolume );
    std::cout << "Output File: " << outputVolume << std::endl;

    writer->SetInput( resampler->GetOutput() );
    writer->Update();

    timer.Stop("Post-processing");

    // stop timer and send out information
    timer.Stop("Complete Code");

    // output information to screen
    std::cout << std::endl;
    std::cout << "Affine Matrix: " << std::endl << affineTransform->GetMatrix() << std::endl;
    std::cout << "Affine Translation: " << affineTransform->GetTranslation() << std::endl;
    std::cout << "Iterations: " << affineOptimizer->GetCurrentIteration() << std::endl;
    std::cout << "MetricValue: " << affineOptimizer->GetValue() << std::endl;

    timer.Report( std::cout );
    std::cout << std::endl;

    return EXIT_SUCCESS;
}
