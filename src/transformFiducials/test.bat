cd /d C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\bin\transformFiducials\Release

:: transformFiducials.exe fixedLandmarks movingLandmarks transformFilename transformFilename resultsFile transformedLandmarksFilename

:: insert all the results into 
:: C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SampleData\transformFiducialsResults

transformFiducials.exe C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SampleData\TP1.fcsv C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SampleData\TP4.fcsv C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SampleData\transformFiducialsResults\affineTransform1_4.tfm C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SampleData\transformFiducialsResults\landmarkResults.csv  C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SampleData\transformFiducialsResults\TP4trans.fcsv > C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SampleData\transformFiducialsResults\log.txt 2>&1

:: C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\SampleData\transformFiducialsResults\bsplineTransform1_4.tfm