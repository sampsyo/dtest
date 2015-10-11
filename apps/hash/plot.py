import boomslang
import json


def plot(results_fn, out_fn):
    with open(results_fn) as f:
        results = json.load(f)
    draw(results, out_fn)


def draw(results, out_fn):
    bars = []
    _, first_data = sorted(results.items())[0]
    for j, (hashname, (min, max)) in enumerate(sorted(first_data.items())):
        bar = boomslang.Bar()
        bar.xValues = range(len(results))
        bar.yValues = []
        bar.label = hashname
        bars.append(bar)

    labels = []
    for i, (dist, data) in enumerate(sorted(results.items())):
        for j, (hashname, (min, max)) in enumerate(sorted(data.items())):
            bars[j].yValues.append(min)
        labels.append(dist)

    clusteredBars = boomslang.ClusteredBars()
    for bar in bars:
        clusteredBars.add(bar)

    clusteredBars.xTickLabels = labels
    clusteredBars.spacing = 0.5

    plot = boomslang.Plot()
    # plot.hasLegend()
    plot.add(clusteredBars)
    plot.save(out_fn)


if __name__ == '__main__':
    plot('results.json', 'hash.png')
