#!/usr/bin/env python3
import sys
import collections


def read_vectors(filename):
    with open(filename) as f:
        data = f.read()

    out = []
    for num in data.split():
        num = num.strip()
        if num:
            out.append(int(num))
    return out


def main(mode, filename):
    data = read_vectors(filename)

    # Get the maximum subcube size for each dimension.
    max_subcube_sizes = []
    for index in range(8):
        column = data[i * 8 + index] for i in range(len(data) / 8)
        counter = collections.Counter(column)
        _, count = counter.most_common(1)
        max_subcube_sizes.append(count)

    print(max(max_subcube_sizes))


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
