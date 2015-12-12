import json
import os
from . import drive
from . import eval
from . import paramselect
from . import test

# Input files to be found in the application's base directory.
DISTRIBUTIONS_JSON = 'distributions.json'
ALTERNATIVES_JSON = 'alternatives.json'
CONFIG_JSON = 'config.json'

MODEL_SCORES = 'model_scores.json'
PARAMETER_SELECTIONS = 'param_selections.json'


def load_json(filename):
    with open(filename) as f:
        return json.load(f)


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
    drive.main(distributions_json, alternatives_json, 'results.json',
               command)
    paramselect.parameter_selections('results.json', PARAMETER_SELECTIONS)
    test.model_score(input_filename, distributions_json, MODEL_SCORES)

    # find the ideal alternative
    eval.main(alternatives_json, input_filename, 'datascores.json', command)

    closest_dist = test.dict_max(load_json(MODEL_SCORES))

    recommended_alt = load_json(PARAMETER_SELECTIONS)[closest_dist]

    datascores = load_json('datascores.json')
    best_alt = test.dict_min(datascores)
    best_score = datascores[best_alt]
    rec_score = datascores[recommended_alt]

    print("\nrecommended =          ", recommended_alt,
          "\nrec max bucket size =  ", rec_score,
          "\nbest =                 ", best_alt,
          "\nbest max bucket size = ", best_score)
