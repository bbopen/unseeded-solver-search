# Reproduction result

Search winners were selected on the training suite. Fixed examples and numerical controls were evaluated separately.

| Method | Accurate / 48 | Mean reward | Mean charged work |
|---|---:|---:|---:|
| evolution-11 | 48 | 4.8428 | 157.21 |
| random-11 | 47 | 4.8081 | 191.89 |
| evolution-22 | 48 | 4.7936 | 206.42 |
| random-22 | 47 | 4.8081 | 191.89 |
| evolution-33 | 45 | 4.8019 | 198.08 |
| random-33 | 47 | 4.8081 | 191.89 |
| fixed-example | 48 | 4.8363 | 163.75 |
| steepest-descent | 7 | 3.3452 | 459.53 |
| diagonal-descent | 12 | 3.6834 | 452.66 |
| diagonal-pcg | 48 | 4.8451 | 154.88 |
| constant-space-pcg | 48 | 4.8645 | 135.51 |

Charged work is a fixed operation proxy. It is not elapsed time.

## Search cost

| Arm | Evaluated programs | Executed solves | Process seconds |
|---|---:|---:|---:|
| evolution-11 | 7348 | 58784 | 2.538 |
| random-11 | 9385 | 75080 | 3.162 |
| evolution-22 | 4790 | 38320 | 1.661 |
| random-22 | 9358 | 74864 | 3.219 |
| evolution-33 | 5787 | 46296 | 2.051 |
| random-33 | 9454 | 75632 | 3.176 |

See report.json for expressions, training progress, per-variant results and hashes.
