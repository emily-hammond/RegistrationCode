@echo #
@echo # This batch file runs the registration 
@echo # process created with ITK from the folder
@echo # C:\Users\ehammond\Documents\ITKprojects\
@echo # RegistrationCode\AlignmentPipeline
@echo #

:: make new directory for the output images
mkdir H:\RegistrationResults\2014.11\Output_1

:: run the registration file :)
cd C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\AlignmentPipeline\bld\Release
main.exe H:\RegistrationResults\2014.11\Input\J3_CTLung_0.5mm.mhd H:\RegistrationResults\2014.11\Input\J3_T2Var155AxNav.mhd H:\RegistrationResults\2014.11\Output_1 100 1000 > H:\RegistrationResults\2014.11\Output_1\logFile.txt 2>&1

:: create the histogram image with Matlab
:: change path to MATLAB executable
::cd C:\Program Files\MATLAB\R2012a\bin
::matlab -nosplash -minimize -r plotHistogram('H:\RegistrationResults\2014.11\Output_1\movingHistogram.txt','H:\RegistrationResults\2014.11\Output_1\movingHistogram.tif')

::matlab -nosplash -minimize -r plotHistogram('H:\RegistrationResults\2014.11\Output_1\fixedHistogram.txt','H:\RegistrationResults\2014.11\Output_1\fixedHistogram.tif')

cd C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\AlignmentPipeline