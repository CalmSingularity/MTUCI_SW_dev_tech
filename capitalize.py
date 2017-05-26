import string
import sys

if (len(sys.argv) != 3):
	print ("The program takes exactly two arguments: a source file and a destination file")
	sys.exit("Wrong arguments")

source_file = sys.argv[1]
dest_file = sys.argv[2]

try:
	myfile = open(source_file, 'r')
except:
	print ("File", source_file, "doesn't exist")
	sys.exit("Can't find the file")

try:
	myfile_caps = open(dest_file, 'w')
except:
	sys.exit("Can't write to file")

for line in myfile:
	myfile_caps.write(line.upper())

myfile.close()
myfile_caps.close()