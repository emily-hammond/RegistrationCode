@echo #
@echo # This batch file runs the rigid registration
@echo # between images and then performs the thresholding
@echo # process created to isolate the skeleton
@echo # from CT images.
@echo #

:: delete/create all the proper results directories
::IF EXIST "C:\Experiments\SPIEMhdFiles\AP20373\Results" ( rmdir "C:\Experiments\SPIEMhdFiles\AP20373\Results" /s /q )
::mkdir C:\Experiments\SPIEMhdFiles\AP20373\Results

::IF EXIST "C:\Experiments\SPIEMhdFiles\AP20374\Results" ( rmdir "C:\Experiments\SPIEMhdFiles\AP20374\Results" /s /q )
::mkdir C:\Experiments\SPIEMhdFiles\AP20374\Results

::IF EXIST "C:\Experiments\SPIEMhdFiles\AP20375\Results" ( rmdir "C:\Experiments\SPIEMhdFiles\AP20375\Results" /s /q )
::mkdir C:\Experiments\SPIEMhdFiles\AP20375\Results

::IF EXIST "C:\Experiments\SPIEMhdFiles\AP20376\Results" ( rmdir "C:\Experiments\SPIEMhdFiles\AP20376\Results" /s /q )
::mkdir C:\Experiments\SPIEMhdFiles\AP20376\Results

::IF EXIST "C:\Experiments\SPIEMhdFiles\AP20377\Results" ( rmdir "C:\Experiments\SPIEMhdFiles\AP20377\Results" /s /q )
::mkdir C:\Experiments\SPIEMhdFiles\AP20377\Results

:: enter registration code directory
cd C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SPIEanalysis\bld\RigidRegistration\Release 

:: RigidRegistration.exe animalID# movingTP > logFile

:: run registrations for AP20373
RigidRegistration.exe AP20373 2 > C:\Experiments\SPIEMhdFiles\AP20373\Results\TP2Reglog.txt 2>&1
RigidRegistration.exe AP20373 3 > C:\Experiments\SPIEMhdFiles\AP20373\Results\TP3Reglog.txt 2>&1
RigidRegistration.exe AP20373 4 > C:\Experiments\SPIEMhdFiles\AP20373\Results\TP4Reglog.txt 2>&1
RigidRegistration.exe AP20373 5 > C:\Experiments\SPIEMhdFiles\AP20373\Results\TP5Reglog.txt 2>&1

:: run registrations for AP20374
RigidRegistration.exe AP20374 2 > C:\Experiments\SPIEMhdFiles\AP20374\Results\TP2Reglog.txt 2>&1
RigidRegistration.exe AP20374 3 > C:\Experiments\SPIEMhdFiles\AP20374\Results\TP3Reglog.txt 2>&1
RigidRegistration.exe AP20374 4 > C:\Experiments\SPIEMhdFiles\AP20374\Results\TP4Reglog.txt 2>&1
RigidRegistration.exe AP20374 5 > C:\Experiments\SPIEMhdFiles\AP20374\Results\TP5Reglog.txt 2>&1

:: run registrations for AP20375
RigidRegistration.exe AP20375 2 > C:\Experiments\SPIEMhdFiles\AP20375\Results\TP2Reglog.txt 2>&1
RigidRegistration.exe AP20375 3 > C:\Experiments\SPIEMhdFiles\AP20375\Results\TP3Reglog.txt 2>&1
RigidRegistration.exe AP20375 4 > C:\Experiments\SPIEMhdFiles\AP20375\Results\TP4Reglog.txt 2>&1
RigidRegistration.exe AP20375 5 > C:\Experiments\SPIEMhdFiles\AP20375\Results\TP5Reglog.txt 2>&1

:: run registrations for AP20376
RigidRegistration.exe AP20376 2 > C:\Experiments\SPIEMhdFiles\AP20376\Results\TP2Reglog.txt 2>&1
RigidRegistration.exe AP20376 3 > C:\Experiments\SPIEMhdFiles\AP20376\Results\TP3Reglog.txt 2>&1
RigidRegistration.exe AP20376 4 > C:\Experiments\SPIEMhdFiles\AP20376\Results\TP4Reglog.txt 2>&1
RigidRegistration.exe AP20376 5 > C:\Experiments\SPIEMhdFiles\AP20376\Results\TP5Reglog.txt 2>&1

