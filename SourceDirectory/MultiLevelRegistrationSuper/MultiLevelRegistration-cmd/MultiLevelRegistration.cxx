/*
COMMAND LINE INTERFACE CODE
*/

// include files
#include "C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SourceDirectory\MultiLevelRegistrationSuper\ReadWriteFunctions.hxx"
#include "C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SourceDirectory\MultiLevelRegistrationSuper\PreprocessingFunctions.hxx"
#include "C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SourceDirectory\MultiLevelRegistrationSuper\itkRegistrationFramework.h"
#include "C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SourceDirectory\MultiLevelRegistrationSuper\itkInitializationFilter.h"
#include "C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SourceDirectory\MultiLevelRegistrationSuper\itkValidationFilter.h"
#include "C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SourceDirectory\MultiLevelRegistrationSuper\itkManageTransformsFilter.h"

// rescale images
#include "itkRescaleIntensityImageFilter.h"
//#include "itkPluginUtilities.h"

// monitoring
#include "itkTimeProbesCollectorBase.h"
#include "itkMemoryProbesCollectorBase.h"
#include <time.h>
#include <stdio.h>
#include <vector>

#include <windows.h>

// declare function
void PrintOutManual();
void Timestamp();

int main(int argc, char * argv[])
{

	// ****************************************************************************************************************************
	std::cout << argc << std::endl;
	// parse through inputs
	if (argc < 3)
	{
		PrintOutManual();
		return EXIT_SUCCESS;
	}

	/*
	// print out all inputs
	for (int i = 0; i < argc; i++)
	{
		std::cout << argv[i] << std::endl;
	}
	*/

	// input images
	std::cout << std::endl;
	int ni = 1;
	std::string emptyStr = "[]";
	std::string fixedImageFilename = ""; if (emptyStr.compare(argv[ni]) != 0){ fixedImageFilename = argv[ni]; std::cout << "fixedImageFilename: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	std::string movingImageFilename = ""; if (emptyStr.compare(argv[ni]) != 0){ movingImageFilename = argv[ni]; std::cout << "movingImageFilename: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	std::string finalTransform = ""; if (emptyStr.compare(argv[ni]) != 0){ finalTransform = argv[ni]; std::cout << "finalTransform: " << argv[ni] << std::endl; ni++; }
	else { ni++; }

	// number of levels
	int numberOfLevels = 0; if (emptyStr.compare(argv[ni]) != 0) { numberOfLevels = atoi(argv[ni]); std::cout << "numberOfLevels: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	itk::ManageTransformsFilter::Pointer dealWithROIs = itk::ManageTransformsFilter::New();
	std::vector<std::vector<float>> ROI;
	std::string ROI1Filename = ""; if (emptyStr.compare(argv[ni]) != 0)
	{
		ROI1Filename = argv[ni];
		std::vector<float> ROIvalues = dealWithROIs->ExtractROIPoints(ROI1Filename.c_str());
		ROI.push_back(ROIvalues);
		std::cout << "ROI1Filename: " << argv[ni] << std::endl;
		ni++;
	}
	else { ni++; }
	std::string ROI2Filename = ""; if (emptyStr.compare(argv[ni]) != 0)
	{
		ROI2Filename = argv[ni];
		std::vector<float> ROIvalues = dealWithROIs->ExtractROIPoints(ROI2Filename.c_str());
		ROI.push_back(ROIvalues);
		std::cout << "ROI2Filename: " << argv[ni] << std::endl;
		ni++;
	}
	else { ni++; }
	std::string ROI3Filename = ""; if (emptyStr.compare(argv[ni]) != 0)
	{
		ROI3Filename = argv[ni];
		std::vector<float> ROIvalues = dealWithROIs->ExtractROIPoints(ROI3Filename.c_str());
		ROI.push_back(ROIvalues);
		std::cout << "ROI3Filename: " << argv[ni] << std::endl;
		ni++; 
	}
	else { ni++; }

	// preprocessing
	float upperThreshold = -1.0; if (emptyStr.compare(argv[ni]) != 0){ upperThreshold = atof(argv[ni]); std::cout << "upperThreshold: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	float lowerThreshold = -1.0; if (emptyStr.compare(argv[ni]) != 0){ lowerThreshold = atof(argv[ni]); std::cout << "lowerThreshold: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	float sigma = -1.0; if (emptyStr.compare(argv[ni]) != 0){ sigma = atof(argv[ni]); std::cout << "sigma: " << argv[ni] << std::endl; ni++; }
	else { ni++; }

	// initialization
	std::string fixedImageInitialTransform = ""; if (emptyStr.compare(argv[ni]) != 0){ fixedImageInitialTransform = argv[ni]; std::cout << "fixedImageInitialTransform: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	std::string referenceImage = ""; if (emptyStr.compare(argv[ni]) != 0) { referenceImage = argv[ni]; std::cout << "referenceImage: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	std::string manualInitialTransformFilename = ""; if (emptyStr.compare(argv[ni]) != 0) { manualInitialTransformFilename = argv[ni]; std::cout << "manualInitialTransformFilename: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	int centerOfGeometry = 1; if (emptyStr.compare(argv[ni]) != 0) { centerOfGeometry = atoi(argv[ni]); std::cout << "centerOfGeometry: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	int iterativeAlignment = 0; if (emptyStr.compare(argv[ni]) != 0) { iterativeAlignment = atoi(argv[ni]); std::cout << "iterativeAlignment: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	int transX = 0; if (emptyStr.compare(argv[ni]) != 0) { transX = atoi(argv[ni]); std::cout << "transX: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	int transY = 0; if (emptyStr.compare(argv[ni]) != 0) { transY = atoi(argv[ni]); std::cout << "transY: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	int transZ = 0; if (emptyStr.compare(argv[ni]) != 0) { transZ = atoi(argv[ni]); std::cout << "transZ: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	int rotX = 0; if (emptyStr.compare(argv[ni]) != 0) { rotX = atoi(argv[ni]); std::cout << "rotX: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	int rotY = 0; if (emptyStr.compare(argv[ni]) != 0) { rotY = atoi(argv[ni]); std::cout << "rotY: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	int rotZ = 0; if (emptyStr.compare(argv[ni]) != 0) { rotZ = atoi(argv[ni]); std::cout << "rotZ: " << argv[ni] << std::endl; ni++; }
	else { ni++; }

	// registration parameters
	float parameterRelaxation = 2.0; if (emptyStr.compare(argv[ni]) != 0) { parameterRelaxation = atof(argv[ni]); std::cout << "parameterRelaxation: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	float rotationScale = 0.001; if (emptyStr.compare(argv[ni]) != 0) { rotationScale = atof(argv[ni]); std::cout << "rotationScale: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	float translationScale = 10; if (emptyStr.compare(argv[ni]) != 0) { translationScale = atof(argv[ni]); std::cout << "translationScale: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	float scalingScale = 0.001; if (emptyStr.compare(argv[ni]) != 0) { scalingScale = atof(argv[ni]); std::cout << "scalingScale: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	int numberOfIterations = 500; if (emptyStr.compare(argv[ni]) != 0) { numberOfIterations = atoi(argv[ni]); std::cout << "numberOfIterations: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	float maximumStepLength = 1.0; if (emptyStr.compare(argv[ni]) != 0) { maximumStepLength = atof(argv[ni]); std::cout << "maximumStepLength: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	float minimumStepLength = 0.001; if (emptyStr.compare(argv[ni]) != 0) { minimumStepLength = atof(argv[ni]); std::cout << "minimumStepLength: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	float relaxationFactor = 0.5; if (emptyStr.compare(argv[ni]) != 0) { relaxationFactor = atof(argv[ni]); std::cout << "relaxationFactor: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	float gradientMagnitudeTolerance = 0.001; if (emptyStr.compare(argv[ni]) != 0) { gradientMagnitudeTolerance = atof(argv[ni]); std::cout << "gradientMagnitudeTolerance: " << argv[ni] << std::endl; ni++; }
	else { ni++; }

	// validation
	std::string fixedImageMaskFilename = ""; if (emptyStr.compare(argv[ni]) != 0){ fixedImageMaskFilename = argv[ni]; std::cout << "fixedImageMaskFilename: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	std::string movingImageMaskFilename = ""; if (emptyStr.compare(argv[ni]) != 0){ movingImageMaskFilename = argv[ni]; std::cout << "movingImageMaskFilename: " << argv[ni] << std::endl; ni++; }
	else { ni++; }

	// debugging
	int observe = 0; if (emptyStr.compare(argv[ni]) != 0) { observe = atoi(argv[ni]); std::cout << "observe: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	int debugTransforms = 0; if (emptyStr.compare(argv[ni]) != 0) { debugTransforms = atoi(argv[ni]); std::cout << "debugTransforms: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	int debugImages = 0; if (emptyStr.compare(argv[ni]) != 0) { debugImages = atoi(argv[ni]); std::cout << "debugImages: " << argv[ni] << std::endl; ni++; }
	else { ni++; }
	std::string debugDirectory = ""; if (emptyStr.compare(argv[ni]) != 0){ debugDirectory = argv[ni]; std::cout << "debugDirectory: " << argv[ni] << std::endl; ni++; }
	else { ni++; }

	// ****************************************************************************************************************************

	// print out start
	std::cout << std::endl;
	std::cout << "-----------------------------------------------------------------------------" << std::endl;
	std::cout << "             MULTI-LEVEL REGISTRATION ";
	Timestamp();
	std::cout << "-----------------------------------------------------------------------------" << std::endl;
	std::cout << std::endl;

	// probes (time/memory)
	itk::TimeProbesCollectorBase	chronometer;
	itk::MemoryProbesCollectorBase	memorymeter;

	// full program
	chronometer.Start("Full program");
	memorymeter.Start("Full program");

	// instantiate image and transform types
	typedef itk::Image< short, 3 >	ImageType;
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
	chronometer.Start("Preprocessing");
	memorymeter.Start("Preprocessing");

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
	chronometer.Start("Initialization");
	memorymeter.Start("Initialization");

	// initialization
	std::cout << "\n*********************************************" << std::endl;
	std::cout << "              INITIALIZATION                 " << std::endl;
	std::cout << "*********************************************\n" << std::endl;

	// perform initialization class
	itk::InitializationFilter::Pointer initialize = itk::InitializationFilter::New();
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
	itk::ManageTransformsFilter::Pointer vtf = itk::ManageTransformsFilter::New();
	if (validation)
	{
		vtf->SetFixedImage(fixedImage);
		vtf->SetFixedLabelMap(fixedImageMask);
		validationFilter->SetImage1(fixedImage);
		validationFilter->SetLabelMap1(fixedImageMask);
		validationFilter->SetImage2(vtf->ResampleImage(movingImage, initialTransform));
		vtf->NearestNeighborInterpolateOn();
		validationFilter->SetLabelMap2(vtf->ResampleImage(movingImageMask, initialTransform));
		vtf->NearestNeighborInterpolateOff();
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

	// initialization
	chronometer.Stop("Initialization");
	memorymeter.Stop("Initialization");

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

	// create independent level transforms
	TransformType::Pointer level1;
	TransformType::Pointer level2;
	TransformType::Pointer level3;
	TransformType::Pointer level4;
	TransformType::Pointer level5;

	for ( int level = 1; level < numberOfLevels + 1; ++level)
	{
		// monitors
		std::string message = "Level " + level;
		chronometer.Start(message.c_str());
		memorymeter.Start(message.c_str());

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

		// Registration level 1
		chronometer.Stop(message.c_str());
		memorymeter.Stop(message.c_str());
	}

	// full program
	chronometer.Stop( "Full program" );
	memorymeter.Stop( "Full program" );

	// print out time/memory results
	chronometer.Report( std::cout );
	memorymeter.Report( std::cout );
	
	return EXIT_SUCCESS;
}

void Timestamp()
{
	time_t ltime;
	ltime = time( NULL );
	printf("%s",asctime( localtime( &ltime ) ) );
	return;
}

void PrintOutManual()
{
	std::cout << "                           **** USER MANUAL ****                             " << std::endl;
	std::cout << std::endl;

	std::cout << " IMAGES: " << std::endl;
	std::cout << "  fixedImageFilename: filename for the fixed image including path | required" << std::endl;
	std::cout << "  movingImageFilename: filename for the moving image including path | required" << std::endl;
	std::cout << "  finalTransformFilename: filename for the final transform (output) | required" << std::endl;
	std::cout << std::endl;

	std::cout << " MULTI-LEVEL: " << std::endl;
	std::cout << "  [numberOfLevels]: number of desired levels for registration | default = 0" << std::endl;
	std::cout << "          = 0 | initialization only " << std::endl;
	std::cout << "          = 1 | whole image registration " << std::endl;
	std::cout << "          = 2 | ROI used to isolate a specific region for registration \n";
	std::cout << "                (must be smaller than the whole image) " << std::endl;
	std::cout << "          = 3 | an additional ROI used to pair down the process even \n";
	std::cout << "                further (must be within the 2nd level ROI) " << std::endl;
	std::cout << "  [level1ROIFilename]: path to the ROI used in the first level\n";
	std::cout << "  [level2ROIFilename]: path to the ROI used in the second level | required \n";
	std::cout << "                       if numberOfLevels >= 2" << std::endl;
	std::cout << "  [level3ROIFilename]: path to the ROI used in the third level | required \n";
	std::cout << "                       if numberOfLevels = 3" << std::endl;
	std::cout << std::endl;

	std::cout << " PREPROCESSING (only applied to moving image currently): " << std::endl;
	std::cout << "  [upperThreshold]: upper threshold value for thresholding the moving image\n";
	std::cout << "  [lowerThreshold]: lower threshold value for thresholding the moving image \n";
	std::cout << "  [sigma]: variance of the gaussian kernel used for smoothing the moving image \n";
	std::cout << std::endl;

	std::cout << " INITIALIZATION: " << std::endl;
	std::cout << "  [fixedImageInitialTransform]: inital transform applied to the fixed image\n";
	std::cout << "                                prior to registration | default = NULL" << std::endl;
	std::cout << "  [referenceImage]: image used as a reference when applying the fixed image\n";
	std::cout << "                    initial transform | required if fixedImageInitialTransform" << std::endl;
	std::cout << "  [manualInitialTransform]: initial transform filename as a manual input" << std::endl;
	std::cout << "                            | default = NULL" << std::endl;
	std::cout << "  [centerOfGeometry]: perform center of geomentry initialization | default = 1" << std::endl;
	std::cout << "          = 0 | YES " << std::endl;
	std::cout << "          = 1 | NO " << std::endl;
	std::cout << "  [iterativeAlignment]: iterative over 125 locations for best alignment | default = 0" << std::endl;
	std::cout << "          = 0 | YES " << std::endl;
	std::cout << "          = 1 | NO " << std::endl;
	std::cout << "  [transX]: perform translation initialization in the x axis | default = 0" << std::endl;
	std::cout << "          = 0 | YES " << std::endl;
	std::cout << "          = 1 | NO " << std::endl;
	std::cout << "  [transY]: perform translation initialization in the y axis | default = 0" << std::endl;
	std::cout << "          = 0 | YES " << std::endl;
	std::cout << "          = 1 | NO " << std::endl;
	std::cout << "  [transZ]: perform translation initialization in the z axis | default = 0" << std::endl;
	std::cout << "          = 0 | YES " << std::endl;
	std::cout << "          = 1 | NO " << std::endl;
	std::cout << "  [rotX]: perform rotational initialization around the x axis | default = 0" << std::endl;
	std::cout << "          = 0 | YES " << std::endl;
	std::cout << "          = 1 | NO " << std::endl;
	std::cout << "  [rotY]: perform translation initialization around the y axis | default = 0" << std::endl;
	std::cout << "          = 0 | YES " << std::endl;
	std::cout << "          = 1 | NO " << std::endl;
	std::cout << "  [rotZ]: perform translation initialization around the z axis | default = 0" << std::endl;
	std::cout << "          = 0 | YES " << std::endl;
	std::cout << "          = 1 | NO " << std::endl;
	std::cout << std::endl;

	std::cout << " REGISTRATION PARAMETERS: " << std::endl;
	std::cout << "  [parameterRelaxation]: divisor to reduced parameters at each level | default = 2" << std::endl;
	std::cout << "  [rotationScale]: expected amount of rotation to occur | default = 0.001" << std::endl;
	std::cout << "  [translationScale]: expected amount of translation to occur | default = 10" << std::endl;
	std::cout << "  [scalingScale]: expected amount of scaling to occur | default = 0.001" << std::endl;
	std::cout << "  [numberOfIterations]: number of iterations allowed at each level of \n";
	std::cout << "                        registration | default = 500" << std::endl;
	std::cout << "  [maximumStepLength]: maximum step length allowed at each level | default = 1" << std::endl;
	std::cout << "  [minimumStepLength]: minimum step length allowed at each level | default = 0.001" << std::endl;
	std::cout << "  [relaxationFactor]: amount by which the step length decreases during \n";
	std::cout << "                      optimization | default = 0.5" << std::endl;
	std::cout << "  [gradientMagnitudeTolerance]: the minimum gradient magnitude \n";
	std::cout << "                                allowed | default = 0.001" << std::endl;
	std::cout << std::endl;

	std::cout << " VALIDATION PARAMETERS: " << std::endl;
	std::cout << "  [fixedImageMaskFilename]: path to the label map corresponding to \n";
	std::cout << "                               the fixed image for validation | required" << std::endl;
	std::cout << "  [movingImageMaskFilename]: path to the label map corresponding to \n";
	std::cout << "                                the moving image for validation | required" << std::endl;

	std::cout << " SPECIAL PARAMETERS: " << std::endl;
	std::cout << "  [observe]: print out the monitoring of the registration process at each \n";
	std::cout << "             level | default = 0" << std::endl;
	std::cout << "          = 0 | YES " << std::endl;
	std::cout << "          = 1 | NO " << std::endl;
	std::cout << "  [debugTransforms]: print out transforms at each level and every 50 iterations\n";
	std::cout << "                     | default = 0" << std::endl;
	std::cout << "  [debugImages]: print out results at start and end of each level | default = 0" << std::endl;
	std::cout << "  [debugDirectory]: path to folder that will contain all the debug information" << std::endl;
	std::cout << std::endl;
	
	return;
}
