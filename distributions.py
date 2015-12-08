import numpy


def piecewise_constant(intervals, weights, count):
    total = sum(weights)
    normalized_weights = [w / total for w in weights]

    synthetic = []
    for _ in range(count):
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

    return synthetic


def normal(mean, variance, count):
    return numpy.random.normal(mean, variance, count)


def poisson(mean, count):
    return numpy.random.poisson(mean, count)


def uniform(min, max, count):
    return numpy.random.uniform(min, max, count)


GEN_FUNCTIONS = {
    'normal': normal,
    'poisson': poisson,
    'uniform': uniform,
    'piecewise_constant': piecewise_constant,
}
