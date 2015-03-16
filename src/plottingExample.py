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
	# isolate the first two elements as the iteration and metric values
	itr, metric, temp = str.split(' ',2)
	# split the parameters into separate strings
	paramStr = temp[1:-2].split(',')
	# convert each parameter into a float value
	params = []
	for i in range(len(paramStr)):
		params.append(float(paramStr[i]))
	# return the proper values from the string
	return int(itr), float(metric), params
	
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
script, monitorFilename, fixedFilename, movingFilename = argv
print argv

# Open a file and read in lines corresponding to data
#filename = raw_input('Enter in name of file: ')
#print(filename)

# open file with monitoring info
with open(monitorFilename) as monitoringTxt:
	# initialize variables
	i = 0
	itr = []
	metric = []
	params = []
	
	# initialize flags
	startFlag = False
	endFlag = False

	# read in lines from file and determine start line and end line
	for line in monitoringTxt:
		# corresponding to the first iteration
		if line[0] == '0':
			startFlag = True
		# corresponding to the print out of the Optimizer stop condition
		elif line[0] == 'O':
			endFlag = True
		elif endFlag:
			startFlag = False
		elif startFlag:
			itrTemp,metricTemp,paramsTemp = parseLineInMonitorFile( line )
			itr.append( itrTemp )
			metric.append( metricTemp )
			params.append( paramsTemp )
		else:
			endFlag = False
	
	print "\n%r has been read in." %monitorFilename

# create a line plot of the iteration number by the metric value
mon = pl.figure(1)
ax0 = mon.add_subplot(111)
ax0.plot(itr,metric)
pl.xlabel('Iteration')
pl.ylabel('Metric Value')
pl.title('Evolution of Metric Value')
pl.savefig('H:\\Results\\2015.03.16_PlottingWithPython\\metricEvolution.png')

with open(fixedFilename) as fixedTxt:
	# initialize variables
	fixedBins = []
	fixedFreq = []
	
	# read in lines and store in variables
	for line in fixedTxt:
		binTemp, freqTemp = parseLineInHistogramFile( line )
		fixedBins.append( binTemp )
		fixedFreq.append( freqTemp )
		
	print "\n%r has been read in." %fixedFilename

# plot fixed histogram
fix = pl.figure(2)
ax1 = fix.add_subplot(111)
ax1.bar(fixedBins, fixedFreq)
pl.title('Fixed Histogram with %d bins' %len(fixedBins))
pl.xlabel('Bin value')
pl.ylabel('Frequency')
pl.savefig('H:\\Results\\2015.03.16_PlottingWithPython\\fixedHistogram.png')

with open(movingFilename) as movingTxt:
	# initialize variables
	fixedBins = []
	fixedFreq = []
	
	# read in lines and store in variables
	for line in movingTxt:
		binTemp, freqTemp = parseLineInHistogramFile( line )
		fixedBins.append( binTemp )
		fixedFreq.append( freqTemp )
		
	print "\n%r has been read in." %movingFilename

mov = pl.figure(3)
ax2 = mov.add_subplot(111)
ax2.bar(fixedBins, fixedFreq)
pl.title('Moving Histogram with %d bins' %len(fixedBins))
pl.xlabel('Bin value')
pl.ylabel('Frequency')
pl.savefig('H:\\Results\\2015.03.16_PlottingWithPython\\movingHistogram.png')
