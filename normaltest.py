from __future__ import print_function
from scipy import stats


def normal_test():
    a = stats.uniform.rvs(size=500)
    k2, p = stats.normaltest(a)
    print(k2, p)


if __name__ == '__main__':
    normal_test()
