from scipy.stats import ks_2samp
import numpy
import sys

def test_normal(data):
	norm = numpy.random.normal(0,1,1000)
	unif = numpy.random.uniform(0,10,1000)

	print(ks_2samp(norm, data))
	print(ks_2samp(unif, data))

if __name__ == '__main__':
    test_normal(numpy.loadtxt(sys.argv[1]))