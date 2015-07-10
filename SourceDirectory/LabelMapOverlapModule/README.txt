Filename: LabelMapOverlapModule.cxx

The purpose of this function is to calculate the Dice and Jaccard similarity coefficients between labels of n label maps.

Call function from terminal:
LabelMapOverlapModule.exe output.csv labelMap1 labelMap2 [labelMap3] [labelMap4] [labelMap5]

Flow of code: ITKv3
- gather inputs
- put labelMap filenames into list
- read in labelImages and put into list
- instantiate LabelOverlapMeasureImageFilter object
- create dice and jaccard arrays (dynamic memory)
- compare each labelImage with all other labelImages
- store results into arrays
- print out results to csv file
- delete dynamic memory