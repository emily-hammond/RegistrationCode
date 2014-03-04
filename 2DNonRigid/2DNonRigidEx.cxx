/*
  Author: Emily Hammond
  Date: 3/4/2014

  This program follows the Registration/ImageRegistration13.cxx example
  from the ITK libraries. Source Code can be found at http://www.itk.org
  /Doxygen/html/Registration_2ImageRegistration13_8cxx-example.html#_a15.

*/

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


int main( int argc, char * argv[] )
{
  const unsigned int      Dimension = 3;
  typedef unsigned short  ShortPixelType;
  typedef float		 	  FloatPixelType;

  typedef itk::Image< ShortPixelType, Dimension >	FixedImageType;
  typedef itk::Image< ShortPixelType, Dimension >	MovingImageType;
  typedef itk::Image< FloatPixelType, Dimension >	InternalImageType;

  typedef itk::ImageFileReader< ShortPixelType >    FixedImageReaderType;
  typedef itk::ImageFileReader< ShortPixelType >    MovingImageReaderType;

  typedef itk::ImageFileWriter< ShortPixelType >    WriterType;

  // instantiate reader/writer objects
  FixedImageReaderType::Pointer fixedImageReader = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

  WriterType::Pointer writer = WriterType::New();

  // set parameters of reader/writer objects
  fixedImageReader->SetFilename( argv[1] );
  movingImageReader->SetFilename( argv[2] );

  writer->SetFilename( argv[3] );

  // set up pipeline
  writer->SetInput( fixedImageReader->GetOutput() );

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
