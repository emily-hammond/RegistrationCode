Filename: LabelMapOverlapModule.cxx

The purpose of this function is to calculate the Dice and Jaccard similarity coefficients between labels of n label maps.

Call function from terminal:
LabelMapOverlapModule.exe output.csv labelMap1 labelMap2 [labelMap3] [labelMap4] [labelMap5]

Flow of code: ITKv3
- define LabelOverlapMeasures function
-- define image type
-- read in images
-- instantiate filter
-- perform calculations within filter
-- write out values to csv file (both overall and individual measures)
- identify inputs
- open file
- perform comparisons
- send images and file to function
- close file
- exit program
