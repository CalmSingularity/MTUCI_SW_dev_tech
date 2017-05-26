import string
import sys

for i in range(len(sys.argv) - 1):
	filename = sys.argv[i + 1]
	try:
		myfile = open(filename, "r")
	except:
		print ("File", filename, "doesn't exist")
		continue
	
	line_counter = 0
	for line in myfile:
		line_counter += 1
	print ("File", myfile.name, "has", line_counter, "lines")
	myfile.close()