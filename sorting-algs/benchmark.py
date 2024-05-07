import asyncio
import json
import subprocess
from typing import Literal

import matplotlib.pyplot as plt

BENCHMARK_DATA_PATH = "sample-data/projekt2_dane.csv"
BIN_PATH = "build/bin/program"


async def run_single_sort(
    record_count: int,
    sorting_alg: Literal["quicksort", "mergesort", "bucketsort"],
    result_dict: dict[str, dict[int, dict[str, int | float]]],
) -> int:
    print(f"Running benchmark for {sorting_alg} with {record_count} records")
    _result = await asyncio.to_thread(
        subprocess.check_output,
        [
            BIN_PATH,
            BENCHMARK_DATA_PATH,
            "2137",
            str(record_count),
            sorting_alg,
            str(record_count),
        ],
    )
    result = _result.decode()
    exec_time = -1
    median = -1
    average = -1
    for line in result.split("\n"):
        if line.startswith("Sort time: "):
            exec_time = int(line.split(" ")[2])
        if line.startswith("Median: "):
            median = float(line.split(" ")[1])
        if line.startswith("Average: "):
            average = float(line.split(" ")[1])

    if exec_time == -1 or median == -1 or average == -1:
        raise ValueError("Invalid output from benchmark")

    print(f"Execution time: {exec_time}")
    results = {"exec_time": exec_time, "median": median, "average": average}
    result_dict[sorting_alg][record_count] = results


async def benchmark() -> dict[str, dict[int, int]]:
    sorting_algs = ["quicksort", "mergesort", "bucketsort"]
    record_counts = [10_000, 100_000, 500_000, 1_000_000]
    results = {alg: {} for alg in sorting_algs}
    tasks = []

    def tasks_factory(alg: str) -> list[asyncio.Task]:
        return [
            asyncio.create_task(run_single_sort(count, alg, results))
            for count in record_counts
        ]

    for alg in sorting_algs:
        tasks.extend(tasks_factory(alg))

    await asyncio.gather(*tasks)
    with open("benchmark_results_.json", "w") as file:
        file.write(json.dumps(results, indent=4))


def print_md_table(results: dict[str, dict[int, dict[str, int | float]]]):
    for alg, data in results.items():
        record_counts = list(data.keys())
        exec_times = list(r["exec_time"] for r in data.values())
        medians = list(r["median"] for r in data.values())
        averages = list(r["average"] for r in data.values())

        print(f"Sorting Algorithm: {alg}")
        print("| Record Count | Average | Median | Execution Time |")
        print("|--------------|---------|--------|----------------|")
        for i in range(len(record_counts)):
            print(f"| {record_counts[i]} | {averages[i]} | {medians[i]} | {exec_times[i]} |")
        print("\n")

def plot_benchmark_results():
    with open("benchmark_results_.json", "r") as file:
        results = json.load(file)

    for alg, data in results.items():
        record_counts = list(data.keys())
        exec_times = list(r["exec_time"] for r in data.values())

        plt.plot(record_counts, exec_times, label=alg)
        plt.xlabel("Record Count")
        plt.ylabel("Execution Time (ms)")
        plt.title(f"Sorting Algorithm Benchmark - {alg}")
        plt.legend()
        plt.savefig(f"benchmark_plot_{alg}.png")
        print_md_table(results)


if __name__ == "__main__":
    asyncio.run(benchmark())
    plot_benchmark_results()
