from scipy.stats import ks_2samp
import numpy
import sys
import json


def test_normal(data, mean, variance):
    if len(data) > 10 ** 10:
        print('that is a lot of data!')
    norm = numpy.random.normal(mean, variance, len(data))
    _, p = ks_2samp(norm, data)
    return p


TEST_FUNCTIONS = {
    'normal': test_normal,
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
