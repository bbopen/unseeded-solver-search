#!/usr/bin/env python3
"""Export a completed run as immutable, separately identified observations."""
import argparse
import hashlib
import json
from pathlib import Path


def identity(value):
    return hashlib.sha256(json.dumps(value, sort_keys=True, separators=(",", ":")).encode()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("run", type=Path)
    parser.add_argument("--out", type=Path, required=True)
    args = parser.parse_args()
    report = json.loads((args.run / "report.json").read_text())
    run_id = "run:" + identity(report)
    shared = [{"id": run_id, "properties": {"kind": "run", "manifest": report["manifest"]}, "edges": []}]
    evaluations = []
    searches = {x["id"]: x for x in report["searches"]}
    for name in report["evaluation"]:
        if name in searches:
            candidate = {"program": searches[name]["program"]}
            origin = searches[name]["mode"]
        else:
            candidate = {"fixed_method": name}
            origin = "fixed_example" if name == "fixed-example" else "known_control"
        candidate_id = "candidate:" + identity(candidate)
        shared.append({"id": candidate_id, "properties": {"kind": "candidate", **candidate}, "edges": []})
        job_id = run_id + ":job:" + name
        shared.append({"id": job_id, "properties": {"kind": "job", "name": name,
            "origin": origin, "status": "complete"}, "edges": [["belongs_to", run_id], ["selected", candidate_id]]})
        path = args.run / f"{name}.evaluation.json"
        artifact_hash = hashlib.sha256(path.read_bytes()).hexdigest()
        evaluation = json.loads(path.read_text())
        for row in evaluation["rows"]:
            condition = {k: row[k] for k in ("width", "family", "variant", "problem_seed")}
            condition["steps"] = evaluation["steps"]
            # Each job/condition has its own node. A poor result cannot overwrite a good result.
            observation_id = "evaluation:" + identity([job_id, condition, artifact_hash])
            metrics = {k: v for k, v in row.items() if k not in ("x", "rhs", "truth")}
            evaluations.append({"id": observation_id, "properties": {"kind": "evaluation",
                "condition": condition, "metrics": metrics, "artifact": path.name,
                "artifact_sha256": artifact_hash}, "edges": [["evaluates", candidate_id], ["belongs_to", job_id]]})
    payload = {"shared": shared, "evaluations": evaluations}
    with args.out.open("x") as f:
        json.dump(payload, f, indent=2, allow_nan=False)
        f.write("\n")
    print(json.dumps({"observations": len(evaluations), "output": str(args.out)}))


if __name__ == "__main__":
    main()
