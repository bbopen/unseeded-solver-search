# Record the experiment in Zergraph

This optional example records the actual outputs from `run.py`. It uses the
[Zergraph work-board pattern](https://github.com/bbopen/zergraph/blob/19f4540d87f6e1e695ba697f5d053a31a7270e3f/docs/COOKBOOK.md).
The dependency is pinned to that commit, and `Cargo.lock` pins its dependencies.

Run from the repository root after completing the main demo:

```sh
python3 examples/zergraph/export.py runs/demo --out runs/demo/graph.json
cargo run --release --locked --manifest-path examples/zergraph/Cargo.toml -- \
  runs/demo/graph.json runs/demo/board.snapshot
```

Both output files must be new. Building the Rust example needs network access
on the first run. The main search does not need Rust or Zergraph.

## What it records

Each search job links to its selected candidate and the experiment. Each
candidate has a separate evaluation node for each test condition. Observations
retain their metric values, source filename and artifact hash. Large vectors
stay in the original JSON files.

## What it checks

Two local graph forks record the small and large test systems. The example then:

1. Merges the forks in both orders and checks that their snapshots agree.
2. Repeats a delivery and checks that it changes nothing.
3. Writes a complete snapshot and restores it.
4. Checks every evaluation's metrics after restore, including inaccurate results.

The forks simulate separate writers. This example provides no transport, task
scheduler, mutual exclusion or hardware ownership. It writes one new snapshot;
it is not a production crash-recovery service. The exporter reads completed
files and therefore adds no work to a numerical rollout.
