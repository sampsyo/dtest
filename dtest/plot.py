import boomslang
import json


COLORS = ["red", "green", "blue", "orange"]


def plot(quality_fn, out_fn):
    with open(quality_fn) as f:
        results = json.load(f)
    draw(results, out_fn)


def draw(quality, out_fn):
    bars = []
    _, first_data = sorted(quality.items())[0]
    for j, (hashname, _) in enumerate(sorted(first_data.items())):
        bar = boomslang.Bar()
        bar.xValues = range(len(quality))
        bar.yMins = []
        bar.yValues = []
        bar.yMaxes = []
        bar.label = hashname
        bar.color = COLORS[j]
        bar.errorBarColor = "black"
        bars.append(bar)

    labels = []
    for i, (dist, data) in enumerate(sorted(quality.items())):
        for j, (hashname, (min, mid, max)) in enumerate(sorted(data.items())):
            bars[j].yMins.append(min)
            bars[j].yValues.append(mid)
            bars[j].yMaxes.append(max)
        labels.append(dist)

    clusteredBars = boomslang.ClusteredBars()
    for bar in bars:
        clusteredBars.add(bar)

    clusteredBars.xTickLabels = labels
    clusteredBars.spacing = 0.5
    clusteredBars.drawErrorBars('y')

    plot = boomslang.Plot()
    plot.hasLegend()
    plot.add(clusteredBars)
    plot.save(out_fn)


if __name__ == '__main__':
    plot('model_quality.json', 'hash.pdf')
