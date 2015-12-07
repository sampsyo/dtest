import json


def winner(results_fn, out_fn):
    with open(results_fn) as f:
        results = json.load(f)

    winners = {}
    for dist, data in results.items():
        best_hash = None
        best_score = None
        for hash, (min, mid, max) in data.items():
            if best_score is None or mid < best_score:
                best_score = mid
                best_hash = hash
        winners[dist] = best_hash

    with open(out_fn, 'w') as f:
        json.dump(winners, f, indent=2, sort_keys=True)


if __name__ == '__main__':
    winner('results.json', 'winner.json')
