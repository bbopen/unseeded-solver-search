"""Independently check every retained solution and the compact report."""
import hashlib
import json
import math
import sys
from pathlib import Path
from test_numerics import system, mv, norm


def main():
    directory = Path(sys.argv[1])
    report = json.loads((directory / "report.json").read_text())
    cache = {}
    checked = 0
    maximum_discrepancy = 0.0
    for method, summary in report["evaluation"].items():
        data = json.loads((directory / f"{method}.evaluation.json").read_text())
        accurate = 0
        for row in data["rows"]:
            key = tuple(row[k] for k in ("width", "family", "variant", "problem_seed"))
            if key not in cache:
                a, truth = system(row)
                cache[key] = a, truth, mv(a, truth)
            a, truth, b = cache[key]
            residual = norm([u - v for u, v in zip(b, mv(a, row["x"]))]) / norm(b)
            error = norm([u - v for u, v in zip(truth, row["x"])]) / norm(truth)
            discrepancy = max(abs(residual - row["residual"]), abs(error - row["error"]))
            assert discrepancy < 2e-12, (method, key, discrepancy)
            assert all(math.isfinite(x) for x in row["x"])
            flag = row["valid"] and residual <= 1e-5 and error <= 1e-3
            assert flag == row["accurate"], (method, key, "accuracy disagreement")
            accurate += flag
            checked += 1
            maximum_discrepancy = max(maximum_discrepancy, discrepancy)
        assert accurate == summary["accurate"]
        assert len(data["rows"]) == summary["systems"] == 48
        for key in ("reward", "work", "iterations"):
            assert abs(math.fsum(r[key] for r in data["rows"]) / 48 - summary[f"mean_{key}"]) < 1e-10
    result = {"solutions_checked": checked, "distinct_systems": len(cache),
              "maximum_error_metric_discrepancy": maximum_discrepancy,
              "report_sha256": hashlib.sha256((directory / "report.json").read_bytes()).hexdigest(),
              "status": "passed"}
    print(json.dumps(result, indent=2))


if __name__ == "__main__":
    main()
