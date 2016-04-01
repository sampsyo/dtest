import json


def gen_alternatives(tables_options=[5, 10, 15, 20, 30, 40, 50, 60],
                     bits_options=[2, 3, 4, 5, 6, 7, 8, 9, 10],
                     outfilename='alternatives.json'):
    out = []
    for tables in tables_options:
        for bits in bits_options:
            config = (tables, bits)
            name = '{} {}'.format(tables, bits)
            out.append({'name': name, 'args': config})

    with open(outfilename, 'w') as f:
        json.dump(out, f, indent=2, sort_keys=True)


if __name__ == '__main__':
    gen_alternatives()
