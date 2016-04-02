import json
import sys
import subprocess
import os
import scipy.stats
import math
import re
from . import distributions

BUCKETS = 1 << 10
NTESTS = 1 << 20
OUTPUT_RE = r'dtest score:\s*(\S+)'


def get_result(args, exe='hash', infile='temp.txt'):
    cmd = [os.path.abspath(exe)] + list(map(str, args)) + [infile]

    print('executing', cmd)
    output = subprocess.check_output(cmd).decode('utf8')

    # Explicitly marked output.
    match = re.search(OUTPUT_RE, output)
    if match:
        return match.group(1)

    # If it's not explicitly marked, use the first token in the output.
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


def generate_sample(dist, count, outfile='temp.txt'):
    if 'kind' in dist:
        # Use a built-in distribution generator. We look up the function
        # in the `distributions` module.
        func = distributions.GEN_FUNCTIONS[dist['kind']]
        args = {k: v for k, v in dist.items() if k not in ('name', 'kind')}
        args['count'] = count
        data = func(**args)
        with open(outfile, 'w') as f:
            for sample in data:
                if isinstance(sample, list):
                    # For bit vectors.
                    f.write(' '.join(str(i).zfill(5) for i in sample)
                            + ' ')
                else:
                    # FIXME integers!
                    f.write('{}\n'.format(int(sample)))

    else:
        # An extensible, external distribution generator. This works by
        # invoking a program with the specified arguments to generate a
        # sample. The distribution dictionary contains `program`, which
        # is an executable name and any initial arguments, and `args`.
        # We'll invoke the program with the number of samples followed
        # by the specified args.
        args = dist['program'] + [count] + dist['args']
        args = [str(a) for a in args]
        with open(outfile, 'w') as f:
            proc = subprocess.Popen(args, stdout=f)
            retcode = proc.wait()
            assert retcode == 0


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
            generate_sample(dist, NTESTS)

            # Invoke the executable.
            results[dist['name']][config['name']] = \
                get_result(config['args'], command)

    scores = counts_to_scores(results)

    with open(outfile, 'w') as f:
        json.dump(scores, f, indent=2, sort_keys=True)


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2], 'results.json')
