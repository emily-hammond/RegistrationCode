/*
INSERT COMMENTS HERE
*/

// include files
#include "ReadWriteFunctions.hxx"
#include "itkRegistrationFramework.h"
#include "itkInitializationFilter.h"
#include "itkValidationFilter.h"

int main( int argc, char * argv[] )
{
	//desired inputs
	char * fixedImageFilename = argv[1];
	char * movingImageFilename = argv[2];
	//char * initialTransformFilename = argv[3];

	// instantiate image type
	typedef itk::Image< unsigned short, 3 >	ImageType;
	typedef itk::Image< unsigned int, 3 >	LabelMapType;
	typedef itk::ScaleVersor3DTransform< double >	TransformType;

	// read in fixed and moving images
	ImageType::Pointer fixedImage = ReadInImage< ImageType >( fixedImageFilename );
	ImageType::Pointer movingImage = ReadInImage< ImageType >( movingImageFilename );
	//ImageType::Pointer fixedLabelMap = ReadInImage< LabelMapType >( fixedLabelMapFilename );
	//ImageType::Pointer movingLabelMap = ReadInImage< LabelMapType >( movingLabelMapFilename );
	//TransformType::Pointer initialTransform = ReadInTransform< TransformType >( initialTransformFilename );

	std::cout << "\nFixed image: " << fixedImageFilename << std::endl;
	std::cout << "Moving image: " << movingImageFilename << std::endl;

	// initialization
	std::cout << "\n*********************************************" << std::endl;
	std::cout << "              INITIALIZATION                 " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	itk::InitializationFilter::Pointer initialize = itk::InitializationFilter::New();
	initialize->SetImages( fixedImage, movingImage );
	//initialize->ObserveOn();
	initialize->CenteredOnGeometryOn();
	initialize->MetricAlignmentOn( 0 );
	initialize->MetricAlignmentOn( 1 );
	initialize->MetricAlignmentOn( 2 );
	initialize->PerformInitialization();

	// test functionality of itkRegistrationFramework.h
	std::cout << "\n*********************************************" << std::endl;
	std::cout << "               REGISTRATION                  " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	itk::RegistrationFramework::Pointer registration = itk::RegistrationFramework::New();
	registration->SetImages( fixedImage, movingImage );
	registration->SetInitialTransform( initialize->GetOutput() );
	//registration->ObserveOn();
	registration->PerformRegistration();

	std::cout << "\n*********************************************" << std::endl;
	std::cout << "                VALIDATION                  " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	itk::ValidationFilter::Pointer validation = itk::ValidationFilter::New();
	//validation->LabelOverlapMeasures( fixedLabelMap, movingLabelMap )
	
	std::cout << "\nFinished\n" << std::endl;

	return EXIT_SUCCESS;
}
