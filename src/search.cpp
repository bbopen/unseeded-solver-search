// Search short arithmetic programs for directions in an SPD linear solve.
// C++17, one CPU thread, no libraries, no generated code compilation.
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using Vec = std::vector<double>;
constexpr int length = 6, base = 6, registers = base + length;
constexpr double tolerance = 1e-5, error_tolerance = 1e-3;
enum Op { Copy, Add, Sub, Mul, Div, Dot, Neg, Half, Min, Max, Mean, Matvec };
const std::array<std::string, 12> names = {
    "copy", "add", "sub", "mul", "div", "dot", "neg", "half",
    "min", "max", "mean", "matvec"};
struct Instruction { int op, a, b; };
using Program = std::array<Instruction, length>;
bool unary(int op) {
    return op == Copy || op == Neg || op == Half || op == Mean || op == Matvec;
}
void validate(const Program& p) {
    for (int k = 0; k < length; ++k) {
        const auto q = p[k];
        if (q.op < 0 || q.op >= 12 || q.a < 0 || q.a >= base + k ||
            q.b < 0 || q.b >= base + k)
            throw std::runtime_error("Invalid instruction or forward reference");
    }
}
std::array<bool, length> active(const Program& p) {
    std::array<bool, length> live{};
    live.back() = true;
    for (int k = length - 1; k >= 0; --k) if (live[k]) {
        if (p[k].a >= base) live[p[k].a - base] = true;
        if (!unary(p[k].op) && p[k].b >= base) live[p[k].b - base] = true;
    }
    return live;
}
std::string expression(const Program& p) {
    std::array<std::string, registers> r;
    std::copy_n(std::array<std::string, base>{"r", "D_inv_r", "previous_step",
        "previous_r", "D_inv_previous_r", "one"}.begin(), base, r.begin());
    for (int k = 0; k < length; ++k) {
        auto q = p[k];
        r[base + k] = q.op == Copy ? r[q.a] : names[q.op] + "(" + r[q.a] +
            (unary(q.op) ? "" : "," + r[q.b]) + ")";
    }
    return r.back();
}
void print_program(const Program& p) {
    std::cout << '[';
    for (int k = 0; k < length; ++k) {
        if (k) std::cout << ',';
        std::cout << '[' << p[k].op << ',' << p[k].a << ',' << p[k].b << ']';
    }
    std::cout << ']';
}
Program read_program(const std::string& path) {
    Program p{};
    std::ifstream f(path);
    for (auto& q : p) if (!(f >> q.op >> q.a >> q.b))
        throw std::runtime_error("Expected six integer triples in program file");
    std::string extra;
    if (f >> extra) throw std::runtime_error("Extra content in program file");
    validate(p);
    return p;
}

// This integer generator avoids implementation-defined standard distributions.
uint64_t mix(uint64_t x) {
    x ^= x >> 30; x *= 0xbf58476d1ce4e5b9ULL;
    x ^= x >> 27; x *= 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}
struct Random {
    uint64_t state;
    uint64_t next() { state += 0x9e3779b97f4a7c15ULL; return mix(state); }
    int index(int n) {
        const uint64_t bound = uint64_t(n), threshold = -bound % bound;
        uint64_t x;
        do { x = next(); } while (x < threshold);
        return int(x % bound);
    }
};
Program random_program(Random& rng) {
    Program p;
    for (int k = 0; k < length; ++k)
        p[k] = {rng.index(12), rng.index(base + k), rng.index(base + k)};
    return p;
}
Program mutate(Program p, Random& rng) {
    int k = rng.index(length);
    switch (rng.index(3)) {
        case 0: p[k].op = rng.index(12); break;
        case 1: p[k].a = rng.index(base + k); break;
        default: p[k].b = rng.index(base + k);
    }
    return p;
}

