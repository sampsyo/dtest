#!/usr/bin/python

import sys
import struct

def txt2dat(txtpath):
    """Convert a text file containing word vectors to a packed data
    format for GloVe.
    """
    assert txtpath.endswith('.txt')
    datpath = txtpath[:-4] + '.dat'

    with open(txtpath, 'r') as inf:
        with open(datpath, 'wb') as ouf:
            counter = 0
            for line in inf:
                row = [float(x) for x in line.split()[1:]]
                assert len(row) == 100
                ouf.write(struct.pack('i', len(row)))
                ouf.write(struct.pack('%sf' % len(row), *row))
                counter += 1
                if counter % 10000 == 0:
                    sys.stdout.write('%d points processed...\n' % counter)


if __name__ == '__main__':
    txt2dat('dataset/glove.6B.100d.txt')
