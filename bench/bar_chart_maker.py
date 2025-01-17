import matplotlib.pyplot as plt
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
from matplotlib.figure import Figure
import numpy as np
import shutil
import time
import os
import sys
import json
from collections import namedtuple
import asyncio
from typing import Iterable


os.system("")
plt.rcdefaults()
filename = 'benchmarks-iterators-{}.png'


def get_cpu_caches(bench_file):
    cpu_caches = ['L0', 'L1', 'L2', 'L3', 'L4']
    benchmark_names = '\n'.join(row[0].strip() for row in bench_file if len(row) == 1 and
                                any(cache in row[0] for cache in cpu_caches))

    index_row_name = None
    for i in range(len(bench_file)):
        if bench_file[i][0] == 'name':
            index_row_name = i

    return benchmark_names, index_row_name


def get_benchmark_names_and_time(bench_dict: dict) -> Iterable[namedtuple]:
    BenchmarkRecord = namedtuple('BenchmarkRecord', 'name real_time time_unit')
    for benchmark in bench_dict['benchmarks']:
        yield BenchmarkRecord(benchmark['name'], float(benchmark['real_time']), benchmark['time_unit'])


def make_table(benchmark_records: Iterable, cxx_version: str, n_iterations: int):
    fig_border = 'steelblue'

    benchmark_records = list(benchmark_records)
    benchmark_records = sorted(benchmark_records, key=lambda e: e.real_time)
    fastest = benchmark_records[0].real_time
    data = [[record.name, record.real_time, round(record.real_time / fastest, 2)] for record in benchmark_records]
    column_headers = ["Time", "Relative speed"]
    row_headers = [x.pop(0) for x in data]

    cell_text = [[f'{round(row[0] / n_iterations, 4)} ns', f'{row[1]} x'] for row in data]
    fig = Figure(linewidth=2, edgecolor=fig_border)
    canvas = FigureCanvas(fig)
    ax = fig.add_subplot(111)
    ax.axis('off')

    fig = Figure(linewidth=2, edgecolor=fig_border)
    canvas = FigureCanvas(fig)
    ax = fig.add_subplot(111)
    ax.axis('off')

    row_colors = plt.get_cmap('BuPu')(np.full(len(row_headers), 0.1))
    col_colors = plt.get_cmap('BuPu')(np.full(len(column_headers), 0.1))

    the_table = ax.table(cellText=cell_text,
                        rowLabels=row_headers,
                        rowColours=row_colors,
                        colColours=col_colors,
                        cellLoc='center',
                        colLabels=column_headers,
                        loc='center')
    the_table.scale(1.5, 1.5)

    fig.text(.90, 0.05, None, horizontalalignment='center', size=8, weight='light')

    canvas.print_figure(f'benchmarks-iterators-{cxx_version}.png',
                        bbox_inches='tight', dpi=200)

def make_chart(cxx_version: str, path: str):
    print(f'Creating chart for "CXX{cxx_version}" from "{path}/benchmark-iterators.json"')
    with open(os.path.join(path, 'benchmark-iterators.json'), 'r') as benchmarks_file:
        bench_json = json.load(benchmarks_file)
        bench_records = get_benchmark_names_and_time(bench_json)

        iterations = 32
        make_table(bench_records, cxx_version, iterations)

    green = '\033[32m'
    print(f'{green}Successfully created {filename.format(cxx_version)}')


async def main():
    cxx_version = sys.argv[1]
    make_chart(cxx_version, os.path.join(os.getcwd(), 'build'))
    


if __name__ == '__main__':
    asyncio.run(main())
