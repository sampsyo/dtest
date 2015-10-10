import json
import sys


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


def emit_distribution(dist):
    code = CODE[dist['kind']]
    return code.format(**dist)


def main(jsonfile):
    with open(jsonfile) as f:
        dists = json.load(f)
        for dist in dists:
            print(emit_distribution(dist))


if __name__ == '__main__':
    main(sys.argv[1])
