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

  typedef itk::ImageFileWriter< FixedImageType >    WriterType;

  // instantiate reader/writer objects
  FixedImageReaderType::Pointer fixedReader = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingReader = MovingImageReaderType::New();

  WriterType::Pointer writer = WriterType::New();

  // set parameters of reader/writer objects
  fixedReader->SetFileName( argv[1] );
  movingReader->SetFileName( argv[2] );
  writer->SetFileName( argv[3] );
  
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

  
  
  // set up pipeline

  // set up registration inputs
  registration->SetFixedImage( fixedReader->GetOutput() );
  registration->SetMovingImage( movingReader->GetOutput() );
  registration->SetFixedImageRegion( fixedReader->GetOutput()->GetBufferedRegion() );

  writer->SetInput( registration->GetOutput() );

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
