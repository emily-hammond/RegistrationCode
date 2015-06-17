Filename: itkN4BiasFieldCorrectionImageFilterTest.cxx
This file was taken from the ITK source code. Code had never been used.

****************************************************

Filename: investigateFiducials.cxx

This code was written to investigate the relationship between fiducials located with the Slicer GUI and how they related to ITK coordinates.

Call function:
investigateFiducials.exe inputImage index0 index1 index 2

Flow of code:
- Declarations
- Read in image
- Define index location based on inputs
- Obtain pixel value at that index location
- Identify the corresponding physical location and output

****************************************************

Filename: ImageHistogram3.cxx
This file was taken from ITK source code to learn how to create a histogram. It is used in conjunction with createHistogram.m, a MATLAB script that takes the output text file and creates a picture of the histogram.

****************************************************

Filename: parseInputFile.cxx (itkParseInputFile.hxx, itkParseInputFile.h)

This code was written to test the parseInputFile class created to parse a file that contains all the input information required for the registration defined in alignmentPipeline.

NOTE: This will be null with the integration with Slicer

Call function:
parseInputFile.exe filename

Flow of code:
- Declarations
- Input filename to parseFile object
-- Set default values
-- Scan line by line for specific words to specific where the info should be stored
-- Put proper value into the variable
-- Derive necessary variables
- Print out results
-- Print everything to terminal

****************************************************

Filename: ReadWriteFunction.cxx

This code was written to create functions that perform read and writing of files with code. It provides a central location where these are defined to locate and use.

Call function:
ReadWriteFunctions.exe inputFilename outputFilename fiducialFilename

Flow of code:
- Declarations
- Functions
-- Read in an image
-- Write out an image
-- Read a fiducial filename from Slicer
-- Print out fiducials in a list
-- Write out a transform
- Create image and landmarks list
- Read in images and fiducials
- Transform fiducials into indices (corina, aorta, baseofheart)
- Set the pixels in the image at those locations to HU=2000
- Write out image

******************************************************

Filename: transformFiducials.cxx (itkLandmarkAnalysis.hxx, itkLandmarkAnalysis.h)

This code was written to perform basic statistics and compare the fiducials from the fixed image and transformed moving image. It applies the series of transforms to the moving image landmarks and compares the final landmarks with the moving landmarks.

Call function:
transformFiducials.exe fixedLandmarksFilename movingLandmarksFilename transformFilename

Flow of code:
- Declarations
- Feed filenames to landmark analysis object
-- Parse through landmark file and store landmarks into list
-- Store transform into list of transforms
- Perform statistics
-- Transform landmarks by iterating through the transform list
-- Convert affine transform to RAS coordinates from LPS coordinates
-- Apply transform to landmarks
-- DOES NOT WORK WITH DEFORMABLE TRANSFORMS YET
-- Compute SSD of each set of landmarks
- Print the results
-- Open a file to write to (csv)
-- Output label headings
-- Write out results to the file
-- Calculate mean and stdev of SSD of landmarks and write to file

test.bat was written to run some sample data through the program and test its functionality.