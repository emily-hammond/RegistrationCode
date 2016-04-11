@echo off
setlocal EnableDelayedExpansion
REM This batch file is to register the upper right lung of animal models
REM with a fixed time point of TP2.

set FTP=2

REM directories for executable, transforms, output text files, CT masks, MR masks
set RegExDir=C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\BuildDirectory\MultiLevelRegistration-bld\MultiLevelRegistration\Release
set ValExDir=C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\BuildDirectory\MultiLevelRegistration-bld\Validation\Release
For /f "tokens=2-4 delims=/ " %%a in ('date /t') do (set mydate=%%c-%%a-%%b)
set txtDir=H:\Results\Data\Output\%mydate%
set transformDir=H:\Results\Data\Transforms\UpperRightLungAlignment\FTP%FTP%
set CTImageDir=C:\Experiments\MHDdata\CAP_WholeBody_B35f
set MRImageDir=C:\Experiments\MHDdata\Vibe3D_BH_Chest
set CTMaskDir5=H:\Results\Data\MaskFiles\5_Lungs\CAP_30_B35f\MHD
set CTMaskDir6=H:\Results\Data\MaskFiles\6_Airways\CAP_30_B35f\Eroded
set MRMaskDir=H:\Results\Data\MaskFiles\6_Airways\Vibe3D_BH_Chest

REM create output directory if it does not exist
if not exist "%txtDir%\NUL" mkdir %txtDir%
if not exist "%txtDir%\NUL" mkdir %transformDir%

REM ROIs
set ROI2=H:\Results\Data\MaskFiles\5_Lungs\ROIs\Lung_rect\%UID%_TP%FTP%_CAP_Lung.acsv
set ROI3=H:\Results\Data\MaskFiles\5_Lungs\ROIs\UpperRightLung_rect\%UID%_TP%FTP%_CAP_UpperRightLung.acsv

REM defaults
set NoOfLevels=3
REM initialization
set center=1
set metricX=0
set metricY=0
set metricZ=1
set manualTransform=[]
REM registration parameters
set rotationScale=0.001
set translationScale=10.0
set scalingScale=0.001
set numberOfIterations=500
set maximumStepLength=4
set minimumStepLength=0.001
set relaxationFactor=0.5
set gradientMagnitudeTolerance=0.001
REM other
set observe=0
set skipWB=0
set debug=0

REM perform registrations for AP20373
set UID=AP20373
set fixedCTImage=%CTImageDir%\%UID%_TP%FTP%_CAP_30_B35f.mhd
set fixedCTMask=[]
set movingMask=[]

