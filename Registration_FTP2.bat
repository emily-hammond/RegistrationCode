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

set NoOfLevels=3

set center=1
set metricX=0
set metricY=0
set metricZ=1
set manualTransform=[]

set rotationScale=0.001
set translationScale=10.0
set scalingScale=0.001
set numberOfIterations=500
set maximumStepLength=4
set minimumStepLength=0.001
set relaxationFactor=0.5
set gradientMagnitudeTolerance=0.001

set observe=0
set skipWB=0
set debug=0

REM Animal
REM set UID=AP20373 AP20374 AP20375 
FOR %%U in (AP20376 AP20377 AP20415) do (
	echo _
	echo %%U

	set ROI2=H:\Results\Data\MaskFiles\5_Lungs\ROIs\Lung_rect\%%U_TP%FTP%_CAP_Lung.acsv
	set ROI3=H:\Results\Data\MaskFiles\5_Lungs\ROIs\UpperRightLung_rect\%%U_TP%FTP%_CAP_UpperRightLung.acsv

	set fixedCTImage=%CTImageDir%\%%U_TP%FTP%_CAP_30_B35f.mhd

	echo _
	echo *******************
	echo CT-CT Registrations
	FOR /l %%T in (1,1,7) do (
		echo MOVING TP = %%T
		IF NOT %%T==%FTP% ( 
			IF EXIST !fixedCTImage! (
				set fixedCTMask=[]
				set movingMask=[]

				IF %%T==7 (
					set movingImage=%CTImageDir%\%%U_TP%%T_CAP_30_Qr40.mhd
					)
				IF NOT %%T==7 (
					set movingImage=%CTImageDir%\%%U_TP%%T_CAP_30_B35f.mhd
					)
				IF NOT EXIST !movingImage! (
					set movingImage=%CTImageDir%\%%U_TP%%T_CAP_30_B35f.mhd
					)
				echo !movingImage!
				IF EXIST !movingImage! (
					"%RegExDir%\MultiLevelRegistration.exe" %transformDir%\%%U_TP%%T_CAP%%T-F%FTP% !fixedCTImage! !movingImage! !fixedCTMask! !movingMask! %NoOfLevels% !ROI2! !ROI3! %observe% %manualTransform% %center% %metricX% %metricY% %metricZ% %rotationScale% %translationScale% %scalingScale% %numberOfIterations% %maximumStepLength% %minimumStepLength% %relaxationFactor% %gradientMagnitudeTolerance% %skipWB% %debug% > "%txtDir%\%%U_TP%%T_CAP%%T-F%FTP%_%mydate%_registration.txt" 2>&1
					echo CT-CT REGISTRATION PERFORMED
					) ELSE ( echo !movingImage! does not exist )

				set label=5
				set fixedCTMask5=%CTMaskDir5%\%%U_TP%FTP%_CAP_30_B35f-!label!-label.mhd
				IF %%T==7 (
					set movingMask5=%CTMaskDir5%\%%U_TP%%T_CAP_30_Qr40-!label!-label.mhd
					)
				IF NOT %%T==7 (
					set movingMask5=%CTMaskDir5%\%%U_TP%%T_CAP_30_B35f-!label!-label.mhd
					)
				IF NOT EXIST !movingMask5! (
					set movingMask5=%CTMaskDir5%\%%U_TP%%T_CAP_30_B35f-!label!-label.mhd
					)
				echo !movingMask5!
				set initialTransform=%transformDir%\%%U_TP%%T_CAP%%T-F%FTP%_InitialTransform.tfm
				set level1Transform=%transformDir%\%%U_TP%%T_CAP%%T-F%FTP%_Level1CompositeTransform.tfm
				set level2Transform=%transformDir%\%%U_TP%%T_CAP%%T-F%FTP%_Level2CompositeTransform.tfm
				set level3Transform=%transformDir%\%%U_TP%%T_CAP%%T-F%FTP%_Level3CompositeTransform.tfm
				IF EXIST !fixedCTMask5! ( 
					IF EXIST !movingMask5! ( 
						IF EXIST !initialTransform! ( 
							IF EXIST !level3Transform! ( 
								IF EXIST !level2Transform! ( 
									IF EXIST !level1Transform! ( 
										"%ValExDir%\Validation.exe" !fixedCTImage! !fixedCTMask5! !movingImage! !movingMask5! !initialTransform! !level1Transform! !level2Transform! !ROI2! !level3Transform! !ROI3! > "%txtDir%\%%U_TP%%T_CAP%%T-F%FTP%_%mydate%_validation-!label!.txt" 2>&1
										echo VALIDATION WITH LEVEL 3 TRANSFORM for label !label!
										set trans=3
										)
									)
								)
							IF NOT EXIST !level3Transform! (
								IF EXIST !level2Transform! (
									IF EXIST !level1Transform! (
										"%ValExDir%\Validation.exe" !fixedCTImage! !fixedCTMask5! !movingImage! !movingMask5! !initialTransform! !level1Transform! !level2Transform! !ROI2! > "%txtDir%\%%U_TP%%T_CAP%%T-F%FTP%_%mydate%_validation-!label!.txt" 2>&1
										echo VALIDATION WITH LEVEL 2 TRANSFORM for label !label!
										set trans=2
										)
									)
								)
							IF NOT EXIST !level3Transform (
								IF NOT EXIST !level2Transform! (
									IF EXIST !level1Transform! (
										"%ValExDir%\Validation.exe" !fixedCTImage! !fixedCTMask5! !movingImage! !movingMask5! !initialTransform! !level1Transform! > "%txtDir%\%%U_TP%%T_CAP%%T-F%FTP%_%mydate%_validation-!label!.txt" 2>&1
										echo VALIDATION WITH LEVEL 1 TRANSFORM for label !label!
										set trans=1
										)
									)
								)
							) ELSE ( echo !initialTransform! does not exist )
						) ELSE ( echo !movingMask5! does not exist )
					) ELSE ( echo !fixedCTMask5! does not exist )

				set label=6
				set fixedCTMask6=%CTMaskDir6%\%%U_TP%FTP%_CAP_30_B35f-!label!-label.mhd
				IF %%T==7 (
					set movingMask6=%CTMaskDir6%\%%U_TP%%T_CAP_30_Qr40-!label!-label.mhd
					)
				IF NOT %%T==7 (
					set movingMask6=%CTMaskDir6%\%%U_TP%%T_CAP_30_B35f-!label!-label.mhd
					)
				IF NOT EXIST !movingMask6! (
					set movingMask6=%CTMaskDir6%\%%U_TP%%T_CAP_30_B35f-!label!-label.mhd
					)
				echo !movingMask6!
				set initialTransform=%transformDir%\%%U_TP%%T_CAP%%T-F%FTP%_InitialTransform.tfm
				set level1Transform=%transformDir%\%%U_TP%%T_CAP%%T-F%FTP%_Level1CompositeTransform.tfm
				set level2Transform=%transformDir%\%%U_TP%%T_CAP%%T-F%FTP%_Level2CompositeTransform.tfm
				set level3Transform=%transformDir%\%%U_TP%%T_CAP%%T-F%FTP%_Level3CompositeTransform.tfm
				IF EXIST !fixedCTMask6! ( 
					IF EXIST !movingMask6! ( 
						IF EXIST !initialTransform! ( 
							IF EXIST !level3Transform! ( 
								IF EXIST !level2Transform! ( 
									IF EXIST !level1Transform! ( 
										"%ValExDir%\Validation.exe" !fixedCTImage! !fixedCTMask6! !movingImage! !movingMask6! !initialTransform! !level1Transform! !level2Transform! !ROI2! !level3Transform! !ROI3! > "%txtDir%\%%U_TP%%T_CAP%%T-F%FTP%_%mydate%_validation-!label!.txt" 2>&1
										echo VALIDATION WITH LEVEL 3 TRANSFORM for label !label!
										)
									)
								)
							IF NOT EXIST !level3Transform! (
								IF EXIST !level2Transform! (
									IF EXIST !level1Transform! (
										"%ValExDir%\Validation.exe" !fixedCTImage! !fixedCTMask6! !movingImage! !movingMask6! !initialTransform! !level1Transform! !level2Transform! !ROI2! > "%txtDir%\%%U_TP%%T_CAP%%T-F%FTP%_%mydate%_validation-!label!.txt" 2>&1
										echo VALIDATION WITH LEVEL 2 TRANSFORM for label !label!
										)
									)
								)
							IF NOT EXIST !level3Transform (
								IF NOT EXIST !level2Transform! (
									IF EXIST !level1Transform! (
										"%ValExDir%\Validation.exe" !fixedCTImage! !fixedCTMask6! !movingImage! !movingMask6! !initialTransform! !level1Transform! > "%txtDir%\%%U_TP%%T_CAP%%T-F%FTP%_%mydate%_validation-!label!.txt" 2>&1
										echo VALIDATION WITH LEVEL 1 TRANSFORM for label !label!
										)
									)
								)
							) ELSE ( echo !initialTransform! does not exist )
						) ELSE ( echo !movingMask6! does not exist )
					) ELSE ( echo !fixedCTMask6! does not exist )
				) ELSE ( echo !fixedCTImage! does not exist )
			)
		echo COMPLETE
		)
		
	echo _
	echo *******************
	echo CT-MR Registrations
	FOR /l %%T in (1,1,7) do (
		set fixedMask=[]
		set movingMask=[]
		echo MOVING TP = %%T
		set fixedImage=%CTImageDir%\%%U_TP%%T_CAP_30_B35f.mhd
		IF EXIST !fixedImage! (
			IF NOT %%T==%FTP% (
				set initialFixedTransform=%transformDir%\%%U_TP%%T_CAP%%T-F%FTP%_Level!trans!CompositeTransform.tfm
				) ELSE ( set initialFixedTransform=C:\ITK\InsightToolkit-4.8.0\Examples\Data\IdentityTransform.tfm )
			IF EXIST !initialFixedTransform! (

				set movingImage=%MRImageDir%\%%U_TP%%T_Vibe3D_BH_Chest.mhd
				IF EXIST !movingImage! (
					"%RegExDir%\MultiLevelRegistration.exe" %transformDir%\%%U_TP%%T_VIBE%%T-F%FTP% !fixedImage! !movingImage! !fixedMask! !movingMask! %NoOfLevels% !ROI2! !ROI3! %observe% %manualTransform% %center% %metricX% %metricY% %metricZ% %rotationScale% %translationScale% %scalingScale% %numberOfIterations% %maximumStepLength% %minimumStepLength% %relaxationFactor% %gradientMagnitudeTolerance% %skipWB% %debug% !initialFixedTransform! !fixedCTImage! > "%txtDir%\%%U_TP%%T_VIBE%%T-F%FTP%_%mydate%_registration.txt" 2>&1
					echo CT-MR REGISTRATION PERFORMED
					) ELSE ( echo !movingImage! does not exist )

				set label=6
				set fixedMask=%CTMaskDir6%\%%U_TP%%T_CAP_30_B35f-!label!-label.mhd
				set movingMask=%MRMaskDir%\%%U_TP%%T_Vibe3D_BH_Chest-!label!-label.mhd
				set initialTransform=%transformDir%\%%U_TP%%T_VIBE%%T-F%FTP%_InitialTransform.tfm
				set level1Transform=%transformDir%\%%U_TP%%T_VIBE%%T-F%FTP%_Level1CompositeTransform.tfm
				set level2Transform=%transformDir%\%%U_TP%%T_VIBE%%T-F%FTP%_Level2CompositeTransform.tfm
				set level3Transform=%transformDir%\%%U_TP%%T_VIBE%%T-F%FTP%_Level3CompositeTransform.tfm
				IF EXIST !fixedMask! ( 
					IF EXIST !movingMask! ( 
						IF EXIST !initialTransform! ( 
							IF EXIST !level3Transform! ( 
								IF EXIST !level2Transform! ( 
									IF EXIST !level1Transform! ( 
										"%ValExDir%\Validation.exe" !fixedImage! !fixedMask! !movingImage! !movingMask! !initialTransform! !level1Transform! !level2Transform! !ROI2! !level3Transform! !ROI3! !initialFixedTransform! !fixedCTImage! > "%txtDir%\%%U_TP%%T_VIBE%%T-F%FTP%_%mydate%_validation-!label!.txt" 2>&1
										echo VALIDATION WITH LEVEL 3 TRANSFORM for label !label!
										)
									)
								)
							IF NOT EXIST !level3Transform! (
								IF EXIST !level2Transform! (
									IF EXIST !level1Transform! (
										"%ValExDir%\Validation.exe" !fixedImage! !fixedMask! !movingImage! !movingMask! !initialTransform! !level1Transform! !level2Transform! !ROI2! [] [] !initialFixedTransform! !fixedCTImage! > "%txtDir%\%%U_TP%%T_VIBE%%T-F%FTP%_%mydate%_validation-!label!.txt" 2>&1
										echo VALIDATION WITH LEVEL 2 TRANSFORM for label !label!
										)
									)
								)
							IF NOT EXIST !level3Transform (
								IF NOT EXIST !level2Transform! (
									IF EXIST !level1Transform! (
										"%ValExDir%\Validation.exe" !fixedImage! !fixedMask! !movingImage! !movingMask! !initialTransform! !level1Transform! [] [] [] [] !initialFixedTransform! !fixedCTImage! > "%txtDir%\%%U_TP%%T_VIBE%%T-F%FTP%_%mydate%_validation-!label!.txt" 2>&1
										echo VALIDATION WITH LEVEL 1 TRANSFORM for label !label!
										)
									)
								)
							) ELSE ( echo !initialTransform! does not exist )
						) ELSE ( echo !movingMask! does not exist )
					) ELSE ( echo !fixedCTMask! does not exist )
				) ELSE ( echo !initialFixedTransform! does not exist )
			) ELSE ( echo !fixedImage! does not exist )
		echo COMPLETE
		)
)

echo _
echo **********************
echo DONE WITH BATCH SCRIPT
	
	