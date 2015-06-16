Filename: RigidToAffineRegistration.cxx

The code was written for Multidimensional Image Processing (MDIP) taught by Dr. Hans Johnson in Spring 2014. The goal is to register CT/MR images with a rigid transform (df:6) and then an affine transform (df:12) with the use of masks if desired. A slicer executable model was written to achieve Slicer3D (www.slicer.org) compatibility for the integration of a GUI.

Call function from terminal:
RigidToAffineRegistration
Long flags:
--fixedVolume
--movingVolume
--outputVolume -> registered moving image
--mask -> use of masks (binary)
--fixedMaskVolume
--movingMaskVolume


Flow of code: ITKv3
- Declarations
- Read in images
- Initialize rigid registration components: Versor transform, linear interpolator, MMI, versor optimizer
- Initialize transform with centered transform initializer
- Set up versor transform
- Set up MMI metric
- Read in mask images if desired
- Set up versor optimizer
- Perform rigid registration
- Set up affine transform
- Define regular step gradient descent optimizer and outline parameters
- Perform affine registration
- Resample moving image with final affine transform
- Write out final image