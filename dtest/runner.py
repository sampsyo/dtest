import json
from . import drive
from . import eval
from . import winner
from . import test

# determine the recommended alternative
drive.main('distributions.json', 'alternatives.json', 'results.json')
winner.winner('results.json', 'winner.json')
test.main('data/zipcodes.txt', 'distributions.json', 'distsims.json')

# find the ideal alternative
eval.main('alternatives.json', 'data/zipcodes.txt', 'datascores.json')


# determine the winner's score on zipcodes
def main(out_filename):
    with open('distsims.json') as f:
        closest_dist = test.dict_max(json.load(f))

    with open('winner.json') as f:
        recommended_alt = json.load(f)[closest_dist]

    with open('datascores.json') as f:
        datascores = json.load(f)
        best_alt = test.dict_min(datascores)
        best_score = datascores[best_alt]
        rec_score = datascores[recommended_alt]

    print("\nrecommended =          ", recommended_alt,
          "\nrec max bucket size =  ", rec_score,
          "\nbest =                 ", best_alt,
          "\nbest max bucket size = ", best_score)

if __name__ == '__main__':
    main('wedontcare')