:: run registrations for AP20377
RigidRegistration.exe AP20377 2 > C:\Experiments\SPIEMhdFiles\AP20377\Results\TP2Reglog.txt 2>&1
RigidRegistration.exe AP20377 3 > C:\Experiments\SPIEMhdFiles\AP20377\Results\TP3Reglog.txt 2>&1
RigidRegistration.exe AP20377 4 > C:\Experiments\SPIEMhdFiles\AP20377\Results\TP4Reglog.txt 2>&1

:: enter in isolate skeleton directory
cd C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SPIEanalysis\bld\IsolateSkeleton\Release 

:: IsolateSkeleton.exe animalID# TP > logFile

:: isolate skeletons for AP20373
IsolateSkeleton.exe AP20373 1 > C:\Experiments\SPIEMhdFiles\AP20373\Results\TP1Skellog.txt 2>&1
IsolateSkeleton.exe AP20373 2 > C:\Experiments\SPIEMhdFiles\AP20373\Results\TP2Skellog.txt 2>&1
IsolateSkeleton.exe AP20373 3 > C:\Experiments\SPIEMhdFiles\AP20373\Results\TP3Skellog.txt 2>&1
IsolateSkeleton.exe AP20373 4 > C:\Experiments\SPIEMhdFiles\AP20373\Results\TP4Skellog.txt 2>&1
IsolateSkeleton.exe AP20373 5 > C:\Experiments\SPIEMhdFiles\AP20373\Results\TP5Skellog.txt 2>&1

:: isolate skeletons for AP20374
IsolateSkeleton.exe AP20374 1 > C:\Experiments\SPIEMhdFiles\AP20374\Results\TP1Skellog.txt 2>&1
IsolateSkeleton.exe AP20374 2 > C:\Experiments\SPIEMhdFiles\AP20374\Results\TP2Skellog.txt 2>&1
IsolateSkeleton.exe AP20374 3 > C:\Experiments\SPIEMhdFiles\AP20374\Results\TP3Skellog.txt 2>&1
IsolateSkeleton.exe AP20374 4 > C:\Experiments\SPIEMhdFiles\AP20374\Results\TP4Skellog.txt 2>&1
IsolateSkeleton.exe AP20374 5 > C:\Experiments\SPIEMhdFiles\AP20374\Results\TP5Skellog.txt 2>&1

:: isolate skeletons for AP20375
IsolateSkeleton.exe AP20375 1 > C:\Experiments\SPIEMhdFiles\AP20375\Results\TP1Skellog.txt 2>&1
IsolateSkeleton.exe AP20375 2 > C:\Experiments\SPIEMhdFiles\AP20375\Results\TP2Skellog.txt 2>&1
IsolateSkeleton.exe AP20375 3 > C:\Experiments\SPIEMhdFiles\AP20375\Results\TP3Skellog.txt 2>&1
IsolateSkeleton.exe AP20375 4 > C:\Experiments\SPIEMhdFiles\AP20375\Results\TP4Skellog.txt 2>&1
IsolateSkeleton.exe AP20375 5 > C:\Experiments\SPIEMhdFiles\AP20375\Results\TP5Skellog.txt 2>&1

:: isolate skeletons for AP20376
IsolateSkeleton.exe AP20376 1 > C:\Experiments\SPIEMhdFiles\AP20376\Results\TP1Skellog.txt 2>&1
IsolateSkeleton.exe AP20376 2 > C:\Experiments\SPIEMhdFiles\AP20376\Results\TP2Skellog.txt 2>&1
IsolateSkeleton.exe AP20376 3 > C:\Experiments\SPIEMhdFiles\AP20376\Results\TP3Skellog.txt 2>&1
IsolateSkeleton.exe AP20376 4 > C:\Experiments\SPIEMhdFiles\AP20376\Results\TP4Skellog.txt 2>&1
IsolateSkeleton.exe AP20376 5 > C:\Experiments\SPIEMhdFiles\AP20376\Results\TP5Skellog.txt 2>&1

:: isolate skeletons for AP20377
IsolateSkeleton.exe AP20377 1 > C:\Experiments\SPIEMhdFiles\AP20377\Results\TP1Skellog.txt 2>&1
IsolateSkeleton.exe AP20377 2 > C:\Experiments\SPIEMhdFiles\AP20377\Results\TP2Skellog.txt 2>&1
IsolateSkeleton.exe AP20377 3 > C:\Experiments\SPIEMhdFiles\AP20377\Results\TP3Skellog.txt 2>&1
IsolateSkeleton.exe AP20377 4 > C:\Experiments\SPIEMhdFiles\AP20377\Results\TP4Skellog.txt 2>&1
