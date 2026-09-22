"""Check returned vectors against a separate dense Python implementation."""
import json
import math
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXE = ROOT / "build/demo"
MASK = 2**64 - 1


def mix(x):
    x &= MASK
    x = ((x ^ (x >> 30)) * 0xbf58476d1ce4e5b9) & MASK
    x = ((x ^ (x >> 27)) * 0x94d049bb133111eb) & MASK
    return x ^ (x >> 31)


def system(row):
    w, f, v, seed = [row[k] for k in ("width", "family", "variant", "problem_seed")]
    n = w * w
    matrix = [[0.0] * n for _ in range(n)]
    for i in range(n):
        mass = .03 if f else .15
        if v == 1:
            mass *= .5 + (mix(seed + i * 101) % 1024) / 1023
        if v == 2:
            mass += sum([i // w == 0, i // w == w - 1, i % w == 0, i % w == w - 1])
        matrix[i][i] += mass
        for axis in range(2):
            if v == 2 and (i % w == w - 1 if axis == 0 else i // w == w - 1):
                continue
            j = (i // w) * w + (i + 1) % w if axis == 0 else (i + w) % n
            u = (mix(seed + i * 31 + axis * 100003) % 1024) / 1023
            weight = 2**(8 * u - 4) if f else .5 + u
            matrix[i][i] += weight
            matrix[j][j] += weight
            matrix[i][j] -= weight
            matrix[j][i] -= weight
    truth = [(mix(seed ^ (i * 8191 + 37)) % 2001 - 1000) * .001 for i in range(n)]
    return matrix, truth


def dot(a, b):
    return math.fsum(x * y for x, y in zip(a, b))


def mv(a, x):
    return [dot(row, x) for row in a]


def norm(x):
    return math.sqrt(dot(x, x))


def reference(a, truth, method, steps):
    """Explicit recurrences, without the arithmetic-program interpreter."""
    b = mv(a, truth)
    n = len(b)
    x, bestx, previous_step, direction = ([0.0] * n for _ in range(4))
    best, previous_rz = dot(b, b), 0.0
    for t in range(steps):
        r = [u - v for u, v in zip(b, mv(a, x))]
        z = [r[i] / a[i][i] for i in range(n)]
        if method == -1:
            mean = math.fsum(r) / n
            direction = [z[i] + ((z[i] + previous_step[i]) + mean) for i in range(n)]
        elif method >= 2:
            if method == 3:
                correction = math.fsum(r) / math.fsum(map(math.fsum, a))
                z = [v + correction for v in z]
            rz = dot(r, z)
            beta = rz / previous_rz if t else 0
            direction = [z[i] + beta * direction[i] for i in range(n)]
            previous_rz = rz
        else:
            direction = r if method == 0 else z
        denominator = dot(direction, mv(a, direction))
        alpha = dot(r, direction) / denominator if denominator > 1e-30 else 0
        previous_step = [alpha * d for d in direction]
        x = [u + v for u, v in zip(x, previous_step)]
        rr = norm([u - v for u, v in zip(b, mv(a, x))])**2
        if rr < best:
            best, bestx = rr, x[:]
        if best <= 1e-10 * dot(b, b):
            break
    return bestx


def call(*args):
    return json.loads(subprocess.run([str(EXE), *map(str, args)], check=True,
                                    capture_output=True, text=True).stdout)


class NumericalTests(unittest.TestCase):
    def test_dense_recurrences_and_reported_errors(self):
        for method in [-1, 0, 1, 2, 3]:
            options = ["--program", ROOT / "examples/fixed-example.program"] if method == -1 else ["--method", method]
            result = call("evaluate", *options, "--steps", 8, "--vectors")
            # One preselected system in each of the six small-matrix strata.
            for row in result["rows"][:24:4]:
                a, truth = system(row)
                b = mv(a, truth)
                expected = reference(a, truth, method, 8)
                with self.subTest(method=method, family=row["family"], variant=row["variant"]):
                    self.assertLess(norm([x - y for x, y in zip(expected, row["x"])]), 2e-9)
                    residual = norm([x - y for x, y in zip(b, mv(a, row["x"]))]) / norm(b)
                    error = norm([x - y for x, y in zip(truth, row["x"])]) / norm(truth)
                    self.assertAlmostEqual(residual, row["residual"], delta=2e-12)
                    self.assertAlmostEqual(error, row["error"], delta=2e-12)
                    self.assertLess(norm([x - y for x, y in zip(b, row["rhs"])]), 2e-12)
                    self.assertTrue(all(a[i][j] == a[j][i] for i in range(64) for j in range(64)))
                    # Positive diagonal dominance proves SPD for this symmetric matrix.
                    self.assertTrue(all(a[i][i] > sum(abs(a[i][j]) for j in range(64) if j != i)
                                        for i in range(64)))

    def test_repeatable_search_and_disjoint_evaluation(self):
        first = call("search", "--seed", 11, "--proposals", 128)
        second = call("search", "--seed", 11, "--proposals", 128)
        first.pop("seconds"); second.pop("seconds")
        self.assertEqual(first, second)
        self.assertEqual(first["evaluated_solves"], 8 * first["evaluated_programs"])
        train = call("evaluate", "--method", 2, "--training")["rows"]
        test = call("evaluate", "--method", 2)["rows"]
        self.assertFalse({r["problem_seed"] for r in train} & {r["problem_seed"] for r in test})

    def test_reject_forward_reference(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "bad.program"
            path.write_text("0 11 0\n" * 6)
            result = subprocess.run([str(EXE), "evaluate", "--program", str(path)], capture_output=True)
            self.assertNotEqual(result.returncode, 0)
            self.assertIn(b"forward reference", result.stderr)


if __name__ == "__main__":
    unittest.main()
