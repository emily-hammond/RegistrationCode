@echo #
@echo # This batch file runs the rigid registration
@echo # between images and then performs the thresholding
@echo # process created to isolate the skeleton
@echo # from CT images.
@echo #

::cd C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SPIEanalysis\bld\RigidRegistration\Release 

:: H:\RegistrationResults\SampleData\phantom.mhd 

::RigidRegistration.exe AP20375 2 > C:\Experiments\SPIEMhdFiles\AP20375\Results\TP2Reglog.txt 2>&1

cd C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SPIEanalysis\bld\IsolateSkeleton\Release 
::cd C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SPIEanalysis\bld\Release

IsolateSkeleton.exe AP20375 2 > C:\Experiments\SPIEMhdFiles\AP20375\Results\TP2Skellog.txt 2>&1
IsolateSkeleton.exe AP20375 3 > C:\Experiments\SPIEMhdFiles\AP20375\Results\TP3Skellog.txt 2>&1

::cd C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SPIEanalysis