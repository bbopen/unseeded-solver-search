#!/usr/bin/env python3
"""Run a fixed small comparison. All selected programs and outcomes stay on disk."""
import argparse
import hashlib
import json
import platform
import statistics
import subprocess
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent
BINARY = ROOT / "build/demo"


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def call(*args):
    start = time.perf_counter()
    result = subprocess.run([str(BINARY), *map(str, args)], check=True,
                            capture_output=True, text=True, timeout=600)
    value = json.loads(result.stdout)
    value["process_seconds"] = time.perf_counter() - start
    return value


def save(path, value):
    path.write_text(json.dumps(value, indent=2, allow_nan=False) + "\n")


def aggregate(rows):
    return {
        "systems": len(rows),
        "accurate": sum(row["accurate"] for row in rows),
        "valid": sum(row["valid"] for row in rows),
        "mean_reward": statistics.mean(row["reward"] for row in rows),
        "mean_work": statistics.mean(row["work"] for row in rows),
        "mean_iterations": statistics.mean(row["iterations"] for row in rows),
        "median_residual": statistics.median(row["residual"] for row in rows),
        "median_forward_error": statistics.median(row["error"] for row in rows),
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", type=Path, default=ROOT / "runs/demo")
    parser.add_argument("--proposals", type=int, default=16384)
    parser.add_argument("--seeds", type=int, nargs="+", default=[11, 22, 33])
    args = parser.parse_args()
    if not 1 <= args.proposals <= 1000000 or any(s < 0 or s >= 2**64 for s in args.seeds):
        parser.error("Use 1..1000000 proposals and unsigned 64-bit seeds")
    if len(set(args.seeds)) != len(args.seeds):
        parser.error("Repeated seeds would duplicate trials")
    args.out.mkdir(parents=True, exist_ok=False)
    before = time.perf_counter()
    subprocess.run(["make"], cwd=ROOT, check=True, stdout=subprocess.DEVNULL)
    build_seconds = time.perf_counter() - before
    manifest = {
        "protocol": "small-search-v1", "seeds": args.seeds, "proposals_per_arm": args.proposals,
        "search_steps": 48, "evaluation_steps": 128,
        "training_systems": 8, "heldout_systems_per_method": 48,
        "selection": "mean training reward only; one winner per seed and arm",
        "search_sources": "random programs and single-field mutations; no known-method seeds",
        "budget": "same proposal count; distinct evaluations and elapsed times also reported",
        "precision": "CPU float64", "platform": platform.platform(),
        "source_sha256": digest(ROOT / "src/search.cpp"),
        "runner_sha256": digest(Path(__file__)), "binary_sha256": digest(BINARY),
        "build_seconds": build_seconds,
        "compiler": subprocess.run(["c++", "--version"], capture_output=True, text=True,
                                   check=True).stdout.splitlines()[0],
    }
    save(args.out / "manifest.json", manifest)
    searches = []
    # Freeze every winner before inspecting any held-out outcome.
    for seed in args.seeds:
        for mode in ("evolution", "random"):
            extra = ["--random"] if mode == "random" else []
            result = call("search", "--seed", seed, "--proposals", args.proposals, *extra)
            result["id"] = f"{mode}-{seed}"
            save(args.out / f"{result['id']}.search.json", result)
            program = args.out / f"{result['id']}.program"
            program.write_text("".join(" ".join(map(str, row)) + "\n" for row in result["program"]))
            searches.append(result)
    save(args.out / "frozen-selection.json", searches)
    methods = [(x["id"], ["--program", args.out / f"{x['id']}.program"]) for x in searches]
    methods += [("fixed-example", ["--program", ROOT / "examples/fixed-example.program"])]
    methods += [(name, ["--method", k]) for k, name in enumerate(
        ["steepest-descent", "diagonal-descent", "diagonal-pcg", "constant-space-pcg"])]
    summaries = {}
    for name, options in methods:
        result = call("evaluate", *options, "--steps", 128, "--vectors")
        save(args.out / f"{name}.evaluation.json", result)
        summaries[name] = aggregate(result["rows"])
        summaries[name]["process_seconds"] = result["process_seconds"]
        summaries[name]["by_variant"] = {
            str(v): aggregate([r for r in result["rows"] if r["variant"] == v]) for v in range(3)}
    report = {
        "manifest": manifest,
        "searches": searches,
        "evaluation": summaries,
        "counts": {
            "search_proposals": sum(x["proposals"] for x in searches),
            "search_evaluated_solves": sum(x["evaluated_solves"] for x in searches),
            "heldout_executed_solves": len(methods) * 48,
        },
        "scope": "Fixed demonstration suites; descriptive results, no population significance claim",
    }
    save(args.out / "report.json", report)
    lines = ["# Reproduction result", "", "Search winners were selected on the training suite. Fixed examples and numerical controls were evaluated separately.", "",
             "| Method | Accurate / 48 | Mean reward | Mean charged work |",
             "|---|---:|---:|---:|"]
    for name, value in summaries.items():
        lines.append(f"| {name} | {value['accurate']} | {value['mean_reward']:.4f} | {value['mean_work']:.2f} |")
    lines += ["", "Charged work is a fixed operation proxy. It is not elapsed time.", "",
              "## Search cost", "", "| Arm | Evaluated programs | Executed solves | Process seconds |",
              "|---|---:|---:|---:|"]
    for x in searches:
        lines.append(f"| {x['id']} | {x['evaluated_programs']} | {x['evaluated_solves']} | {x['process_seconds']:.3f} |")
    lines += ["", "See report.json for expressions, training progress, per-variant results and hashes."]
    (args.out / "README.md").write_text("\n".join(lines) + "\n")
    print("\n".join(lines))


if __name__ == "__main__":
    main()
