import json
import sys
import subprocess
import os
import scipy.stats
import math


CODE = {
    'normal': """
std::normal_distribution<double> distribution({mean}, {variance});
Generator< std::normal_distribution<double> > gen(engine, distribution,
        false);
""",
    'poisson': """
std::poisson_distribution<uint64_t> distribution({mean});
Generator< std::poisson_distribution<uint64_t> > gen(engine,
        distribution, true);
""",
    'piecewise_constant': """
std::array<double, 3> intervals = {intervals};
std::array<double, 2> weights = {weights};
std::piecewise_constant_distribution<double> distribution
    (intervals.begin(), intervals.end(), weights.begin());
Generator< std::piecewise_constant_distribution<double> > gen(engine,
        distribution, false);
""",
    'uniform': """
std::uniform_int_distribution<uint64_t> distribution({min}, {max});
Generator< std::uniform_int_distribution<uint64_t> > gen(engine,
        distribution, true);
""",
}
DRIVER_SOURCE = "hash.cpp"
BUCKETS = 256
NTESTS = 1 << 20


def c_literal(v):
    if isinstance(v, list):
        s = str(v)
        return '{' + s[1:-1] + '}'
    else:
        return v


def emit_distribution(dist):
    code = CODE[dist['kind']]
    values = {k: c_literal(v) for k, v in dist.items()}
    return code.format(**values).strip()


def generator_c(dist):
    return '{}.c'.format(dist['name'])


def write_distribution(dist):
    c = emit_distribution(dist)
    fn = generator_c(dist)
    with open(fn, 'w') as f:
        f.write(c)


def compile_driver(dist):
    gen_c = generator_c(dist)
    exe = 'hash_{}'.format(dist['name'])
    print('compiling', exe)
    command = ["c++", DRIVER_SOURCE,
               "-D", "GENERATOR_C=\"{}\"".format(gen_c),
               "-o", exe]
    subprocess.check_output(command)
    return exe


def get_results(exe):
    print('executing', exe)
    results = []
    output = subprocess.check_output([os.path.abspath(exe)])
    for line in output.split(b'\n'):
        line = line.strip()
        if line:
            name, score = line.split(b': ')
            score = float(score.decode('utf8'))
            results.append((name.decode('utf8'), score))
    results.sort(key=lambda p: p[1])
    return results


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
            dist_out[func] = min * BUCKETS, max * BUCKETS
    return out


def main(jsonfile, outfile):
    # Compile each version.
    exes = {}
    with open(jsonfile) as f:
        dists = json.load(f)
        for dist in dists:
            write_distribution(dist)
            exes[dist['name']] = compile_driver(dist)

    # Run each executable.
    results = {}
    for name, exe in exes.items():
        results[name] = dict(get_results(exe))

    scores = counts_to_scores(results)

    with open(outfile, 'w') as f:
        json.dump(scores, f, indent=2, sort_keys=True)


if __name__ == '__main__':
    main(sys.argv[1], 'results.json')
