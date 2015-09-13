from scipy.stats import ks_2samp
import numpy
import sys

def test_normal(data):
	# numpy.random.seed(12345678)
	x = numpy.random.normal(0,1,1000)
	y = numpy.random.normal(0,1,1000)
	z = numpy.random.normal(1.1,0.9, 1000)

	print(ks_2samp(x, y))
	print(ks_2samp(x, data))

if __name__ == '__main__':
    test_normal(numpy.loadtxt(sys.argv[1]))