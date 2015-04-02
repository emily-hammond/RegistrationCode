#
# Emily Hammond
# 2015.03.10

from sys import argv
import numpy as np
import pylab as pl

# ****************** FUNCTION *******************
# create a function that parses through a string and
# separates it into the proper categories for the monitoring file
def parseLineInMonitorFile( str ):
	# isolate the first four elements
	itr, stepSize, gradMag, metric, temp = str.split(' ',4)
	# split the parameters into separate strings
	paramStr = temp[1:-2].split(',')
	# convert each parameter into a float value
	params = []
	for i in range(len(paramStr)):
		params.append(float(paramStr[i]))
	# return the proper values from the string
	return int(itr), float(stepSize), float(gradMag), float(metric), params
	
# ****************** FUNCTION *******************
# create a function that parses through a string and
# separates it into the proper categories for the histogram files

def parseLineInHistogramFile( str ):
	# lines look like this [bin],frequency
	# split on the comma
	bin, freq = str.split(',')
	# remove the brackets from bin
	bin = bin[1:-1]
	# return both numbers
	return int(bin), float(freq)

# ****************** MAIN CODE *******************
# read in filename from the command line as the first argument
script, fixedImage, movingImage, resultsDirectory = argv

# find the different files to take from
monitorFilename = resultsDirectory + "\\" + movingImage + "_log.txt"
fixedFilename = resultsDirectory + "\\" + fixedImage + "_" + "Histogram.txt"
movingFilename = resultsDirectory + "\\" + movingImage + "_" + "Histogram.txt"

# Open a file and read in lines corresponding to data
#filename = raw_input('Enter in name of file: ')
#print(filename)

# open file with monitoring info
with open(monitorFilename) as monitoringTxt:
	# initialize variables
	i = 0
	itr = []
	step = []
	grad = []
	metric = []
	params = []
	
	# initialize flags
	startFlag = False
	endFlag = False

	# read in lines from file and determine start line and end line
	for line in monitoringTxt:
		# corresponding to the first iteration
		if line.find('Itr#') != -1:
			startFlag = True
		# corresponding to the print out of the Optimizer stop condition
		elif (line.find('Optimizer')!= -1 or line.find('Exception')!= -1) and startFlag:
			endFlag = True
		elif endFlag:
			startFlag = False
		elif startFlag:
			itrTemp,stepSizeTemp,gradMagTemp,metricTemp,paramsTemp = parseLineInMonitorFile( line )
			itr.append( itrTemp )
			step.append( stepSizeTemp )
			grad.append( gradMagTemp )
			metric.append( metricTemp )
			params.append( paramsTemp )
		else:
			endFlag = False
	
print "%r complete" %monitorFilename

# calculate number of parameters automatically
j = len(params[0])

# transpose rows and columns in params
params = [[row[i] for row in params] for i in range(j)]

# create a line plot of the iteration number by the metric value
mon = pl.figure(1)
ax0a = mon.add_subplot(311)
ax0a.plot(itr,metric)
pl.xlabel('Iteration')
pl.ylabel('Metric Value')
pl.title('Evolution of Metric Value')
ax0b = mon.add_subplot(312)
ax0b.plot(itr,step)
pl.xlabel('Iteration')
pl.ylabel('StepSize')
pl.title('Evolution of Step Size')
ax0c = mon.add_subplot(313)
ax0c.plot(itr,grad)
pl.xlabel('Iteration')
pl.ylabel('GradMag')
pl.title('Evolution of Gradient Magnitude')
pl.savefig(resultsDirectory + '\\' + movingImage + '_metricStepPlot.png')

# create a plot for the transform parameters
if j > 6:
	y = 3
else:
	y = 2
# rotation
tran = pl.figure(2)
ax0 = tran.add_subplot(3,y,1)
ax0.plot(itr,params[0])
pl.ylabel('Rot1')
pl.title('Rotation Parameters')
ax1 = tran.add_subplot(3,y,1+y)
ax1.plot(itr,params[1])
pl.ylabel('Rot2')
ax2 = tran.add_subplot(3,y,1+y*2)
ax2.plot(itr,params[2])
pl.ylabel('Rot3')
pl.xlabel('Iteration')
ax3 = tran.add_subplot(3,y,2)
ax3.plot(itr,params[3])
pl.ylabel('TransX')
pl.title('Translation Parameters')
ax4 = tran.add_subplot(3,y,2+y)
ax4.plot(itr,params[4])
pl.ylabel('TransY')
ax5 = tran.add_subplot(3,y,2+y*2)
ax5.plot(itr,params[5])
pl.ylabel('TransZ')
pl.xlabel('Iteration')
if j > 6:
	ax6 = tran.add_subplot(3,y,3)
	ax6.plot(itr,params[6])
	pl.ylabel('ScaleX')
	pl.title('Scaling Parameters')
	ax7 = tran.add_subplot(3,y,3+y)
	ax7.plot(itr,params[7])
	pl.ylabel('ScaleY')
	ax8 = tran.add_subplot(3,y,3+y*2)
	ax8.plot(itr,params[8])
	pl.ylabel('ScaleZ')
	pl.xlabel('Iteration')
pl.savefig(resultsDirectory + '\\' + movingImage + '_transformEvolutionPlot.png')

with open(fixedFilename) as fixedTxt:
	# initialize variables
	fixedBins = []
	fixedFreq = []
	
	# read in lines and store in variables
	for line in fixedTxt:
		binTemp, freqTemp = parseLineInHistogramFile( line )
		fixedBins.append( binTemp )
		fixedFreq.append( freqTemp )
		
print "%r complete" %fixedFilename

# plot fixed histogram
fix = pl.figure(3)
ax1 = fix.add_subplot(111)
ax1.bar(fixedBins, fixedFreq)
pl.title('Fixed Histogram with %d bins' %len(fixedBins))
pl.xlabel('Bin value')
pl.ylabel('Frequency')
pl.savefig(resultsDirectory + '\\' + fixedImage + '_fixedHistogram.png')

with open(movingFilename) as movingTxt:
	# initialize variables
	fixedBins = []
	fixedFreq = []
	
	# read in lines and store in variables
	for line in movingTxt:
		binTemp, freqTemp = parseLineInHistogramFile( line )
		fixedBins.append( binTemp )
		fixedFreq.append( freqTemp )
		
print "%r complete" %movingFilename

mov = pl.figure(4)
ax2 = mov.add_subplot(111)
ax2.bar(fixedBins, fixedFreq)
pl.title('Moving Histogram with %d bins' %len(fixedBins))
pl.xlabel('Bin value')
pl.ylabel('Frequency')
pl.savefig(resultsDirectory + '\\' + movingImage + '_movingHistogram.png')
