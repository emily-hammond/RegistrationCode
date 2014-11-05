% The purpose of this function is to read in the histogram file generated
% from the ITK registration alignment pipeline
% (C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\AlignmentPipeline)
%
% Usage: plotHistogram(inputFilename, outputFilename)
%           inputFilename is a .txt file containing the bin # and
%           corresponding frequency in this format:
%               [0],12
%               [1],10
%               etc.
%
%           outputFilename is the desired name of the histogram figure
%           including the extension format
%               outFig.tif
%
% To run this function from the command line, run
% addpath('C:\Users\ehammond\Documents\ITKprojects\RegistrationCode\AlignmentPipeline')
% first to ensure the function is on the path
%

function plotHistogram(InputFilename, OutputFilename)
[path,name1,ext] = fileparts(InputFilename);
[path,name2,ext] = fileparts(OutputFilename);

% read in file
histFile = fopen(InputFilename,'r');

% read each line in correct format (places into a cell)
% index [bins x 1] and frequency [bins x 1]
histData = textscan(histFile, '[%f],%f');

% isolate the data
bins = histData{1};
frequency = histData{2};
normFrequency = frequency./sum(frequency);

% determine the number of bins to be thresheld
numThreshbins = 5;
if(numel(bins)*0.05 < 5)
    numThreshbins = numel(bins)*0.05
elseif(numel(bins)*0.05 > 100)
    numThreshbins = 20;
end

% find the x max values with their indices
for i = 1:numThreshbins
    maxValue = max(normFrequency);
    idx(i) = find(normFrequency == maxValue);
    normFrequency(idx(i)) = 0;
end

% threshold the max bins found
normFrequency(idx) = maxValue;

% plot the data and make a pretty figure
fig = bar(bins,normFrequency,'hist');
axis('tight');
hX = xlabel('Bins');
hY = ylabel('Frequency');
message = sprintf('Normalized Histogram Plot of %s with %d bins',name1,numel(bins));
hTitle = title(message);
set(hTitle,'FontSize', 12, ...
    'FontWeight', 'bold');
set([hX, hY],'FontSize', 10);
set(gca,'FontSize',8,...
    'Box','off',...
    'TickDir','out',...
    'TickLength',[0.02, 0.02],...
    'XMinorTick','on',...
    'YMinorTick','on',...
    'YGrid','on')
set(gcf, 'PaperPositionMode','auto');

% save the figure to the proper file
OutputFilename = sprintf('%s\\%s%d%s',path,name2,numel(bins),ext);
saveas(fig, OutputFilename);

% close the fileS
fclose(histFile);

% close out of matlab
% exit