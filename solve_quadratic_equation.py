import math
import cmath
from sys import argv

def solveQuadraticEquation (a, b, c):
	print ("Quadratic equation: {0}*x^2 + {1}*x + {2} = 0".format(a, b, c))

	if (a == b == 0):
		if (c != 0):
			print ("{0} = 0 is not a valid equation".format(c))
		else:
			print ("0 = 0 is not an interesting equation")
		return

	if (a == 0):
		print ("The single solution is x =", -c / b)
		return

	D = b * b - 4 * a * c

	if (D > 0):
		x1 = (-b + math.sqrt(D)) / (2 * a)
		x2 = (-b - math.sqrt(D)) / (2 * a)
		print ("The equation has two real roots:")
		print ("x1 =", x1)
		print ("x2 =", x2)
	elif (D == 0):
		x = -b / (2 * a)
		print ("The single solution is x =", x)
	elif (D < 0):
		x1 = (-b + cmath.sqrt(D)) / (2 * a)
		x2 = (-b - cmath.sqrt(D)) / (2 * a)
		print ("The equation has two complex roots:")
		print ("x1 =", x1)
		print ("x2 =", x2)


#------------------------------------------------------------------
argumentsAreCorrect = True

if (len(argv) != 4):
	print ("A quadratic equation has exactly 3 arguments!")
	argumentsAreCorrect = False
else:
	try:
		a = float(argv[1])
		b = float(argv[2])
		c = float(argv[3])
	except:
		print ("Incorrect arguments. Arguments must be numbers!")
		argumentsAreCorrect = False

if (argumentsAreCorrect):
	solveQuadraticEquation(a, b, c)