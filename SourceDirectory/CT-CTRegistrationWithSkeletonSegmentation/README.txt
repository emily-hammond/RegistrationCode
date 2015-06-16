Filename: RigidRegistration.cxx

This code was written to rigidly register CT to CT images. It assumes TP1 is the fixed time point and asks for the animal tag and TP desired to be registered. It derives the location of the images and assumes they are in a set file location. (THIS PROBABLY HAS CHANGED!!!)

Call function:
RigidRegistration.exe UniversityID movingTP

Flow of code:
- Declarations
- Functions
-- Change optimizer step length after multiresolution levels
-- Output information from optimizer at each iteration
-- Read in images
-- Write out images
-- Write out transforms
- Derive input and output filenames based on UID and TP
- Define registration components: scaling transform (df:9), linear interpolator, RSGD optimizer, MMI, Multi-resolution approach
- Read in images
- Set up registration and perform initialization (center by geometry)
- Apply initialization to moving image (commented out to write the image out)
- Set up metric and optimizer
- Set up multi-resolution pyramids (set to level 1 currently)
- Perform registration
- Apply final transform to moving image and write to file
- Output final parameters

SPIEregistration.bat is written to mass run this program with all the data.

*********************************************************

Filename: IsolateSkeleton.cxx

This code was written to automatically segment out the skeleton in a CT scan. It is used in conjunction with the rigid registration code above.


Call function:
IsolateSkeleton.exe UniversityID TP

Flow of code:
- Declarations
- Functions
-- Read in images
-- Write out images
- Derive input/output filenames
- Read in image
- Find min/max pixel values in image
- Binary threshold the image >1200 (images stored as mhd images NOT dicom)
- Define structuring element
- Dilate thresholded image
- Isolate labels into individual connected regions
- Delete labels <1000 pixels in size
- Print out remaining labels
- Combine labels with size >20000
- Threshold image again to remove lower labels
- Erode final image
- Print out desired images

SPIE_isolateSkeletonsOfOriginalImages.bat and SPIE_isolateSkeletonsOfRegisteredImages.bat were written to mass run the segmentations on the original images and the registered images.