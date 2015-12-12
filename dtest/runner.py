import json
import os
from . import drive
from . import paramselect
from . import test

# Input files to be found in the application's base directory.
DISTRIBUTIONS_JSON = 'distributions.json'
ALTERNATIVES_JSON = 'alternatives.json'
CONFIG_JSON = 'config.json'

MODEL_SCORES = 'model_scores.json'
PARAMETER_SELECTIONS = 'param_selections.json'
MODEL_QUALITY = 'model_quality.json'
DATA_QUALITY = 'data_quality.json'


def load_json(filename):
    with open(filename) as f:
        return json.load(f)


def get_data_quality(alternatives_json, data_filename, out_filename, command):
    with open(alternatives_json) as f:
        configs = json.load(f)

    results = {}
    for config in configs:
        res = drive.get_result(config['args'], command, infile=data_filename)
        results[config['name']] = res

#    print(json.dumps(results, sort_keys=True, indent=2))
    with open(out_filename, 'w') as f:
        json.dump(results, f, sort_keys=True, indent=2)


# determine the winner's score on zipcodes
def run(appdir):
    distributions_json = os.path.join(appdir, DISTRIBUTIONS_JSON)
    alternatives_json = os.path.join(appdir, ALTERNATIVES_JSON)
    config_json = os.path.join(appdir, CONFIG_JSON)

    # Find the input file to use. (Eventually, this could be multiple inputs.)
    # Also get the command we should run.
    with open(config_json) as f:
        config = json.load(f)
    input_filename = os.path.join(appdir, config['input'])
    command = os.path.join(appdir, config['command'])

    # determine the recommended alternative
    drive.main(distributions_json, alternatives_json, MODEL_QUALITY,
               command)
    paramselect.parameter_selections(MODEL_QUALITY, PARAMETER_SELECTIONS)
    test.model_score(input_filename, distributions_json, MODEL_SCORES)

    # find the ideal alternative
    get_data_quality(alternatives_json, input_filename, DATA_QUALITY, command)

    closest_dist = test.dict_max(load_json(MODEL_SCORES))

    recommended_alt = load_json(PARAMETER_SELECTIONS)[closest_dist]

    data_quality = load_json(DATA_QUALITY)
    best_alt = test.dict_min(data_quality)
    best_score = data_quality[best_alt]
    rec_score = data_quality[recommended_alt]

    print("\nrecommended =          ", recommended_alt,
          "\nrec max bucket size =  ", rec_score,
          "\nbest =                 ", best_alt,
          "\nbest max bucket size = ", best_score)
