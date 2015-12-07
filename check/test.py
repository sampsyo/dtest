from scipy.stats import ks_2samp
import numpy
import sys
import json


def test_normal(data, mean, variance):
    if len(data) > 10 ** 10:
        print('that is a lot of data!')
    synthetic = numpy.random.normal(mean, variance, len(data))
    _, p = ks_2samp(synthetic, data)
    return p


def test_poisson(data, mean):
    synthetic = numpy.random.poisson(mean, len(data))
    _, p = ks_2samp(synthetic, data)
    return p


def test_uniform(data, min, max):
    synthetic = numpy.random.uniform(min, max, len(data))
    _, p = ks_2samp(synthetic, data)
    return p


def test_piecewise_constant(data, intervals, weights):
    total = sum(weights)
    normalized_weights = [w / total for w in weights]

    synthetic = []
    for _ in range(len(data)):
        # Choose a piece.
        position = numpy.random.ranf()
        for index, weight in enumerate(normalized_weights):
            position -= weight
            if position <= 0.0:
                break
        else:
            assert False

        # Choose the interval from the index.
        lo, hi = intervals[index], intervals[index + 1]

        # Sample from the range.
        synthetic.append(numpy.random.uniform(lo, hi))

    _, p = ks_2samp(synthetic, data)
    return p


TEST_FUNCTIONS = {
    'normal': test_normal,
    'poisson': test_poisson,
    'uniform': test_uniform,
    'piecewise_constant': test_piecewise_constant,
}


def main(data_filename, dists_filename):
    data = numpy.loadtxt(data_filename)

    with open(dists_filename) as dists_file:
        dists = json.load(dists_file)

    for dist in dists:
        try:
            func = TEST_FUNCTIONS[dist['kind']]
        except KeyError:
            print('no test found for', dist['kind'])
            continue

        args = {k: v for k, v in dist.items() if k not in ('kind', 'name')}
        score = func(data, **args)

        print(dist['name'], score)


if __name__ == '__main__':
    main(*sys.argv[1:])
