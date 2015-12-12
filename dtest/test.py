from scipy.stats import ks_2samp
import numpy
import sys
import json
from . import distributions

NUM_TESTS = 10


def test_normal(data, mean, variance):
    if len(data) > 10 ** 10:
        print('that is a lot of data!')
    synthetic = distributions.normal(mean, variance, len(data))
    _, p = ks_2samp(synthetic, data)
    return p


def test_poisson(data, mean):
    synthetic = distributions.poisson(mean, len(data))
    _, p = ks_2samp(synthetic, data)
    return p


def test_uniform(data, min, max):
    synthetic = distributions.uniform(min, max, len(data))
    _, p = ks_2samp(synthetic, data)
    return p


def test_piecewise_constant(data, intervals, weights):
    synthetic = distributions.piecewise_constant(intervals, weights, len(data))
    _, p = ks_2samp(synthetic, data)
    return p


TEST_FUNCTIONS = {
    'normal': test_normal,
    'poisson': test_poisson,
    'uniform': test_uniform,
    'piecewise_constant': test_piecewise_constant,
}


def get_scores(data, dists):
    scores = {}

    for dist in dists:
        try:
            func = TEST_FUNCTIONS[dist['kind']]
        except KeyError:
            print('no test found for', dist['kind'])
            continue

        args = {k: v for k, v in dist.items() if k not in ('kind', 'name')}
        score = func(data, **args)

        scores[dist['name']] = score

    return scores


def dict_average(dicts):
    totals = {k: 0.0 for k in dicts[0]}
    for d in dicts:
        for k, v in d.items():
            totals[k] += v
    return {k: v / len(totals) for k, v in totals.items()}


def dict_max(d):
    max_key = None
    max_value = None
    for k, v in d.items():
        if max_key is None or v > max_value:
            max_key = k
            max_value = v
    return max_key


def dict_min(d):
    min_key = None
    min_value = None
    for k, v in d.items():
        if min_key is None or v < min_value:
            min_key = k
            min_value = v
    return min_key


def model_score(data_filename, dists_filename, out_filename):
    data = numpy.loadtxt(data_filename)

    with open(dists_filename) as dists_file:
        dists = json.load(dists_file)

    scores_samples = []
    for _ in range(NUM_TESTS):
        scores_samples.append(get_scores(data, dists))

    scores_avg = dict_average(scores_samples)
#    print(dict_max(scores_avg))
    with open(out_filename, 'w') as f:
        json.dump(scores_avg, f)
