from __future__ import division
from scipy.stats import ks_2samp
import numpy
import json
import collections
import math
import subprocess
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


def _entropy(counts):
    total = sum(counts.values())
    ent = 0.0
    for key, count in counts.items():
        p = count / total
        ent += p * math.log(1 / p)
    return ent


def chunkify(stream, size):
    cur_chunk = []
    for v in stream:
        cur_chunk.append(v)
        if len(cur_chunk) >= size:
            yield cur_chunk
            cur_chunk = []


def test_bitvec(data, bits, entropy):
    shorts = bits // 16

    min_entropy = bits
    for i in range(shorts):
        counter = collections.Counter(v[i] for v in chunkify(data, shorts))
        min_entropy = min(min_entropy, _entropy(counter))

    return abs(min_entropy - entropy) / bits


TEST_FUNCTIONS = {
    'normal': test_normal,
    'poisson': test_poisson,
    'uniform': test_uniform,
    'piecewise_constant': test_piecewise_constant,
    'bitvec': test_bitvec,
}


def get_scores(data_filename, dists):
    """Get the similarity scores for the given data against the given
    distribution specifications. Return a map from distribution names to
    scores.
    """
    scores = {}

    for dist in dists:
        if 'test_program' in dist:
            # An extensible distribution that has a program that can compute
            # scores.

            # Run the test program and direct it to read the data file.
            command = dist['test_program'] + dist['args']
            command = [str(a).encode('utf8') for a in command]
            print('testing data file {} with command {}'.format(data_filename,
                                                                command))
            with open(data_filename) as f:
                proc = subprocess.Popen(
                    command,
                    stdin=f,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                )
                stdout, stderr = proc.communicate()
                if stderr:
                    print(stderr.decode('utf8'))
                retcode = proc.wait()
                assert retcode == 0

            # The program's output should be the score.
            score = float(stdout.strip())

        else:
            # A built-in distribution test. The data file must be formatted as
            # a text file with a list of numbers.
            try:
                func = TEST_FUNCTIONS[dist['kind']]
            except KeyError:
                print('no test found for', dist['kind'])
                continue

            args = {k: v for k, v in dist.items() if k not in ('kind', 'name')}
            data = numpy.loadtxt(data_filename)
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


def model_score(data_filename, dists_filename, out_filename):
    """Score some data against a set of possible distributions and write
    the similarity scores.
    """
    with open(dists_filename) as dists_file:
        dists = json.load(dists_file)

    scores_samples = []
    for _ in range(NUM_TESTS):
        scores_samples.append(get_scores(data_filename, dists))

    scores_avg = dict_average(scores_samples)
    with open(out_filename, 'w') as f:
        json.dump(scores_avg, f)
