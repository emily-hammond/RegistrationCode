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


int main( int argc, char * argv[] )
{
  const unsigned int      Dimension = 3;
  typedef unsigned short  ShortPixelType;
  typedef float		  FloatPixelType;

  typedef itk::Image< ShortPixelType, Dimension >	FixedImageType;
  typedef itk::Image< ShortPixelType, Dimension >	MovingImageType;
  typedef itk::Image< FloatPixelType, Dimension >	InternalImageType;

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

  // set up pipeline
  writer->SetInput( fixedReader->GetOutput() );

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