struct Edge { int i, j; double weight; };
struct Problem {
    int width, family, variant;
    uint64_t seed;
    Vec shift, diagonal, truth, rhs;
    std::vector<Edge> edges;
    // A is a positive diagonal plus a weighted graph Laplacian.
    void apply(const Vec& x, Vec& out) const {
        for (size_t i = 0; i < x.size(); ++i) out[i] = shift[i] * x[i];
        for (auto e : edges) {
            double v = e.weight * (x[e.i] - x[e.j]);
            out[e.i] += v; out[e.j] -= v;
        }
    }
};
Problem problem(int width, int family, int variant, uint64_t seed) {
    int n = width * width;
    Problem p{width, family, variant, seed, Vec(n), Vec(n), Vec(n), Vec(n), {}};
    for (int i = 0; i < n; ++i) {
        double s = family ? .03 : .15;
        if (variant == 1) s *= .5 + double(mix(seed + i * 101ULL) % 1024) / 1023;
        // Homogeneous Dirichlet boundaries add a positive boundary diagonal.
        if (variant == 2) {
            int row = i / width, col = i % width;
            s += (row == 0) + (row == width - 1) + (col == 0) + (col == width - 1);
        }
        p.shift[i] = p.diagonal[i] = s;
        p.truth[i] = (int(mix(seed ^ uint64_t(i * 8191 + 37)) % 2001) - 1000) * .001;
        for (int axis = 0; axis < 2; ++axis) {
            if (variant == 2 && (axis == 0 ? i % width == width - 1 : i / width == width - 1))
                continue;
            int j = axis == 0 ? (i / width) * width + (i + 1) % width : (i + width) % n;
            double u = double(mix(seed + i * 31ULL + axis * 100003ULL) % 1024) / 1023;
            double w = family ? std::exp2(8 * u - 4) : .5 + u;
            p.edges.push_back({i, j, w});
        }
    }
    for (auto e : p.edges) { p.diagonal[e.i] += e.weight; p.diagonal[e.j] += e.weight; }
    p.apply(p.truth, p.rhs);
    return p;
}
std::vector<Problem> training() {
    std::vector<Problem> bank;
    for (int f = 0; f < 2; ++f) for (int k = 0; k < 4; ++k)
        bank.push_back(problem(8, f, 0, 1000 + 100 * f + k));
    return bank;
}
std::vector<Problem> heldout() {
    std::vector<Problem> bank;
    for (int width : {8, 16}) for (int f = 0; f < 2; ++f)
        for (int v = 0; v < 3; ++v) for (int k = 0; k < 4; ++k)
            bank.push_back(problem(width, f, v, 1000000 + width * 10000 + f * 1000 + v * 100 + k));
    return bank;
}
double dot(const Vec& a, const Vec& b) {
    return std::inner_product(a.begin(), a.end(), b.begin(), 0.0);
}
double point(int op, double a, double b) {
    switch (op) {
        case Copy: return a;
        case Add: return a + b;
        case Sub: return a - b;
        case Mul: return a * b;
        case Div: return a / (std::abs(b) < 1e-12 ? (b < 0 ? -1e-12 : 1e-12) : b);
        case Neg: return -a;
        case Half: return .5 * a;
        case Min: return std::min(a, b);
        case Max: return std::max(a, b);
    }
    throw std::runtime_error("Unsupported point operation");
}
struct Result {
    double reward, residual, error, work;
    int iterations;
    bool valid;
    Vec x;
    bool accurate() const { return valid && residual <= tolerance && error <= error_tolerance; }
};
// Methods: -1 means interpreted program. Controls are evaluated only after search.
// 0: steepest descent; 1: diagonal descent; 2: diagonal PCG; 3: constant-space PCG.
Result solve(const Problem& p, const Program& program, int method, int steps) {
    int n = int(p.rhs.size());
    std::array<Vec, registers> reg;
    for (auto& x : reg) x.resize(n);
    Vec x(n), bestx(n), previous_step(n), previous_r(n), direction(n), ad(n), residual(n);
    const auto live = active(program);
    double b2 = dot(p.rhs, p.rhs), best = b2, work = 0, previous_rz = 0;
    double coarse_denominator = std::accumulate(p.shift.begin(), p.shift.end(), 0.0);
    // Charge a matvec and a reduction for the equivalent one-time coarse setup.
    if (method == 3) work += 1.25;
    bool valid = true;
    int done = 0;
    for (int t = 0; t < steps; ++t) {
        p.apply(x, ad);
        for (int i = 0; i < n; ++i) {
            reg[0][i] = p.rhs[i] - ad[i];
            reg[1][i] = reg[0][i] / p.diagonal[i];
            reg[2][i] = previous_step[i]; reg[3][i] = previous_r[i];
            reg[4][i] = previous_r[i] / p.diagonal[i]; reg[5][i] = 1;
        }
        work += 1;
        if (method == -1) {
            for (int k = 0; k < length; ++k) if (live[k]) {
                auto q = program[k];
                const Vec &a = reg[q.a], &b = reg[q.b];
                Vec& dest = reg[base + k];
                if (q.op == Matvec) p.apply(a, dest);
                else if (q.op == Dot || q.op == Mean) {
                    double value = q.op == Dot ? dot(a, b) : std::accumulate(a.begin(), a.end(), 0.0) / n;
                    std::fill(dest.begin(), dest.end(), value);
                } else for (int i = 0; i < n; ++i) dest[i] = point(q.op, a[i], b[i]);
                work += q.op == Matvec ? 1 : q.op == Dot || q.op == Mean ? .25 : .0625;
            }
            direction = reg.back();
        } else {
            Vec z = method == 0 ? reg[0] : reg[1];
            if (method == 3) {
                double c = std::accumulate(reg[0].begin(), reg[0].end(), 0.0) / coarse_denominator;
                for (auto& v : z) v += c;
                work += .3125;
            }
            if (method >= 2) {
                double rz = dot(reg[0], z);
                double beta = t ? rz / std::max(previous_rz, 1e-30) : 0;
                for (int i = 0; i < n; ++i) direction[i] = z[i] + beta * direction[i];
                previous_rz = rz; work += .3125;
            } else direction = z;
        }
        p.apply(direction, ad);
        double dad = dot(direction, ad), rd = dot(reg[0], direction);
        double alpha = dad > 1e-30 ? rd / dad : 0;
        work += 1.5;
        if (!std::isfinite(alpha) || std::any_of(direction.begin(), direction.end(),
            [](double v) { return !std::isfinite(v) || std::abs(v) > 1e18; })) {
            valid = false; break;
        }
        for (int i = 0; i < n; ++i) {
            previous_step[i] = alpha * direction[i]; x[i] += previous_step[i];
        }
        previous_r = reg[0];
        p.apply(x, ad);
        for (int i = 0; i < n; ++i) residual[i] = p.rhs[i] - ad[i];
        double rr = dot(residual, residual);
        work += 1.25; done = t + 1;
        if (!std::isfinite(rr)) { valid = false; break; }
        if (rr < best) { best = rr; bestx = x; }
        if (best <= tolerance * tolerance * b2) break;
    }
    double error2 = 0;
    for (int i = 0; i < n; ++i) error2 += (bestx[i] - p.truth[i]) * (bestx[i] - p.truth[i]);
    double relative = std::sqrt(best / b2), error = std::sqrt(error2 / dot(p.truth, p.truth));
    double reward = valid ? std::min(5.0, -std::log10(std::max(relative, tolerance))) - .001 * work : -5;
    return {reward, relative, error, work, done, valid, bestx};
}
void print_vector(const Vec& v) {
    std::cout << '[';
    for (size_t i = 0; i < v.size(); ++i) { if (i) std::cout << ','; std::cout << v[i]; }
    std::cout << ']';
}
void print_result(const Problem& p, const Result& r, bool vectors) {
    std::cout << "{\"width\":" << p.width << ",\"family\":" << p.family
        << ",\"variant\":" << p.variant << ",\"problem_seed\":" << p.seed
        << ",\"reward\":" << r.reward << ",\"residual\":" << r.residual
        << ",\"error\":" << r.error << ",\"work\":" << r.work
        << ",\"iterations\":" << r.iterations << ",\"valid\":" << (r.valid ? "true" : "false")
        << ",\"accurate\":" << (r.accurate() ? "true" : "false");
    if (vectors) {
        std::cout << ",\"x\":"; print_vector(r.x);
        std::cout << ",\"truth\":"; print_vector(p.truth);
        std::cout << ",\"rhs\":"; print_vector(p.rhs);
    }
    std::cout << '}';
}

