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

class BenchmarkResult:
    def __init__(self, real_time: str, time_unit: str):
        self.library_name = ''
        self.real_time = real_time
        self.time_unit = time_unit
    
    def __repr__(self):
        return f'BenchmarkResult(name={self.library_name}, real_time={self.real_time}, time_unit={self.time_unit})'


class Table:
    def __init__(self):
        self._rows: dict[str, list[BenchmarkResult]] = {}
        self._implementation_names: list[str] = []

    def update_row(self, name: str, result: BenchmarkResult):
        last_underscore = name.rfind('_')
        prefix, postfix = name[:last_underscore], name[last_underscore + 1:]
        result.library_name = postfix
        if not prefix in self._rows:
            self._rows[prefix] = []
        self._rows[prefix].append(result)
    
    @property
    def implementation_names(self):
        return self._implementation_names
    
    @property
    def rows(self) -> Iterable[tuple[str, list[BenchmarkResult]]]:
        return self._rows.items()
    
    def create(self):
        largest_key = max(enumerate(self._rows.values()), key=lambda x: len(x[1]))
        self._implementation_names = list(map(lambda x: x.library_name, largest_key[1]))

        for benchmark_results in self._rows.values():
            for implementation in self._implementation_names:
                if implementation in map(lambda br: br.library_name, benchmark_results):
                    continue
                benchmark_result = BenchmarkResult('-', '-')
                benchmark_result.library_name = implementation
                benchmark_results.append(benchmark_result)

    def __repr__(self):
        return f'rows={self.rows}'


def get_benchmarks(bench_dict: dict) -> Table:
    table = Table()

    for benchmark in bench_dict['benchmarks']:
        name = benchmark['name']
        table.update_row(name, BenchmarkResult(benchmark['real_time'], benchmark['time_unit']))

    table.create()
    return table


def make_table(benchmark_records: Table, cxx_version: str, n_iterations: int):
    fig_border = 'steelblue'
    
    data = []
    row_headers = []
    for it_name, impls in benchmark_records.rows:
        data.append(list(map(lambda r: f'{round(r.real_time / n_iterations, 4)} {r.time_unit} / {round(r.real_time, 4)} {r.time_unit}'
                             if isinstance(r.real_time, float) else r.real_time, impls)))
        row_headers.append(it_name)

    fig = Figure(linewidth=2, edgecolor=fig_border)
    canvas = FigureCanvas(fig)
    ax = fig.add_subplot(111)
    ax.axis('off')

    fig = Figure(linewidth=2, edgecolor=fig_border)
    canvas = FigureCanvas(fig)
    ax = fig.add_subplot(111)
    ax.axis('off')

    row_colors = plt.get_cmap('BuPu')(np.full(len(row_headers), 0.1))
    col_colors = plt.get_cmap('BuPu')(np.full(len(data), 0.1))

    the_table = ax.table(cellText=data,
                        rowLabels=row_headers,
                        rowColours=row_colors,
                        colColours=col_colors,
                        cellLoc='center',
                        colLabels=list(map(lambda n: f'{n} (1 iteration / {n_iterations} iterations)', benchmark_records.implementation_names)),
                        loc='center')
    the_table.scale(1.5, 1.5)

    fig.text(.90, 0.05, '', horizontalalignment='center', size=8, weight='light')

    canvas.print_figure(f'benchmarks-iterators-{cxx_version}.png', bbox_inches='tight', dpi=150)

def make_chart(cxx_version: str, path: str):
    print(f'Creating chart for "CXX{cxx_version}" from "{path}/benchmark-iterators.json"')

    assert os.path.exists(path), f'Path "{path}" does not exist'

    with open(os.path.join(path, 'benchmark-iterators.json'), 'r') as benchmarks_file:
        bench_json = json.load(benchmarks_file)
        bench_records = get_benchmarks(bench_json)

        iterations = 32
        make_table(bench_records, cxx_version, iterations)

    green = '\033[32m'
    print(f'{green}Successfully created {filename.format(cxx_version)}')


async def main():
    cxx_version = None
    try:
        cxx_version = sys.argv[1]
    except IndexError:
        print('No CXX version provided, using CMakeCache.txt.')
        with open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'build', 'CMakeCache.txt'), 'r') as cmake_cache:
            for line in cmake_cache:
                if not line.startswith('CMAKE_CXX_STANDARD:'):
                    continue
                cxx_version = line.split('=')[1].strip()
                break
        if cxx_version is None:
            print('No CXX version provided and CMakeCache.txt does not contain CMAKE_CXX_STANDARD.')
            sys.exit(1)
        print('Found CXX version in CMakeCache.txt:', cxx_version)

    make_chart(cxx_version, os.path.join(os.path.dirname(os.path.abspath(__file__)), 'build'))
    


if __name__ == '__main__':
    asyncio.run(main())
