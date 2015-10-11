import boomslang
import json


def plot(results_fn, out_fn):
    with open(results_fn) as f:
        results = json.load(f)
    draw(results, out_fn)


def draw(results, out_fn):
    bars = []
    _, first_data = sorted(results.items())[0]
    for j, (hashname, _) in enumerate(sorted(first_data.items())):
        bar = boomslang.Bar()
        bar.xValues = range(len(results))
        bar.yMins = []
        bar.yValues = []
        bar.yMaxes = []
        bar.label = hashname
        bar.color = "gray"
        bars.append(bar)

    labels = []
    for i, (dist, data) in enumerate(sorted(results.items())):
        for j, (hashname, (min, mid, max)) in enumerate(sorted(data.items())):
            # bars[j].yMins.append(min)
            bars[j].yValues.append(mid)
            # bars[j].yMaxes.append(max)
        labels.append(dist)

    clusteredBars = boomslang.ClusteredBars()
    for bar in bars:
        clusteredBars.add(bar)

    clusteredBars.xTickLabels = labels
    clusteredBars.spacing = 0.5
    # clusteredBars.drawErrorBars('y')

    plot = boomslang.Plot()
    # plot.hasLegend()
    plot.add(clusteredBars)
    plot.save(out_fn)


if __name__ == '__main__':
    plot('results.json', 'hash.pdf')
