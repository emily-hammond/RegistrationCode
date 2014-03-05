/*
  Author: Emily Hammond
  Date: 3/4/2014

  This program follows the Registration/ImageRegistration4.cxx example
  from the ITK libraries. Source Code can be found at http://www.itk.org/Doxygen/html/
  Registration_2ImageRegistration4_8cxx-example.html.

*/

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegistrationMethod.h"
#include "itkTranslationTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkCheckerboardImageFilter.h"


int main( int argc, char * argv[] )
{
  const unsigned int      Dimension = 2;
  typedef unsigned short  ShortPixelType;

  // set up data objects
  typedef itk::Image< ShortPixelType, Dimension >	FixedImageType;
  typedef itk::Image< ShortPixelType, Dimension >	MovingImageType;

  typedef itk::ImageFileReader< FixedImageType >     FixedImageReaderType;
  typedef itk::ImageFileReader< MovingImageType >    MovingImageReaderType;

  // instantiate reader objects
  FixedImageReaderType::Pointer fixedReader = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingReader = MovingImageReaderType::New();

  // set parameters of reader objects
  fixedReader->SetFileName( argv[1] );
  movingReader->SetFileName( argv[2] );
  
  // set up registration component objects
  typedef itk::ImageRegistrationMethod< FixedImageType, MovingImageType > RegistrationType;
  typedef itk::TranslationTransform< double, Dimension >		  TransformType;
  typedef itk::LinearInterpolateImageFunction< MovingImageType, double >  InteroplatorType;
  typedef itk::MattesMutualInformationImageToImageMetric< FixedImageType, MovingImageType > MetricType;
  typedef itk::RegularStepGradientDescentOptimizer			  OptimizerType;

  // instantiate registration objects
  RegistrationType::Pointer 	registration = RegistrationType::New();
  TransformType::Pointer 	transform = TransformType::New();
  InterpolatorType::Pointer 	interoplator = InterpolatorType::New();
  MetricType::Pointer 		metric = MetricType::New();
  OptimizerType::Pointer 	optimizer = OptimizerType::New();

  // put in parameters for registrator
  registration->SetTransform( transform );
  registration->SetInterpolator( interpolator );
  registration->SetMetric( metric );
  registration->SetOptimizer( optimizer );

  // put in parameters for MI metric - number of bins and number of samples
  metric->SetNumberOfHistogramBins( 24 );
  metric->SetNumberOfSpatialSamples( 10000 );

  // initialize the transform
  typedef RegistrationType::ParametersType ParametersType;
  //   create a variable of the type of parameters in the transform and initialize it the number of parameters objects
  ParametersType initialParameters( transform->GetNumberOfParameters() );
  //   set an initial offset in the x (0) and y (0) directions - dimensions in mm
  initialParameters[0] = 0.0;
  initialParameters[1] = 0.0;
  //   input into registrator
  registration->SetInitialTransformParameters( initialParameters );

  // set up the optimizer and put in its parameters
  optimizer->MinimizeOn();	// set optimizer up for minimization
  optimizer->SetMaximumStepLength( 2.00 );
  optimizer->GetMinimumStepLength( 0.01 );
  optimizer->SetNumberOfIterations( 200 );
  optimizer->SetRelaxationFactor( 0.8 );	// controls for the rate of step size reduction
  
  // set up pipeline to perform registration
  // set up registration inputs
  registration->SetFixedImage( fixedReader->GetOutput() );
  registration->SetMovingImage( movingReader->GetOutput() );
  registration->SetFixedImageRegion( fixedReader->GetOutput()->GetBufferedRegion() );

  // perform the registration
  try
  {
      registration->Update();
      std::cout << "OptimizerStepCondition: " << registration->GetOptimizer()->GetStopConditionDescription() << std::endl;
  }
  catch( itk::ExceptionObject & err )
  {
      std::cerr << "ExceptionObject caught!\n;
      std::cerr << err << std::endl;
      return EXIT_FAILURE;
  }

  // obtain and print out the final results
  ParametersType finalParameters = registration->GetLastTransformParameters();
  std::cout << "\nResults = \n";
  std::cout << " Translation X = " << finalParameters[0] << std::endl;
  std::cout << " Translation Y = " << finalParameters[1] << std::endl;
  std::cout << " Iterations = " << optimizer->GetCurrentIterations() << std::endl;
  std::cout << " Metric Value = " << optimizer->GetValue() << std::endl;
  std::cout << " Stop Condition = " << optimizer->GetStopCondition() << std::endl;

  // apply the resulting transform to the moving image using the ResampleImageFilter
  typedef itk::ResampleImageFilter< MovingImageType, FixedImageType > ResampleFilterType;
  // initialized a new transform data object
  TransformType::Pointer finalTransform = TransformType::New();
  //   set the parameters to those of the final result
  finalTransform->SetParameters( finalParameters );
  finalTransform->SetFixedParameters( transform->GetFixedParameters() );

  // instantiate a resample object and a new original fixed image
  ResampleImageFilter::Pointer resample = ResampleImageFilter::New();
  FixedImageType::Pointer fixedImage = fixedReader->GetOutput();
  // set up parameters to be those of the transform and identical to those of the fixed image
  resample->SetTransform( finalTransform );
  resample->SetSize( fixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin( fixedImage->GetOrigin() );
  resample->SetOutputSpacing( fixedImage->GetSpacing() );
  resample->SetDefaultPixelValue( 100 );
  
  // instantiate an output image to write to
  typedef unsigned char  CharPixelType;
  typedef itk::Image< CharPixelType, Dimension > OutputImageType;
  typedef itk::ImageFileWriter< OutputImageType > WriterType;

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[3] );

  // set up to cast the image from a short to a char
  typedef itk::CastImageFilter< ShortPixelType, CharImageType > CastFilterType;
  CastFilterType::Pointer caster = CastFilterType::New();

  // set up pipeline to casting and writing the image
  resample->SetInput( movingReader->GetOutput() );
  caster->SetInput( resample->GetOutput() );
  writer->SetInput( caster->GetOutput() );

  try
  {
      writer->Update();
  }
  catch( itk::ExceptionObject & err )
  {
      std::cerr << "ExceptionObject caught !" << std::endl;
      std::cerr << err << std::endl;
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

}
