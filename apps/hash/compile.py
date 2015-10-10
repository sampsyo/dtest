import json
import sys
import subprocess
import os


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

    with open(outfile, 'w') as f:
        json.dump(results, f, indent=2, sort_keys=True)


if __name__ == '__main__':
    main(sys.argv[1], 'results.json')
