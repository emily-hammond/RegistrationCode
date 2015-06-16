@echo #
@echo # This batch file calls elastix to perform the temporal rigid registration
@echo # specified in rigid_SPIE_registration.txt. It registers 5 TP images to 
@echo # each other and places the output in the corresponding output directory.
@echo # Fixed Image is TP3.
@echo #
@echo # Directory - C:\Experiments\SPIEMhdFiles\AP20373

mkdir C:\Experiments\SPIEMhdFiles\AP20373\Output
cd /d C:\Users\ehammond\Documents\Elastix\bld\bin\Release

elastix -f0 C:\Experiments\SPIEMhdFiles\AP20373\T3\T3_AP20373.mhd -m0 C:\Experiments\SPIEMhdFiles\AP20373\T4\T4_AP20373.mhd -m1 C:\Experiments\SPIEMhdFiles\AP20373\T5\T5_AP20373.mhd -out C:\Experiments\SPIEMhdFiles\AP20373\Output -p C:\Experiments\SPIEMhdFiles\rigid_SPIE_registration.txt

@echo #
@echo # See C:\Experiments\SPIEMhdFiles\AP20373\Output for results:
@echo # 

:: -m0 C:\Experiments\SPIEMhdFiles\AP20373\T1\T1_AP20373.mhd
:: -m1 C:\Experiments\SPIEMhdFiles\AP20373\T2\T2_AP20373.mhd