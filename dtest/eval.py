import json
from . import drive


def main(alternatives_json, data_filename, out_filename, command):
    with open(alternatives_json) as f:
        configs = json.load(f)

    results = {}
    for config in configs:
        res = drive.get_result(config['args'], command, infile=data_filename)
        results[config['name']] = res

#    print(json.dumps(results, sort_keys=True, indent=2))
    with open(out_filename, 'w') as f:
        json.dump(results, f, sort_keys=True, indent=2)
