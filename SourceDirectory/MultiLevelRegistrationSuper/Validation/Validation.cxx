/*
 * Emily Hammond
 * 2016-03-16
 *
 * The goal of this code is to determine the overlap measures between two
 * label masks and a transform
 */

#include "C:\Slicer\RegistrationCode\SourceDirectory\MultiLevelRegistrationSuper\ReadWriteFunctions.hxx"
#include "C:\Slicer\RegistrationCode\SourceDirectory\MultiLevelRegistrationSuper\itkValidationFilter.h"
#include "C:\Slicer\RegistrationCode\SourceDirectory\MultiLevelRegistrationSuper\itkManageTransformsFilter.h"
#include <time.h>

// ***********************Main function********************************
void Timestamp();
int main( int argc, char * argv[] )
{
	std::cout << "-----------------------------------------------------------------------------" << std::endl;
	std::cout << "      MULTI-LEVEL REGISTRATION VALIDATION ";
	Timestamp();
	std::cout << "-----------------------------------------------------------------------------" << std::endl;
	std::cout << std::endl;

	/*std::cout << "Number of inputs: " << argc << std::endl;
	for( int i = 0; i < argc; ++i )
	{
		std::cout << i << ": " << argv[i] << std::endl;
	}*/

	// inputs
	char * fixedImageFilename = '\0';
	char * fixedMaskFilename = '\0';
	char * movingImageFilename = '\0';
	char * movingMaskFilename = '\0';
	char * initialTransformFilename = '\0';
	if( argc < 6 )
	{
		std::cout << ".exe fixedImage fixedMask movingImage movingMask initialTransform [level1Transform]";
		std::cout << "      [level2Transform level2ROI] [level3Transform level3ROI] [initialFixedTransform referenceImage]" << std::endl;;
		return EXIT_FAILURE;
	}
	else
	{
		fixedImageFilename = argv[1];
		fixedMaskFilename = argv[2];
		movingImageFilename = argv[3];
		movingMaskFilename = argv[4];
		initialTransformFilename = argv[5];
	}
	char * level1TransformFilename = '\0';
	char * level2TransformFilename = '\0';
	char * level3TransformFilename = '\0';
	char * level2ROIFilename = '\0';
	char * level3ROIFilename = '\0';
	char * initialFixedTransformFilename = '\0';
	char * referenceImageFilename = '\0';
	if( argc > 6 && strcmp(argv[6],"[]") != 0 )
	{
		level1TransformFilename = argv[6];
	}
	if( argc > 7 && strcmp(argv[7],"[]") != 0 )
	{
		level2TransformFilename = argv[7];
		level2ROIFilename = argv[8];
	}
	if( argc > 9 && strcmp(argv[9],"[]") != 0 )
	{
		level3TransformFilename = argv[9];
		level3ROIFilename = argv[10];
	}
	if( argc > 11 && strcmp(argv[11],"[]") != 0 )
	{
		initialFixedTransformFilename = argv[11];
		referenceImageFilename = argv[12];
	}
	if( argc > 13 )
	{
		std::cout << "Too many inputs." << std::endl;
		return EXIT_FAILURE;
	}

	typedef itk::Image< short, 3 >	ImageType;
	typedef itk::ScaleVersor3DTransform< double >	TransformType;

	std::cout << "\nFixed image           : " << fixedImageFilename << std::endl;
	std::cout << "Fixed validation mask : " << fixedMaskFilename << std::endl;
	std::cout << "Moving image          : " << movingImageFilename << std::endl;
	std::cout << "Moving validation mask: " << movingMaskFilename << std::endl;

	// check inputs
	itk::ManageTransformsFilter::Pointer transforms = itk::ManageTransformsFilter::New();
	ImageType::Pointer fixedImage = ImageType::New();
	ImageType::Pointer fixedMask = ImageType::New();

	// apply fixed initial transform if given
	if( argc > 11 && strcmp(argv[11],"[]") != 0 )
	{
		// apply transform to fixed image
		TransformType::Pointer initialFixedTransform = ReadInTransform< TransformType >( initialFixedTransformFilename );
		ImageType::Pointer fixedImageTemp = ReadInImage< ImageType >( fixedImageFilename );
		transforms->SetFixedImage( ReadInImage< ImageType >( referenceImageFilename ) );
		fixedImage = transforms->ResampleImage( fixedImageTemp, initialFixedTransform );

		// apply transform to fixed validation mask
		transforms->NearestNeighborInterpolateOn();
		ImageType::Pointer fixedMaskTemp = ReadInImage< ImageType >( fixedMaskFilename );
		fixedMask = transforms->ResampleImage( fixedMaskTemp, initialFixedTransform );
		transforms->NearestNeighborInterpolateOff();

		std::cout << "Initial transform applied to fixed image." << std::endl;
	}
	else
	{
		fixedImage = ReadInImage< ImageType >( fixedImageFilename );
		fixedMask = ReadInImage< ImageType >( fixedMaskFilename );
	}

	// read in necessary images
	ImageType::Pointer movingImage = ReadInImage< ImageType >( movingImageFilename );
	ImageType::Pointer movingMask = ReadInImage< ImageType >( movingMaskFilename );
	TransformType::Pointer initialTransform = ReadInTransform< TransformType >( initialTransformFilename );
	
	// transforms filter
	transforms->SetFixedImage( fixedImage );
	transforms->SetFixedLabelMap( fixedMask );
	transforms->SetMovingImage( movingImage );
	transforms->SetMovingLabelMap( movingMask );

	std::cout << "\nTransform: " << initialTransformFilename << std::endl;
	std::cout << initialTransform << std::endl;

	// instantiate validation filter
	itk::ValidationFilter::Pointer validation = itk::ValidationFilter::New();
	validation->SetImage1( fixedImage );
	validation->SetLabelMap1( fixedMask );
	validation->SetImage2( transforms->ResampleImage( movingImage, initialTransform ) );
	transforms->NearestNeighborInterpolateOn();
	validation->SetLabelMap2( transforms->ResampleImage( movingMask, initialTransform ) );
	transforms->NearestNeighborInterpolateOff();
	validation->LabelOverlapMeasuresOn();
	try
	{
		validation->Update();
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}

	if( argc > 6 && strcmp(argv[6],"[]") != 0 )
	{
		TransformType::Pointer level1Transform = ReadInTransform< TransformType >( level1TransformFilename );
		std::cout << "\nTransform: " << level1TransformFilename << std::endl;
		std::cout << level1Transform << std::endl;

		validation->SetImage2( transforms->ResampleImage( movingImage, level1Transform ) );
		transforms->NearestNeighborInterpolateOn();
		validation->SetLabelMap2( transforms->ResampleImage( movingMask, level1Transform ) );
		transforms->NearestNeighborInterpolateOff();
		validation->LabelOverlapMeasuresOn();
		try
		{
			validation->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}
	}

	if( argc > 7 && strcmp(argv[7],"[]") != 0 )
	{
		TransformType::Pointer level2Transform = ReadInTransform< TransformType >( level2TransformFilename );

		// transforms filter
		itk::ManageTransformsFilter::Pointer transforms2 = itk::ManageTransformsFilter::New();
		transforms2->SetFixedImage( fixedImage );
		transforms2->SetFixedLabelMap( fixedMask );
		transforms2->SetMovingImage( movingImage );
		transforms2->SetMovingLabelMap( movingMask );

		transforms2->AddTransform( level2Transform );
		transforms2->SetROIFilename( level2ROIFilename );
		std::cout << "ROI2: " << level2ROIFilename << std::endl;
		transforms2->ResampleImageOn();
		transforms2->CropImageOn();
		try
		{
			transforms2->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}

		std::cout << "\nTransform: " << level2TransformFilename << std::endl;
		std::cout << level2Transform << std::endl;

		validation->SetImage1( transforms2->GetFixedCroppedImage() );
		validation->SetLabelMap1( transforms2->GetFixedCroppedLabelMap() );
		validation->SetImage2( transforms2->GetMovingCroppedImage() );
		validation->SetLabelMap2( transforms2->GetMovingCroppedLabelMap() );
		validation->LabelOverlapMeasuresOn();
		try
		{
			validation->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}
	}

	if( argc > 9 && strcmp(argv[9],"[]") != 0 )
	{
		TransformType::Pointer level3Transform = ReadInTransform< TransformType >( level3TransformFilename );

		// transforms filter
		itk::ManageTransformsFilter::Pointer transforms3 = itk::ManageTransformsFilter::New();
		transforms3->SetFixedImage( fixedImage );
		transforms3->SetFixedLabelMap( fixedMask );
		transforms3->SetMovingImage( movingImage );
		transforms3->SetMovingLabelMap( movingMask );

		transforms3->AddTransform( level3Transform );
		transforms3->SetROIFilename( level3ROIFilename );
		transforms3->ResampleImageOn();
		transforms3->CropImageOn();
		try
		{
			transforms3->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}

		std::cout << "\nTransform: " << level3TransformFilename << std::endl;
		std::cout << level3Transform << std::endl;

		validation->SetImage1( transforms3->GetFixedCroppedImage() );
		validation->SetLabelMap1( transforms3->GetFixedCroppedLabelMap() );
		validation->SetImage2( transforms3->GetMovingCroppedImage() );
		validation->SetLabelMap2( transforms3->GetMovingCroppedLabelMap() );
		validation->LabelOverlapMeasuresOn();
		try
		{
			validation->Update();
		}
		catch(itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}
	}

    return EXIT_SUCCESS;
}

void Timestamp()
{
	time_t ltime;
	ltime = time( NULL );
	printf("%s",asctime( localtime( &ltime ) ) );
	return;
}