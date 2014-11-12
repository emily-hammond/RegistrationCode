@echo #
@echo # This batch file runs the rigid registration
@echo # between images and then performs the thresholding
@echo # process created to isolate the skeleton
@echo # from CT images.
@echo #

cd C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SPIEanalysis\bld\RigidRegistration\Release 

RigidRegistration.exe C:\Experiments\SPIEMhdFiles\AP20373\T3\T3_AP20373.mhd C:\Experiments\SPIEMhdFiles\AP20373\T2\T2_AP20373.mhd C:\Experiments\SPIEMhdFiles\Results\Test > C:\Experiments\SPIEMhdFiles\Results\Test\log.txt

cd C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SPIEanalysis
::cd C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SPIEanalysis\bld\Release

::IsolatedSkeleton.exe C:\Experiments\SPIEMhdFiles\AP20373\T1\T1_AP20373.mhd C:\Experiments\SPIEMhdFiles\AP20373\Output 2 > C:\Experiments\SPIEMhdFiles\AP20373\Output\log.txt

::cd C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SPIEanalysis