REM iterate through all potential TPs and perform CT-CT registration
FOR /l %%T in (1,1,7) do (
	echo MOVING TP = %%T
	IF NOT %%T==%FTP% ( 
		IF EXIST %fixedCTImage% (
			REM perform registration if the moving image exists
			set movingImage=%CTImageDir%\%UID%_TP%%T_CAP_30_B35f.mhd
			IF EXIST !movingImage! (
				REM "%RegExDir%\MultiLevelRegistration.exe" %transformDir%\%UID%_TP!TP!_CAP!TP!-F%FTP% %fixedCTImage% !movingImage! !fixedCTMask! !movingMask! %NoOfLevels% %ROI2% %ROI3% %observe% %manualTransform% %center% %metricX% %metricY% %metricZ% %rotationScale% %translationScale% %scalingScale% %numberOfIterations% %maximumStepLength% %minimumStepLength% %relaxationFactor% %gradientMagnitudeTolerance% %skipWB% %debug% > "%txtDir%\%UID%_TP%%T_CAP%%T-F%FTP%_%mydate%_registration.txt" 2>&1
				echo CT-CT REGISTRATION PERFORMED
				)
			REM validation
			REM label 5
			set label=5
			set fixedCTMask=%CTMaskDir5%\%UID%_TP%FTP%_CAP_30_B35f-!label!-label.mhd
			set movingMask=%CTMaskDir5%\%UID%_TP%%T_CAP_30_B35f-!label!-label.mhd
			set initialTransform=%transformDir%\%UID%_TP%%T_CAP%%T-F%FTP%_InitialTransform.tfm
			set level1Transform=%transformDir%\%UID%_TP%%T_CAP%%T-F%FTP%_Level1CompositeTransform.tfm
			set level2Transform=%transformDir%\%UID%_TP%%T_CAP%%T-F%FTP%_Level2CompositeTransform.tfm
			set level3Transform=%transformDir%\%UID%_TP%%T_CAP%%T-F%FTP%_Level3CompositeTransform.tfm
			IF EXIST !fixedCTMask! ( 
				IF EXIST !movingMask! ( 
					IF EXIST !initialTransform! ( 
						IF EXIST !level3Transform! ( 
							IF EXIST !level2Transform! ( 
								IF EXIST !level1Transform! ( 
									REM "%ValExDir%\Validation.exe" %fixedCTImage% !fixedCTMask! %movingImage% !movingMask! !initialTransform! !level1Transform! !level2Transform! %ROI2% !level3Transform! %ROI3% > "%txtDir%\%UID%_TP%%T_CAP%%T-F%FTP%_%mydate%_validation-!label!.txt" 2>&1
									echo VALIDATION WITH LEVEL 3 TRANSFORM for label !label!
									set trans=3
									)
								)
							)
						IF NOT EXIST !level3Transform! (
							IF EXIST !level2Transform! (
								IF EXIST !level1Transform! (
									REM "%ValExDir%\Validation.exe" %fixedCTImage% !fixedCTMask! %movingImage% !movingMask! !initialTransform! !level1Transform! !level2Transform! %ROI2%> "%txtDir%\%UID%_TP%%T_CAP%%T-F%FTP%_%mydate%_validation-!label!.txt" 2>&1
									echo VALIDATION WITH LEVEL 2 TRANSFORM for label !label!
									set trans=2
									)
								)
							)
						IF NOT EXIST !level3Transform (
							IF NOT EXIST !level2Transform! (
								IF EXIST !level1Transform! (
									REM "%ValExDir%\Validation.exe" %fixedCTImage% !fixedCTMask! %movingImage% !movingMask! !initialTransform! !level1Transform! > "%txtDir%\%UID%_TP%%T_CAP%%T-F%FTP%_%mydate%_validation-!label!.txt" 2>&1
									echo VALIDATION WITH LEVEL 1 TRANSFORM for label !label!
									set trans=1
									)
								)
							)
						)	
					)
				)
			REM label 6
			set label=6
			set fixedCTMask=%CTMaskDir6%\%UID%_TP%FTP%_CAP_30_B35f-!label!-label.mhd
			set movingMask=%CTMaskDir6%\%UID%_TP%%T_CAP_30_B35f-!label!-label.mhd
			set initialTransform=%transformDir%\%UID%_TP%%T_CAP%%T-F%FTP%_InitialTransform.tfm
			set level1Transform=%transformDir%\%UID%_TP%%T_CAP%%T-F%FTP%_Level1CompositeTransform.tfm
			set level2Transform=%transformDir%\%UID%_TP%%T_CAP%%T-F%FTP%_Level2CompositeTransform.tfm
			set level3Transform=%transformDir%\%UID%_TP%%T_CAP%%T-F%FTP%_Level3CompositeTransform.tfm
			IF EXIST !fixedCTMask! ( 
				IF EXIST !movingMask! ( 
					IF EXIST !initialTransform! ( 
						IF EXIST !level3Transform! ( 
							IF EXIST !level2Transform! ( 
								IF EXIST !level1Transform! ( 
									REM "%ValExDir%\Validation.exe" %fixedCTImage% !fixedCTMask! %movingImage% !movingMask! !initialTransform! !level1Transform! !level2Transform! %ROI2% !level3Transform! %ROI3% > "%txtDir%\%UID%_TP%%T_CAP%%T-F%FTP%_%mydate%_validation-!label!.txt" 2>&1
									echo VALIDATION WITH LEVEL 3 TRANSFORM for label !label!
									)
								)
							)
						IF NOT EXIST !level3Transform! (
							IF EXIST !level2Transform! (
								IF EXIST !level1Transform! (
									REM "%ValExDir%\Validation.exe" %fixedCTImage% !fixedCTMask! %movingImage% !movingMask! !initialTransform! !level1Transform! !level2Transform! %ROI2%> "%txtDir%\%UID%_TP%%T_CAP%%T-F%FTP%_%mydate%_validation-!label!.txt" 2>&1
									echo VALIDATION WITH LEVEL 2 TRANSFORM for label !label!
									)
								)
							)
						IF NOT EXIST !level3Transform (
							IF NOT EXIST !level2Transform! (
								IF EXIST !level1Transform! (
									REM "%ValExDir%\Validation.exe" %fixedCTImage% !fixedCTMask! %movingImage% !movingMask! !initialTransform! !level1Transform! > "%txtDir%\%UID%_TP%%T_CAP%%T-F%FTP%_%mydate%_validation-!label!.txt" 2>&1
									echo VALIDATION WITH LEVEL 1 TRANSFORM for label !label!
									)
								)
							)
						)	
					)
				)
			)
		)
	echo COMPLETE
	)
	
