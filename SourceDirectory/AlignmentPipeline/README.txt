Filename: main.cxx

This code was written to perform rigid registration on medical imaging data. It uses the MMI similarity metric and the versor transform (df:6). This code may requires the modified versor gd optimizer to output/monitor the gradient magnitude during the registration process

Call function:
main.exe inputFilenameContainingParameters

Flow of code:
- Declarations
- Functions
-- Read in image
-- Write out image
-- Write out transform
-- Create histogram
--- Define histogram filter
--- Calculate min/max of image
--- Determine # of bins
--- Set bin min/max values
--- Send image to histogram filter and create
--- Write histogram values out to file (txt)
-- Output things to string stream
--- Define stringstream and send out templated object
-- Determine physical range of image
--- Get image origin, spacing, size, index
--- Identify the location of the last pixel (index)
--- Transform this point to physical coordinates
--- Determine the end point FOV (last pixel + 0.5) -> return
--- Transform this point to physical coordinates
--- Determine the starting point FOV -> return
--- Transform this point to physical coordinates
--- Calculate the center FOV point -> return
- Functions
-- Output information from the optimizer during registration
- Start timing mechanism
- Parse through input file
- Read in images
- Create histograms
- Set up versor transform
- Perform center of geometry initialization
- Write out transform
- Set up linear interpolator
- Set up MMI metric (V3)
- Perform metric initialization
- Optain FOV of moving and fixed images
- Identify range in which to move the moving image (coronal or sagittal direction)
- Move image in specified direction
- Determine which location had the best metric alignment
- Write out transform
- Set up versor optimizer
- Set up registration method
- Perform registration
- Write out final transform
- Obtain joint PDF and write out
- Apply final transform to moving image
- Stop timer