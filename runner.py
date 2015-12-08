import subprocess
import collections


def get_results():
    results = []
    output = subprocess.check_output(['./hash'])
    for line in output.split('\n'):
        line = line.strip()
        if line:
            name, score = line.split(': ')
            score = float(score)
            results.append((name, score))
    results.sort(key=lambda p: p[1])
    return results


def print_results(results):
    for name, score in results:
        print('{}: {}'.format(name, score))


def get_ranks(results):
    ranks = {}
    for i, (name, _) in enumerate(results):
        ranks[name] = i
    return ranks


def mean(s):
    t = 0.0
    c = 0
    for v in s:
        t += v
        c += 1
    return t / c


def main():
    rank_lists = collections.defaultdict(list)

    for i in range(10):
        results = get_results()
        print_results(results)
        for name, rank in get_ranks(results).items():
            rank_lists[name].append(rank)
        print('---')

    rank_list_pairs = rank_lists.items()
    rank_list_pairs.sort(key=lambda p: mean(p[1]))
    for name, ranks in rank_list_pairs:
        print('{}: {} {}'.format(name, mean(ranks), ranks))


if __name__ == '__main__':
    main()
