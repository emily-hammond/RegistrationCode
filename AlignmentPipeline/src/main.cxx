/* Emily Hammond
 * 2014-10-17
 *
 * This code is the main code written to align multi-modal images taken of the same
 * subject on the same day.
 * 
 * steps:
 *	1. Create a set up for the desired inputs and outputs
 *			Inputs: fixed image, moving image, output directory name
 *			Outputs: transformed moving image, final transform parameters, 
 *					 deformation image, jacobian map			
 *	2. Read and write functions pulled from RegistrationCode/src/ReadWriteFunctions
 *	3. Flow of program
 *		- Read in desired images
 *		- (Perform desired preprocessing steps)
 *		- Perform initial alignment
 *			CenteredTransformInitializer by geometry
 *		- Perform rigid registration
 *			Components: VersorRigid3DTransform, VersorRigid3DOptimizer, 
 *						MattesMutualInformation, LinearInterpolator
 *		- Give option for affine registration
 *			Components: AffineTransform, ConjugateGradientLineSearchOptimizerv4, 
 *						MattesMutualInformation, LinearInterpolator
 *		- Give option for bspline registration
 *			Components: BSplineTransform, ConjugateGradientLineSearchOptimizerv4, 
 *						MattesMutualInformation, LinearInterpolator
 *		- Write out all the desired files
 *
 */

/**************************************************************************
 * Header files to include
 **************************************************************************/

// reading/writing files
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"

// initialization
#include "itkCenteredTransformInitializer.h"

// creating histogram
#include "itkImageToHistogramFilter.h"
#include "itkMinimumMaximumImageCalculator.h"

// rigid registration
#include "itkVersorRigid3DTransform.h"
#include "itkMattesMutualInformationImageToImageMetricv4.h"

// additional C++ libraries
#include <itksys/SystemTools.hxx>
#include <fstream>
#include <string>

/*************************************************************************
 * Write templated functions for reading/writing files to clean up code 
 *************************************************************************/

// Write a function to read in images templated over dimension and pixel type
template<typename ImageType>
typename ImageType::Pointer ReadInImage( const char * ImageFilename )
{
	typedef itk::ImageFileReader<ImageType>		ReaderType;	
	typename ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName( ImageFilename );
	
	// update reader
	try
	{
		reader->Update();
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}
	
	// return output
	return reader->GetOutput();
}


// Write a function to write out images templated over input and output pixel type
template<typename inputImageType, typename outputImageType>
int WriteOutImage( const char * ImageFilename, typename inputImageType::Pointer image )
{
	typedef itk::CastImageFilter<inputImageType, outputImageType> CastFilterType;
	typename CastFilterType::Pointer caster = CastFilterType::New();
	caster->SetInput( image );

	typedef itk::ImageFileWriter<outputImageType> WriterType;
	typename WriterType::Pointer writer = WriterType::New();
	writer->SetFileName( ImageFilename );
	writer->SetInput( caster->GetOutput() );

	// update the writer
	try
	{
		writer->Update();
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}
	
	// return output
	return EXIT_SUCCESS;
}

// Write a function to read in the fiducials (from Slicer) for each image
// templated over point type (taken from image) and landmarks type (map<string, point type>)
template<typename PointType, typename LandmarksType>
typename LandmarksType ReadFiducial( const char * fiducialFilename )
{
	// open file
	std::ifstream myfile( fiducialFilename );
	LandmarksType landmarks;
	
	// read in lines from file
	std::string line;
	while( getline( myfile, line ) )
	{
		if( line.compare( 0, 1, "#" ) != 0 ) // skip lines starting with #
		{
			// determine where the first coordinate lives
			size_t pos1 = line.find( ',', 0 );
			PointType	pointPos;

			// grab the coordinates from the string
			for( unsigned int i = 0; i < 3; ++i )
			{
				const size_t pos2 = line.find( ',', pos1+1 );
				pointPos[i] = atof( line.substr( pos1+1, pos2-pos1-1 ).c_str() );
				if( i < 2 ) // negate first two components for RAS->LPS
				{
					pointPos[i] *= -1;
				}
				pos1 = pos2;
			}
			
			// determine what label/location the coordinates are from
			if( line.find( "Carina" ) != -1 )
			{
				landmarks["Carina"] = pointPos;
			}
			else if( line.find( "Aorta" ) != -1 )
			{
				landmarks["Aorta"] = pointPos;
			}
			else if( line.find( "BaseOfHeart" ) != -1 )
			{
				landmarks["BaseOfHeart"] = pointPos;
			}
			else
			{
				std::cerr << "Reassign labels to fiducials." << std::endl;
				std::cerr << "   Acceptable labels: Carina, Aorta, BaseOfHeart" << std::endl;
			}
		}
	}
	return landmarks;
}



// Write a function to write out a transform
template<typename TransformType>
int WriteOutTransform( const char * transformFilename, typename TransformType::Pointer transform )
{
	typedef itk::TransformFileWriterTemplate< double > TransformWriterType;
	TransformWriterType::Pointer writer = TransformWriterType::New();
	writer->SetInput( transform );
	writer->SetFileName( transformFilename );

	// update the writer
	try
	{
		writer->Update();
	}
	catch(itk::ExceptionObject & err)
	{
		std::cerr << "Exception Object Caught!" << std::endl;
		std::cerr << err << std::endl;
		std::cerr << std::endl;
	}
	
	// return output
	return EXIT_SUCCESS;
}