REM repeat for MR-CT registrations
FOR /l %%T in (1,1,7) do (
	echo MOVING TP = %%T
	set fixedImage=%CTImageDir%\%UID%_TP%%T_CAP_30_B35f.mhd
	set initialFixedTransform=%transformDir%\%UID%_TP%%T_CAP%%T-F%FTP%_Level!trans!CompositeTransform.tfm
	IF NOT %%T==%FTP% ( 
		IF EXIST !fixedImage! (
			IF EXIST !initialFixedTransform! (
				REM perform registration if the moving image exists
				set movingImage=%MRImageDir%\%UID%_TP%%T_VIBE_BH_Chest.mhd
				IF EXIST !movingImage! (
					REM "%RegExDir%\MultiLevelRegistration.exe" %transformDir%\%UID%_TP!TP!_CAP!TP!-F%FTP% !fixedImage! !movingImage! !fixedCTMask! !movingMask! %NoOfLevels% %ROI2% %ROI3% %observe% %manualTransform% %center% %metricX% %metricY% %metricZ% %rotationScale% %translationScale% %scalingScale% %numberOfIterations% %maximumStepLength% %minimumStepLength% %relaxationFactor% %gradientMagnitudeTolerance% %skipWB% %debug% !initialFixedTransform! %fixedCTImage% > "%txtDir%\%UID%_TP%%T_CAP%%T-F%FTP%_%mydate%_registration.txt" 2>&1
					echo CT-MR REGISTRATION PERFORMED
					)
				REM validation
				REM label 6
				set label=6
				set fixedMask=%CTMaskDir6%\%UID%_TP%FTP%_CAP_30_B35f-!label!-label.mhd
				set movingMask=%MRMaskDir%\%UID%_TP%%T_CAP_30_B35f-!label!-label.mhd
				set initialTransform=%transformDir%\%UID%_TP%%T_CAP%%T-F%FTP%_InitialTransform.tfm
				set level1Transform=%transformDir%\%UID%_TP%%T_CAP%%T-F%FTP%_Level1CompositeTransform.tfm
				set level2Transform=%transformDir%\%UID%_TP%%T_CAP%%T-F%FTP%_Level2CompositeTransform.tfm
				set level3Transform=%transformDir%\%UID%_TP%%T_CAP%%T-F%FTP%_Level3CompositeTransform.tfm
				IF EXIST !fixedCTMask! ( 
					IF EXIST !movingMask! ( 
						IF EXIST !initialTransform! ( 
							IF EXIST !level3Transform! ( 
								IF EXIST !level2Transform! ( 
									IF EXIST !level1Transform! ( 
										REM "%ValExDir%\Validation.exe" !fixedImage! !fixedMask! %movingImage% !movingMask! !initialTransform! !level1Transform! !level2Transform! %ROI2% !level3Transform! %ROI3% %initialFixedTransform% %fixedCTImage% > "%txtDir%\%UID%_TP%%T_CAP%%T-F%FTP%_%mydate%_validation-!label!.txt" 2>&1
										echo VALIDATION WITH LEVEL 3 TRANSFORM for label !label!
										)
									)
								)
							IF NOT EXIST !level3Transform! (
								IF EXIST !level2Transform! (
									IF EXIST !level1Transform! (
										REM "%ValExDir%\Validation.exe" !fixedImage! !fixedMask! %movingImage% !movingMask! !initialTransform! !level1Transform! !level2Transform! %ROI2% [] [] %initialFixedTransform% %fixedCTImage% > "%txtDir%\%UID%_TP%%T_CAP%%T-F%FTP%_%mydate%_validation-!label!.txt" 2>&1
										echo VALIDATION WITH LEVEL 2 TRANSFORM for label !label!
										)
									)
								)
							IF NOT EXIST !level3Transform (
								IF NOT EXIST !level2Transform! (
									IF EXIST !level1Transform! (
										REM "%ValExDir%\Validation.exe" !fixedImage! !fixedMask! %movingImage% !movingMask! !initialTransform! !level1Transform! [] [] [] [] %initialFixedTransform% %fixedCTImage% > "%txtDir%\%UID%_TP%%T_CAP%%T-F%FTP%_%mydate%_validation-!label!.txt" 2>&1
										echo VALIDATION WITH LEVEL 1 TRANSFORM for label !label!
										)
									)
								)
							)	
						)
					)
				)
			)
		)
	echo COMPLETE
	)
	
	
	