// include files
#include ".\ReadWriteFunctions.hxx"
#include ".\PreprocessingFunctions.hxx"
#include ".\itkRegistrationFramework.h"
#include ".\itkInitializationFilter.h"
#include ".\itkManageTransformsFilter.h"
#include ".\itkValidationFilter.h"

// rescale images
#include "itkRescaleIntensityImageFilter.h"

#include "itkPluginUtilities.h"
#include "Multi-LevelRegistrationCLP.h"

// Use an anonymous namespace to keep class types and function names
// from colliding when module is used as shared object module.  Every
// thing should be in an anonymous namespace except for the module
// entry point, e.g. main()
//
/*
namespace
{

template <typename TPixel>
int DoIt( int argc, char * argv[], TPixel )
{
  PARSE_ARGS;

  return EXIT_SUCCESS;
}

} // end of anonymous namespace
*/

int main( int argc, char * argv[] )
{
  /*PARSE_ARGS;

  itk::ImageIOBase::IOPixelType     pixelType;
  itk::ImageIOBase::IOComponentType componentType;

  try
    {
    itk::GetImageType(inputVolume, pixelType, componentType);

    // This filter handles all types on input, but only produces
    // signed types
    switch( componentType )
      {
      case itk::ImageIOBase::UCHAR:
        return DoIt( argc, argv, static_cast<unsigned char>(0) );
        break;
      case itk::ImageIOBase::CHAR:
        return DoIt( argc, argv, static_cast<signed char>(0) );
        break;
      case itk::ImageIOBase::USHORT:
        return DoIt( argc, argv, static_cast<unsigned short>(0) );
        break;
      case itk::ImageIOBase::SHORT:
        return DoIt( argc, argv, static_cast<short>(0) );
        break;
      case itk::ImageIOBase::UINT:
        return DoIt( argc, argv, static_cast<unsigned int>(0) );
        break;
      case itk::ImageIOBase::INT:
        return DoIt( argc, argv, static_cast<int>(0) );
        break;
      case itk::ImageIOBase::ULONG:
        return DoIt( argc, argv, static_cast<unsigned long>(0) );
        break;
      case itk::ImageIOBase::LONG:
        return DoIt( argc, argv, static_cast<long>(0) );
        break;
      case itk::ImageIOBase::FLOAT:
        return DoIt( argc, argv, static_cast<float>(0) );
        break;
      case itk::ImageIOBase::DOUBLE:
        return DoIt( argc, argv, static_cast<double>(0) );
        break;
      case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
      default:
        std::cerr << "Unknown input image pixel component type: ";
        std::cerr << itk::ImageIOBase::GetComponentTypeAsString( componentType );
        std::cerr << std::endl;
        return EXIT_FAILURE;
        break;
      }
    }

  catch( itk::ExceptionObject & excep )
    {
    std::cerr << argv[0] << ": exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }*/

	// parse through inputs 
	PARSE_ARGS;

	// print out start
	std::cout << "-----------------------------------------------------------------------------" << std::endl;
	std::cout << "                         MULTI-LEVEL REGISTRATION ";
	std::cout << "-----------------------------------------------------------------------------" << std::endl;
	std::cout << std::endl;

	// instantiate image and transform types
	typedef short					PixelType;
	typedef itk::Image< PixelType, 3 >	ImageType;
	typedef itk::Image< unsigned char, 3 >	MaskImageType;
	typedef itk::ScaleVersor3DTransform< double >	TransformType;
	typedef itk::AffineTransform< double >	AffineTransformType;

	// manage transforms/cropping/applying class
	itk::ManageTransformsFilter::Pointer transforms = itk::ManageTransformsFilter::New();

	// validation
	ImageType::Pointer fixedImageMask = ImageType::New();
	ImageType::Pointer movingImageMask = ImageType::New();
	bool validation = false;
	if (!fixedImageMaskFilename.empty() && !movingImageMaskFilename.empty())
	{
		validation = true;
		std::cout << "Validation will be performed" << std::endl;
	}

	// fixed and moving images
	ImageType::Pointer fixedImage = ImageType::New();
	ImageType::Pointer movingImage = ReadInImage< ImageType >(movingImageFilename.c_str());
	if (validation)
	{
		movingImageMask = ReadInImage< ImageType >(movingImageMaskFilename.c_str());
	}

	// apply fixed initial transform if given
	if (!fixedImageInitialTransform.empty())
	{
		// apply transform to fixed image
		TransformType::Pointer initialFixedTransform = ReadInTransform< TransformType >(fixedImageInitialTransform.c_str());
		ImageType::Pointer fixedImageTemp = ReadInImage< ImageType >(fixedImageFilename.c_str());
		try
		{
			std::cout << "Applying initial transform to fixed image." << std::endl;
			transforms->SetFixedImage(ReadInImage< ImageType >(referenceImage.c_str()));
			fixedImage = transforms->ResampleImage(fixedImageTemp, initialFixedTransform);
		}
		catch (itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}
		std::cout << "Initial transform applied to fixed image." << std::endl;
		std::cout << "Moving image read in." << std::endl;

		if (validation)
		{
			transforms->NearestNeighborInterpolateOn();
			ImageType::Pointer fixedImageMaskTemp = ReadInImage< ImageType >(fixedImageMaskFilename.c_str());
			fixedImageMask = transforms->ResampleImage(fixedImageMaskTemp, initialFixedTransform);
			transforms->NearestNeighborInterpolateOff();
		}
	}
	else
	{
		fixedImage = ReadInImage< ImageType >(fixedImageFilename.c_str());
		std::cout << "Fixed image read in." << std::endl;
		std::cout << "Moving image read in." << std::endl;

		if (validation)
		{
			fixedImageMask = ReadInImage< ImageType >(fixedImageMaskFilename.c_str());
		}
	}

	// preprocessing
	std::cout << "\n*********************************************" << std::endl;
	std::cout << "              PREPROCESSING                  " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	if (upperThreshold > 0)
	{
		movingImage = UpperThresholdImage< ImageType >(movingImage, upperThreshold);
	}
	if (lowerThreshold > 0)
	{
		movingImage = LowerThresholdImage< ImageType >(movingImage, lowerThreshold);
	}
	if (sigma > 0)
	{
		movingImage = SmoothImage< ImageType >(movingImage, sigma);
	}

	// write out preprocessed images if debugging
	if (!debugDirectory.empty() && debugImages)
	{
		std::string movingFilename = debugDirectory + "\\PreprocessedMovingImage.nrrd";
		WriteOutImage< ImageType, ImageType >(movingFilename.c_str(), transforms->GetTransformedImage());
	}

	// initialization
	std::cout << "\n*********************************************" << std::endl;
	std::cout << "              INITIALIZATION                 " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	// perform initialization class
	itk::InitializationFilter<PixelType>::Pointer initialize = itk::InitializationFilter<PixelType>::New();
	TransformType::Pointer initialTransform = TransformType::New();
	initialize->SetFixedImage(fixedImage);
	initialize->SetMovingImage(movingImage);

	if (manualInitialTransformFilename.empty())
	{
		// turn on flags if no predefined transform
		if (observe){ initialize->ObserveOn(); }
		if (centerOfGeometry){ initialize->CenteredOnGeometryOn(); }
		if (iterativeAlignment) { initialize->IterativeAlignmentOn(); }
		if (transX){ initialize->MetricTranslationOn(0); }
		if (transY){ initialize->MetricTranslationOn(1); }
		if (transZ){ initialize->MetricTranslationOn(2); }
		if (rotX){ initialize->MetricRotationOn(0); }
		if (rotY){ initialize->MetricRotationOn(1); }
		if (rotZ){ initialize->MetricRotationOn(2); }
		initialize->Update();
		initialTransform = initialize->GetTransform();
		std::cout << "Initialization complete." << std::endl;
	}
	else
	{
		initialize->Update(ReadInTransform< AffineTransformType >(manualInitialTransformFilename.c_str()));
		initialTransform = initialize->GetTransform();
		std::cout << "Initialization complete via predefined transform." << std::endl;
	}

	std::cout << "\nFinal Parameters" << std::endl;
	std::cout << "Transform" << std::endl;
	std::cout << "  Translation   : " << initialTransform->GetTranslation() << std::endl;
	std::cout << "  Rotation      : " << (initialTransform->GetVersor().GetAngle())*180.0 / 3.141592653589793238463 << std::endl;
	std::cout << "  Axis          : " << initialTransform->GetVersor().GetAxis() << std::endl;
	std::cout << std::endl;

	// set up transforms class and insert fixed image (will not change)
	//transforms->AddTransform( initialize->GetTransform() );
	transforms->SetInitialTransform(initialTransform);
	transforms->AddTransform(initialTransform);
	transforms->SetFixedImage(fixedImage);
	transforms->SetMovingImage(movingImage);

	// write out initial transform
	WriteOutTransform< TransformType >(finalTransform.c_str(), initialTransform);

	if (!debugDirectory.empty() && debugTransforms)
	{
		std::string transformFilename = debugDirectory + "\\InitialTransform.tfm";
		WriteOutTransform< TransformType >(transformFilename.c_str(), initialTransform);
	}

	// validation at initialTransform
	itk::ValidationFilter::Pointer validationFilter = itk::ValidationFilter::New();
	if (validation)
	{
		validationFilter->SetImage1(fixedImage);
		validationFilter->SetLabelMap1(fixedImageMask);
		validationFilter->SetImage2(transforms->ResampleImage(movingImage, initialTransform));
		transforms->NearestNeighborInterpolateOn();
		validationFilter->SetLabelMap2(transforms->ResampleImage(movingImageMask, initialTransform));
		transforms->NearestNeighborInterpolateOff();
		validationFilter->LabelOverlapMeasuresOn();
		try
		{
			validationFilter->Update();
		}
		catch (itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}
	}

	// determine the number of ROIs and creating iterators
	int numberOfROIs = ROI.size();
	bool ROI1 = false;

	std::vector< std::vector< float> >::iterator it = ROI.begin();
	if (numberOfLevels - numberOfROIs == 1)
	{
		ROI1 = false;
	}
	else if (numberOfLevels - numberOfROIs == 0)
	{
		ROI1 = true;
	}
	else
	{
		std::cout << "Mismatched number of ROIs specified." << std::endl;
		return EXIT_FAILURE;
	}

	for (int level = 1; level < numberOfLevels + 1; ++level)
	{
		std::cout << "\n*********************************************" << std::endl;
		std::cout << "            REGISTRATION LEVEL " << level << "               " << std::endl;
		std::cout << "*********************************************\n" << std::endl;

		// prepare the Registration framework
		itk::RegistrationFramework::Pointer registration = itk::RegistrationFramework::New();

		// apply transform from previous level 
		transforms->ResampleImageOn();

		// insert appropriate ROI into transforms class and crop image
		if (level == 1 && ROI1) // if it is level 1 and ROI is to be used in Level 1
		{
			std::cout << "Applying ROI at level 1" << std::endl;

			transforms->SetROI(*it);
			transforms->CropImageOn();

			std::vector<float>::iterator jt = (*it).begin();
			std::cout << "ROI: ";
			for (jt; jt != (*it).end(); jt++)
			{
				std::cout << *jt << ", ";
			}
			std::cout << std::endl;

			it++;
			// insert images into registration class
			try
			{
				transforms->Update();
			}
			catch (itk::ExceptionObject & err)
			{
				std::cerr << "Exception Object Caught!" << std::endl;
				std::cerr << err << std::endl;
				std::cerr << std::endl;
				return EXIT_FAILURE;
			}
			registration->SetFixedImage(transforms->GetFixedCroppedImage());
			registration->SetMovingImage(transforms->GetMovingCroppedImage());

			if (!debugDirectory.empty() && debugImages)
			{
				std::string fixedFilename = debugDirectory + "\\Level" + std::to_string(level) + "InputFixedImage.nrrd";
				WriteOutImage< ImageType, ImageType >(fixedFilename.c_str(), transforms->GetTransformedImage());
				std::string movingFilename = debugDirectory + "\\Level" + std::to_string(level) + "InputMovingImage.nrrd";
				WriteOutImage< ImageType, ImageType >(movingFilename.c_str(), transforms->GetTransformedImage());
			}
		}
		else if (level != 1) // if it is not level 1
		{
			transforms->SetROI(*it);
			transforms->CropImageOn();

			std::vector<float>::iterator jt = (*it).begin();
			std::cout << "ROI: ";
			for (jt; jt != (*it).end(); jt++)
			{
				std::cout << *jt << ", ";
			}
			std::cout << std::endl;

			it++;
			// insert images into registration class
			try
			{
				transforms->Update();
			}
			catch (itk::ExceptionObject & err)
			{
				std::cerr << "Exception Object Caught!" << std::endl;
				std::cerr << err << std::endl;
				std::cerr << std::endl;
				return EXIT_FAILURE;
			}
			registration->SetFixedImage(transforms->GetFixedCroppedImage());
			registration->SetMovingImage(transforms->GetMovingCroppedImage());

			if (!debugDirectory.empty() && debugImages)
			{
				std::string fixedFilename = debugDirectory + "\\Level" + std::to_string(level) + "InputFixedImage.nrrd";
				WriteOutImage< ImageType, ImageType >(fixedFilename.c_str(), transforms->GetTransformedImage());
				std::string movingFilename = debugDirectory + "\\Level" + std::to_string(level) + "InputMovingImage.nrrd";
				WriteOutImage< ImageType, ImageType >(movingFilename.c_str(), transforms->GetTransformedImage());
			}
		}
		else // ROI is not applied at level
		{
			std::cout << "ROI not applied at level " << level << std::endl;
			// insert images into registration class
			try
			{
				transforms->Update();
			}
			catch (itk::ExceptionObject & err)
			{
				std::cerr << "Exception Object Caught!" << std::endl;
				std::cerr << err << std::endl;
				std::cerr << std::endl;
				return EXIT_FAILURE;
			}
			registration->SetFixedImage(fixedImage);
			registration->SetMovingImage(transforms->GetTransformedImage());

			if (!debugDirectory.empty() && debugImages)
			{
				std::cout << "Fixed image not changed at level " << level << std::endl;
				std::string movingFilename = debugDirectory + "\\Level" + std::to_string(level) + "InputMovingImage.nrrd";
				WriteOutImage< ImageType, ImageType >(movingFilename.c_str(), transforms->GetTransformedImage());
			}
		}

		// insert parameters into registration
		registration->SetNumberOfIterations(numberOfIterations);
		registration->SetRelaxationFactor(relaxationFactor);
		registration->SetMinimumStepLength(minimumStepLength);
		registration->SetGradientMagnitudeTolerance(gradientMagnitudeTolerance);

		if (level != 1)
		{
			registration->SetMaximumStepLength(maximumStepLength / (parameterRelaxation*(level - 1)));
			registration->SetRotationScale(rotationScale / (parameterRelaxation*(level - 1)));
			registration->SetTranslationScale(translationScale / (parameterRelaxation*(level - 1)));
			registration->SetScalingScale(scalingScale / (parameterRelaxation*(level - 1)));
		}
		else
		{
			registration->SetMaximumStepLength(maximumStepLength);
			registration->SetRotationScale(rotationScale);
			registration->SetTranslationScale(translationScale);
			registration->SetScalingScale(scalingScale);
		}

		// observe process
		if (observe) { registration->ObserveOn(); }
		if (debugTransforms)
		{
			registration->DebugOn();
			std::string directory = debugDirectory + "\\Level" + std::to_string(level);
			registration->SetDebugDirectory(directory);
		}

		// perform registration
		try
		{
			registration->Update();
		}
		catch (itk::ExceptionObject & err)
		{
			std::cerr << "Exception Object Caught!" << std::endl;
			std::cerr << err << std::endl;
			std::cerr << std::endl;
		}

		// print results
		registration->Print();

		// add transform to transforms class
		transforms->AddTransform(registration->GetFinalTransform());

		// write out composite transform
		WriteOutTransform< itk::ManageTransformsFilter::CompositeTransformType >(finalTransform.c_str(), transforms->GetCompositeTransform());

		// write out transforms
		if (!debugDirectory.empty() && debugTransforms)
		{
			std::string transformFilename = debugDirectory + "\\Level" + std::to_string(level) + "Transform.tfm";
			WriteOutTransform< itk::ManageTransformsFilter::CompositeTransformType >(transformFilename.c_str(), transforms->GetCompositeTransform());
		}

		// write out images
		if (!debugDirectory.empty() && debugImages)
		{
			std::string movingFilename = debugDirectory + "\\Level" + std::to_string(level) + "OuputMovingImage.nrrd";
			WriteOutImage< ImageType, ImageType >(movingFilename.c_str(), transforms->ResampleImage(movingImage, transforms->GetCompositeTransform()));
		}

		// obtain validation measures
		if (validation)
		{
			validationFilter->SetImage2(transforms->ResampleImage(movingImage, transforms->GetCompositeTransform()));
			transforms->NearestNeighborInterpolateOn();
			validationFilter->SetLabelMap2(transforms->ResampleImage(movingImageMask, transforms->GetCompositeTransform()));
			transforms->NearestNeighborInterpolateOff();
			validationFilter->LabelOverlapMeasuresOn();
			try
			{
				validationFilter->Update();
			}
			catch (itk::ExceptionObject & err)
			{
				std::cerr << "Exception Object Caught!" << std::endl;
				std::cerr << err << std::endl;
				std::cerr << std::endl;
			}
		}
	}

  return EXIT_SUCCESS;
}
