@echo #
@echo # This batch file runs the thresholding
@echo # process created to isolate the skeleton
@echo # from CT images.
@echo #

cd C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SPIEanalysis\bld\Release

IsolatedSkeleton.exe C:\Experiments\SPIEMhdFiles\AP20373\T1\T1_AP20373.mhd C:\Experiments\SPIEMhdFiles\AP20373\Output 2 > C:\Experiments\SPIEMhdFiles\AP20373\Output\log.txt

cd C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SPIEanalysis