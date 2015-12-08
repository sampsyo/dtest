import json
import sys
import compile


def main(alternatives_json, data_filename):
    with open(alternatives_json) as f:
        configs = json.load(f)

    results = {}
    for config in configs:
        res = compile.get_result(config['args'], infile=data_filename)
        results[config['name']] = res

    print(json.dumps(results, sort_keys=True, indent=2))


if __name__ == '__main__':
    main(*sys.argv[1:])
