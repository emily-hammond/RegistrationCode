Filename: VersorToAffine.cxx

This code was written for Multidimensional Image Processing (MDIP) taught by Dr. Hans Johnson in Spring 2014. The goal is to register CT/MR images with a rigid transform (versor df:6) and an affine transform (df:12) if desired with the optional use of masks.



Call function:
VersorToAffine.exe fixedFilename movingFilename mask fixedMaskFilename movingMaskFilename outputFilename rigid fiducialFilename

fixedFilename -> fixed image filename (include path)
movingFilename -> moving image filename (include path)
mask -> 1 if using masks
fixedMaskFilename -> mask image corresponding to the fixed image (leave blank if mask = 0)
movingMaskFilename -> mask image corresponding to the moving image (leave blank if mask = 0)
outputFilename -> desired final image filename (include path)
rigid -> 1 if only desiring rigid registration (not affine)
fiducialFilename -> cvs filename that contains the fiducial locations for the corina, base of heart and top of aorta for each of the moving and fixed images (THIS FILE HAS A SPECIFIC FORMAT!!!! PAY ATTENTION!!!)



Flow of code: ITKv3
- Declarations
- Read in fixed/moving images
- Initialize components of registration: VersorRigid3DTransform, VersorRigid3DTransformOptimizer, LinearInterpolateImageFunction, MattesMutualInformationImageToImageMetric
- Initialize registration with CenteredTransformInitializer
- Read in image masks if desired
- Set up metric and optimizer
- Initialize components of affine registration: AffineTransform, RegularStepGradientDescentOptimizer
- Set up transform and optimizer
- Perform affine registration if desired
- Set up resample image filter
- Apply proper transform(s)
- Write out final moving image
- Read in fiducial file
- Parse through file to find the name of of the fixed/moving images
- Transform fiducials and calculate the SSD