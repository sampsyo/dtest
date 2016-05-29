#!/usr/bin/python

import sys
import struct

def txt2dat(txtpath):
    """Convert a text or CSV file containing word vectors to a packed
    data format for GloVe.
    """
    assert txtpath.endswith('.txt') or txtpath.endswith('.csv')
    is_csv = txtpath.endswith('.csv')
    datpath = txtpath[:-4] + '.dat'

    with open(txtpath, 'r') as inf:
        with open(datpath, 'wb') as ouf:
            counter = 0
            for line in inf:
                line = line.strip()
                if not line:
                    continue

                # Use either commas or whitespace to split.
                if is_csv:
                    parts = line.split(',')
                else:
                    parts = line.split()

                row = [float(x) for x in parts[1:]]
                ouf.write(struct.pack('i', len(row)))
                ouf.write(struct.pack('%sf' % len(row), *row))
                counter += 1
                if counter % 10000 == 0:
                    sys.stdout.write('%d points processed...\n' % counter)


if __name__ == '__main__':
    txt2dat(sys.argv[1])