struct Candidate { Program p; double reward; std::string key; };
void search(uint64_t seed, int proposals, bool random_only) {
    const auto start = std::chrono::steady_clock::now();
    Random rng{seed};
    auto bank = training();
    std::vector<Candidate> elite;
    // Exact expression keys preserve evaluation order, copies, and charged work.
    // Full triples are used, so no algebraic equality is assumed.
    std::set<std::string> seen;
    int evaluated = 0;
    std::vector<std::pair<int, double>> trace;
    for (int i = 0; i < proposals; ++i) {
        Program p = random_only || elite.empty() || rng.index(4) == 0
            ? random_program(rng) : mutate(elite[rng.index(int(elite.size()))].p, rng);
        const auto live = active(p);
        std::ostringstream key;
        for (int k = 0; k < length; ++k) {
            key << live[k] << ':';
            if (live[k]) key << p[k].op << ',' << p[k].a << ',' << (unary(p[k].op) ? 0 : p[k].b);
            key << ';';
        }
        if (seen.insert(key.str()).second) {
            double score = 0;
            for (const auto& system : bank) score += solve(system, p, -1, 48).reward;
            score /= bank.size(); ++evaluated;
            elite.push_back({p, score, key.str()});
            std::sort(elite.begin(), elite.end(), [](const Candidate& a, const Candidate& b) {
                return a.reward != b.reward ? a.reward > b.reward : a.key < b.key;
            });
            if (elite.size() > 32) elite.resize(32);
        }
        if ((i + 1) % 1024 == 0 || i + 1 == proposals) trace.push_back({i + 1, elite.front().reward});
    }
    double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
    std::cout << "{\"mode\":\"" << (random_only ? "random" : "evolution")
        << "\",\"seed\":" << seed << ",\"proposals\":" << proposals
        << ",\"evaluated_programs\":" << evaluated << ",\"evaluated_solves\":" << evaluated * bank.size()
        << ",\"seconds\":" << elapsed << ",\"training_reward\":" << elite.front().reward
        << ",\"expression\":\"" << expression(elite.front().p) << "\",\"program\":";
    print_program(elite.front().p);
    std::cout << ",\"trace\":[";
    for (size_t i = 0; i < trace.size(); ++i) {
        if (i) std::cout << ',';
        std::cout << '[' << trace[i].first << ',' << trace[i].second << ']';
    }
    std::cout << "]}\n";
}
int main(int argc, char** argv) {
    try {
        std::cout << std::setprecision(17);
        std::string mode = argc > 1 ? argv[1] : "help", path;
        uint64_t seed = 11;
        int proposals = 16384, method = -1, steps = 128;
        bool random_only = false, vectors = false, train = false;
        for (int i = 2; i < argc; ++i) {
            std::string flag = argv[i];
            if (flag == "--random") { random_only = true; continue; }
            if (flag == "--vectors") { vectors = true; continue; }
            if (flag == "--training") { train = true; continue; }
            if (i + 1 == argc) throw std::runtime_error("Missing option value");
            std::string value = argv[++i];
            if (flag == "--program") path = value;
            else if (flag == "--seed") seed = std::stoull(value);
            else if (flag == "--proposals") proposals = std::stoi(value);
            else if (flag == "--method") method = std::stoi(value);
            else if (flag == "--steps") steps = std::stoi(value);
            else throw std::runtime_error("Unknown option");
        }
        if (proposals < 1 || proposals > 1000000 || steps < 1 || steps > 512 || method < -1 || method > 3)
            throw std::runtime_error("Resource or method limit exceeded");
        if (mode == "search") { search(seed, proposals, random_only); return 0; }
        if (mode == "evaluate") {
            Program p{};
            if (method == -1) p = read_program(path);
            auto bank = train ? training() : heldout();
            std::cout << "{\"split\":\"" << (train ? "training" : "heldout")
                << "\",\"method\":" << method << ",\"steps\":" << steps << ",\"rows\":[";
            for (size_t i = 0; i < bank.size(); ++i) {
                if (i) std::cout << ',';
                print_result(bank[i], solve(bank[i], p, method, steps), vectors);
            }
            std::cout << "]}\n"; return 0;
        }
        std::cerr << "Usage: demo search [--seed N] [--proposals N] [--random]\n"
            << "       demo evaluate --program FILE [--steps N] [--vectors] [--training]\n"
            << "       demo evaluate --method 0|1|2|3 [--steps N] [--vectors]\n";
        return mode == "help" ? 0 : 1;
    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
