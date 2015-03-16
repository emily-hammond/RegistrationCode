#
# Emily Hammond
# 2015.03.10

from sys import argv
import numpy as np
import pylab as pl

# create a function that parses through the string and
# separates it into the proper categories
def parseLine( str ):
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

# read in filename from the command line as the first argument
script, filename = argv
print "Reading file: %r" %filename

# Open a file and read in lines corresponding to data
#filename = raw_input('Enter in name of file: ')
#print(filename)

# open file
with open(filename) as txt:
	
	# initialize variables
	i = 0
	itr = []
	metric = []
	params = []
	
	# initialize flags
	startFlag = False
	endFlag = False

	# read in lines from file and determine start line and end line
	for line in txt:
		# corresponding to the first iteration
		if line[0] == '0':
			startFlag = True
		# corresponding to the print out of the Optimizer stop condition
		elif line[0] == 'O':
			endFlag = True
		elif endFlag:
			startFlag = False
		elif startFlag:
			itrTemp,metricTemp,paramsTemp =  parseLine( line )
			itr.append( itrTemp )
			metric.append( metricTemp )
			params.append( paramsTemp )
		else:
			endFlag = False

# create a line plot of the iteration number by the metric value
pl.plot(itr,metric)
pl.xlabel('Iteration')
pl.ylabel('Metric Value')
pl.title('Evolution of Metric Value')
pl.show()