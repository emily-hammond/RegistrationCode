/*
 * Emily Hammond
 * 2016-03-16
 *
 * The goal of this code is to determine the overlap measures between two
 * label masks and a transform
 */

#include "C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SourceDirectory\MultiLevelRegistration\ReadWriteFunctions.hxx"
#include "C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SourceDirectory\MultiLevelRegistration\itkValidationFilter.h"
#include "C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SourceDirectory\MultiLevelRegistration\itkManageTransformsFilter.h"

// ***********************Main function********************************
int main( int argc, char * argv[] )
{
	// inputs
	char * fixedImageFilename = argv[1];
	char * fixedMaskFilename = argv[2];
	char * movingImageFilename = argv[3];
	char * movingMaskFilename = argv[4];
	char * initialTransformFilename = argv[5];
	char * level1TransformFilename = '\0';
	char * level2TransformFilename = '\0';
	char * level3TransformFilename = '\0';
	char * level2ROIFilename = '\0';
	char * level3ROIFilename = '\0';
	if( argc > 6 )
	{
		level1TransformFilename = argv[6];
	}
	if( argc > 7 )
	{
		level2TransformFilename = argv[7];
		level2ROIFilename = argv[8];
	}
	if( argc > 9 )
	{
		level3TransformFilename = argv[8];
		level3ROIFilename = argv[9];
	}

	typedef itk::Image< short, 3 >	ImageType;
	typedef itk::ScaleVersor3DTransform< double >	TransformType;

	std::cout << "\nFixed image           : " << fixedImageFilename << std::endl;
	std::cout << "Fixed validation mask : " << fixedMaskFilename << std::endl;
	std::cout << "Moving image          : " << movingImageFilename << std::endl;
	std::cout << "Moving validation mask: " << movingMaskFilename << std::endl;

	// read in necessary images
	ImageType::Pointer fixedImage = ReadInImage< ImageType >( fixedImageFilename );
	ImageType::Pointer movingImage = ReadInImage< ImageType >( movingImageFilename );
	ImageType::Pointer fixedMask = ReadInImage< ImageType >( fixedMaskFilename );
	ImageType::Pointer movingMask = ReadInImage< ImageType >( movingMaskFilename );
	TransformType::Pointer initialTransform = ReadInTransform< TransformType >( initialTransformFilename );
	
	// transforms filter
	itk::ManageTransformsFilter::Pointer transforms = itk::ManageTransformsFilter::New();
	transforms->SetFixedImage( fixedImage );
	transforms->SetFixedLabelMap( fixedMask );
	transforms->SetMovingImage( movingImage );
	transforms->SetMovingLabelMap( movingMask );

	std::cout << "\nTransform: " << initialTransformFilename << std::endl;

	// instantiate validation filter
	itk::ValidationFilter::Pointer validation = itk::ValidationFilter::New();
	validation->SetImage1( fixedImage );
	validation->SetLabelMap1( fixedMask );
	validation->SetImage2( transforms->ResampleImage( movingImage, initialTransform ) );
	transforms->NearestNeighborInterpolateOn();
	validation->SetLabelMap2( transforms->ResampleImage( movingMask, initialTransform ) );
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

	if( argc > 6 )
	{
		TransformType::Pointer level1Transform = ReadInTransform< TransformType >( level1TransformFilename );
		std::cout << "\nTransform: " << level1TransformFilename << std::endl;

		validation->SetImage2( transforms->ResampleImage( movingImage, level1Transform ) );
		transforms->NearestNeighborInterpolateOn();
		validation->SetLabelMap2( transforms->ResampleImage( movingMask, level1Transform ) );
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

	if( argc > 7 )
	{
		TransformType::Pointer level2Transform = ReadInTransform< TransformType >( level2TransformFilename );
		TransformType::Pointer level1Transform = ReadInTransform< TransformType >( level1TransformFilename );

		// transforms filter
		itk::ManageTransformsFilter::Pointer transforms2 = itk::ManageTransformsFilter::New();
		transforms2->SetFixedImage( fixedImage );
		transforms2->SetFixedLabelMap( fixedMask );
		transforms2->SetMovingImage( movingImage );
		transforms2->SetMovingLabelMap( movingMask );

		transforms2->AddTransform( level1Transform );
		transforms2->CropImageOn();
		transforms2->ResampleImageOn();
		transforms2->SetROIFilename( level2ROIFilename );
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

		validation->SetImage2( transforms->ResampleImage( transforms2->GetMovingCroppedImage(), level2Transform ) );
		transforms->NearestNeighborInterpolateOn();
		validation->SetLabelMap2( transforms->ResampleImage( transforms2->GetMovingCroppedLabelMap(), level2Transform ) );
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

	if( argc > 9 )
	{
		TransformType::Pointer level3Transform = ReadInTransform< TransformType >( level3TransformFilename );
		TransformType::Pointer level2Transform = ReadInTransform< TransformType >( level1TransformFilename );

		// transforms filter
		itk::ManageTransformsFilter::Pointer transforms3 = itk::ManageTransformsFilter::New();
		transforms3->SetFixedImage( fixedImage );
		transforms3->SetFixedLabelMap( fixedMask );
		transforms3->SetMovingImage( movingImage );
		transforms3->SetMovingLabelMap( movingMask );

		transforms3->AddTransform( level2Transform );
		transforms3->CropImageOn();
		transforms3->ResampleImageOn();
		transforms3->SetROIFilename( level3ROIFilename );
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

		validation->SetImage2( transforms->ResampleImage( transforms3->GetMovingCroppedImage(), level3Transform ) );
		transforms->NearestNeighborInterpolateOn();
		validation->SetLabelMap2( transforms->ResampleImage( transforms3->GetMovingCroppedLabelMap(), level3Transform ) );
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