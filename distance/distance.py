#!/usr/bin/env python3
import sys
import collections
import random


def read_vectors(filename):
    with open(filename) as f:
        data = f.read()

    out = []
    for num in data.split():
        num = num.strip()
        if num:
            out.append(int(num))
    return out


def chunkify(stream, size):
    cur_chunk = []
    for v in stream:
        cur_chunk.append(v)
        if len(cur_chunk) >= size:
            yield cur_chunk
        cur_chunk = []


def main(mode, filename):
    data = chunkify(read_vectors(filename), 8)

    if mode != 0:
        data = shuffle_driver(data, mode == 1)

    # Get the maximum subcube size for each dimension.
    max_subcube_sizes = []
    for index in range(8):
        column = (data[i * 8 + index] for i in range(len(data) // 8))
        counter = collections.Counter(column)
        _, count = counter.most_common(1)[0]
        max_subcube_sizes.append(count)

    print(max(max_subcube_sizes))


def shuffle_driver(data, random_flag):
    if random_flag:
        masks = generate_masks_random()
    else:
        masks = generate_masks_round_robin(data)

    return shuffle(data, masks)


def generate_masks_random():
    indices = list(range(128))
    random.shuffle(indices)
    return list(chunkify(indices, 8))


def shuffle(data, masks):
    return [shuffle_vec(vec) for vec in data]


def shuffle_vec(vec, masks):
    pieces = []
    for chunk in masks:
        bits = 0
        for index in chunk:
            short = vec[index // 8]
            bit = (short >> (15 - index % 16)) & 1
            bits = (bits << 1) | bit
        pieces.append(bits)
    return pieces


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
