% Emily Hammond
% 2015.02.16
%
% This code is written to investigate the histograms of images for the
% purpose of registration parameter tuning

% read in image - must be in .mhd format
filename = 'C:\Experiments\Results\SampleMhdImages_IngridT3\AP20376_20130207_CT\Head_30_H31s.mhd';
info = mha_read_header( filename );
img = mha_read_volume( info );

% create histogram and display
numOfBins = 50;
minValue = min(img(:));
maxValue = max(img(:));
binSize = (maxValue - minValue)/numOfBins;

for i = binSize:binSize:maxValue
    ind = find( (i-binSize) 