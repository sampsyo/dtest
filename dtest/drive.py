import json
import sys
import subprocess
import os
import scipy.stats
import math
from . import distributions

BUCKETS = 256
NTESTS = 1 << 10


def get_result(args, exe='hash', infile='temp.txt'):
    cmd = [os.path.abspath(exe)] + list(map(str, args)) + [infile]

    print('executing', cmd)
    output = subprocess.check_output(cmd)

    parts = output.split()
    return int(parts[0])


def clopper_pearson(x, n, alpha=0.05):
    """Estimate the confidence interval for a sampled Bernoulli random
    variable.
    """
    b = scipy.stats.beta.ppf
    lo = b(alpha / 2, x, n - x + 1)
    hi = b(1 - alpha / 2, x + 1, n - x)
    return 0.0 if math.isnan(lo) else lo, 1.0 if math.isnan(hi) else hi


def counts_to_scores(results):
    out = {}
    for dist, res in results.items():
        out[dist] = dist_out = {}
        for func, collisions in res.items():
            min, max = clopper_pearson(collisions, NTESTS)
            mid = collisions / NTESTS
            dist_out[func] = min, mid, max
    return out


def generate_sample(dist, count):
    func = distributions.GEN_FUNCTIONS[dist['kind']]
    args = {k: v for k, v in dist.items() if k not in ('name', 'kind')}
    args['count'] = count
    return func(**args)


def main(distributions_json, alternatives_json, outfile, command):
    # Get the distributions to generate.
    with open(distributions_json) as f:
        dists = json.load(f)

    # Get the configurations to test.
    configs = {}
    with open(alternatives_json) as f:
        configs = json.load(f)

    # Run each configuration on each distribution.
    results = {}
    for dist in dists:
        results[dist['name']] = {}

        for config in configs:
            # Write the data file.
            data = generate_sample(dist, NTESTS)
            with open('temp.txt', 'w') as f:
                for sample in data:
                    if isinstance(sample, list):
                        # For bit vectors.
                        f.write(' '.join(str(i).zfill(5) for i in sample)
                                + ' ')
                    else:
                        # FIXME integers!
                        f.write('{}\n'.format(int(sample)))

            # Invoke the executable.
            results[dist['name']][config['name']] = \
                get_result(config['args'], command)

    scores = counts_to_scores(results)

    with open(outfile, 'w') as f:
        json.dump(scores, f, indent=2, sort_keys=True)


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2], 'results.json')
