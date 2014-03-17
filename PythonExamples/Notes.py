#PythonNotes
#
# makes comments with the # symbol (octothorpe, pound, hash, mesh, etc)
#
# print a message to command - anything between the "" or '' will be printed
print "Printing message."
print 'Pringing again...'

# numbers and math
print 75 % 4 # three
print 3 + 2 + 1 - 5 + 4 % 2 - 1 / 4 + 6 # seven
print 3 + 2 < 5 - 7 # false

# variables and names
# declaration
variableName = 100
string = 'single quotes'

# enter a variable into a print statement
print "Insert variable %s here %d." %( string, variableName )
# see other formatting identifiers here - http://docs.python.org/release/2.5.2/lib/typesseq-strings.html

# strings and text
x = "There are %d types of people." % 10 # stores the string in a variable x
# creates two variables
hilarious = False
joke_evaluation = "Isn't that joke so funny?! %r"
# inserts hilarious into the string in joke_evaluation as if it had been typed together
#       print "Isn't that joke so funny?! %r" %False
print joke_evaluation % hilarious
# may also print the addition of strings
print joke_evaluation + "    " + x

# more printing stuff