// Write function to output the histogram
template<typename ImageType>
int CreateHistogram(typename ImageType::Pointer image)
{
	// plot the histogram based on the number of bins
	typedef itk::Statistics::ImageToHistogramFilter< typename ImageType >	HistogramFilterType;
	HistogramFilterType::Pointer histogramFilter = HistogramFilterType::New();

	typedef itk::MinimumMaximumImageCalculator< ImageType >	MinMaxCalculatorType;
	MinMaxCalculatorType::Pointer minMaxCalc = MinMaxCalculatorType::New();

	minMaxCalc->SetImage( image );
	minMaxCalc->Compute();
	ImageType::PixelType min = minMaxCalc->GetMinimum();
	ImageType::PixelType max = minMaxCalc->GetMaximum();
	HistogramFilterType::HistogramSizeType bins;
	bins[0] = (max - min)/10.0;

	histogramFilter->SetHistogramSize( bins );
	histogramFilter->SetMarginalScale( 10.0 );
	//histogramFilter->SetHistogramBinMinimum( 0.0 );
	//histogramFilter->SetHistogramBinMaximum( 1000000 );

	histogramFilter->SetInput( image );
	histogramFilter->Update();

	typedef HistogramFilterType::HistogramType	HistogramType;
	const HistogramType * histogram = histogramFilter->GetOutput();

	std::ofstream histogramFile;
	histogramFile.open( "histogram.txt" );

	HistogramType::ConstIterator histItr = histogram->Begin();
	HistogramType::ConstIterator histEnd = histogram->End();

	typedef HistogramType::AbsoluteFrequencyType	AbsoluteFrequencyType;
	while( histItr != histEnd )
	{
		const AbsoluteFrequencyType frequency = histItr.GetFrequency();
		histogramFile.write( (const char *)(&frequency), sizeof(frequency) );
		if( frequency != 0)
		{
			HistogramType::IndexType index;
			index = histogram->GetIndex(histItr.GetInstanceIdentifier());
			std::cout << "Index = " << index << ", Frequency = " << frequency << std::endl;
		}
		++histItr;
	}

	histogramFile.close();

	return EXIT_SUCCESS;
}

// write function to output the histogram

/*************************************************************************
 * Main function to perform/test functionality
 *************************************************************************/
int main(int argc, char * argv[])
{
	
	// list desired inputs
	const char * fixedImageFilename = argv[1];
	const char * movingImageFilename = argv[2];
	//const char * fixedFiducialList = argv[3];
	//const char * movingFiducialList = argv[4];
	std::string outputDirectory = argv[5];
	std::string outputFileFormat = argv[5];
	const char * transformFilename = "initializedRigidTransform.txt";

	// list desired outputs
	std::string rigidResultFilename = outputDirectory + "\rigidResult." + outputFileFormat;
	std::string rigidTransformFilename = outputDirectory + "\rigidTransformParameters.txt";
	std::string deformableResultFilename = outputDirectory + "\deformableResult." + outputFileFormat;
	std::string deformableTransformFilename = outputDirectory + "\deformableTransformParameters.txt";
	std::string deformationFilename = outputDirectory + "deformation." + outputFileFormat;
	std::string jacobianMapFilename = outputDirectory + "jacobianMap." + outputFileFormat;

	// read in necessary files
	const unsigned int	Dimension = 3;
	typedef short		PixelType;

	typedef itk::Image< PixelType, Dimension >	FixedImageType;
	typedef itk::Image< PixelType, Dimension >	MovingImageType;

	FixedImageType::Pointer fixedImage = ReadInImage<FixedImageType>(fixedImageFilename);
	MovingImageType::Pointer movingImage = ReadInImage<MovingImageType>(movingImageFilename);

	// insert preprocessing steps
	//	- inhomogeneity correction
	//  - generation of mask files
	//  - etc.

	// set up rigid transform with initialization
	typedef itk::VersorRigid3DTransform< double >	RigidTransformType;
	RigidTransformType::Pointer rigidTransform = RigidTransformType::New();

	typedef itk::CenteredTransformInitializer< RigidTransformType, FixedImageType, MovingImageType >	InitializerType;
	InitializerType::Pointer initializer = InitializerType::New();
	
	initializer->SetTransform( rigidTransform );
	initializer->SetFixedImage( fixedImage );
	initializer->SetMovingImage( movingImage );
	initializer->GeometryOn();
	initializer->InitializeTransform();

	// write out transform after initialization
	//WriteOutTransform< RigidTransformType >( transformFilename , rigidTransform );

	// set up the metric
	//typedef itk::MattesMutualInformationImageToImageMetricv4< FixedImageType, MovingImageType >	MetricType;
	//MetricType::Pointer metric = MetricType::New();

	CreateHistogram< MovingImageType >( movingImage );

	return EXIT_SUCCESS;
}