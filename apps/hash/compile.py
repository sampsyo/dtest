import json
import sys
import subprocess


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
    command = ["c++", DRIVER_SOURCE,
               "-D", "GENERATOR_C=\"{}\"".format(gen_c),
               "-o", exe]
    subprocess.check_output(command)


def main(jsonfile):
    with open(jsonfile) as f:
        dists = json.load(f)
        for dist in dists:
            write_distribution(dist)
            compile_driver(dist)


if __name__ == '__main__':
    main(sys.argv[1])
