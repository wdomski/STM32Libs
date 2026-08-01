# ealgebra — Embedded Algebra Library

Minimal vector, matrix, quaternion, scalar, and trigonometric math in pure C (no `math.h`).
Designed for microcontrollers and embedded systems.

- **Single source file** (`ealgebra.c`) + **single header** (`ealgebra.h`)
- **No dynamic allocation** — caller provides storage; structs wrap existing arrays
- **No `math.h`** — all math (trig, sqrt, exp, ln, inverse trig) implemented from scratch
- **DRY** — shared internal helpers, dispatch functions delegate to size-specific implementations
- All public names carry the `e` prefix (`evector`, `esin`, `edet`, …) to avoid clashes with `<math.h>`

---

## Build

```bash
mkdir build && cd build
cmake ..
make
ctest        # runs unit tests
```

---

## Quick reference

### Vector

```c
float a_data[] = {1, 2, 3};
float b_data[] = {4, 5, 6};
evector a = evec_make(a_data, 3);
evector b = evec_make(b_data, 3);

float  dot       = evec_dot(&a, &b);          /* 32 */
float  norm      = evec_norm(&a);             /* sqrt(14) */
float  dist      = evec_distance(&a, &b);     /* ||a-b|| */
evec_cross    (&a, &b, &result);              /* cross product (3D) */
evec_normalize(&a, &out);                     /* unit vector */
evec_scale    (&a, 2.0f, &out);
evec_add      (&a, &b, &out);
evec_mul      (&a, &b, &out);                 /* Hadamard product */
```

### Matrix

```c
float data[] = {1, 2, 3, 4, 5, 6};
ematrix m = emat_make(data, 2, 3);  /* 2 rows, 3 cols, row-major */

float val = emat_at(&m, 1, 2);      /* read element (1,2) */
emat_set(&m, 1, 2, 9.0f);           /* write element (1,2) */

emat_identity(&m);                  /* fill with identity */
emat_mul     (&A, &B, &C);          /* C = A × B */
emat_vec_mul (&M, &x, &y);          /* y = M × x */
emat_scale   (&m, 2.0f, &out);
emat_add     (&A, &B, &out);
emat_sub     (&A, &B, &out);
float tr = emat_trace(&m);          /* sum of diagonal */

/* Rotation matrices (3×3) */
emat_rot_x(angle, &R);              /* rotation about X axis */
emat_rot_y(angle, &R);              /* rotation about Y axis */
emat_rot_z(angle, &R);              /* rotation about Z axis */
emat_skew(&v, &S);                  /* skew-symmetric from 3-vector */
```
### Determinant, Transpose, Inverse

```c
/* Determinant, Transpose, Inverse */
float d = edet(&m);                 /* auto-dispatches by size */
etranspose(&src, &dst);
einverse(&m, &inv);                 /* returns 0 on success, -1 if singular */

/* Frobenius norm */
float fn = emat_frobenius(&m);      /* sqrt(sum of squares) */

/* Solve linear system Ax = b */
emat_solve(&A, &b, &x);             /* Gaussian elimination w/ partial pivoting */
```

### Scalar math

```c
float s = esqrt(16.0f);                     /* 4 */
float a = eabs(-3.5f);                      /* 3.5 */
float s = esign(-5.0f);                     /* -1 */
float m = emod(10.0f, 3.0f);               /* 1.0 */
float e = eexp(1.0f);                       /* e ≈ 2.71828 */
float l = eln(E_E);                         /* 1 */
float p = epow(2.0f, 3.0f);                 /* 8 */
float c = eclamp(x, lo, hi);               /* clamp to [lo, hi] */
float r = elerp(0.0f, 10.0f, 0.5f);         /* 5.0 */
float a = easin(1.0f);                      /* π/2 */
float a = eacos(0.0f);                      /* π/2 */
float a = eatan(1.0f);                      /* π/4 */
float h = eatan2(1.0f, 1.0f);               /* π/4 */
float m = emap(0.5f, 0, 1, 0, 100);         /* 50 */
float s = esigmoid(0.0f);                   /* 0.5 */
float t = etanh(1.0f);                      /* ≈ 0.7616 */
float f = efloor(3.7f);                     /* 3.0 */
float c = eceil(-3.2f);                     /* -3.0 */
float r = eround(2.5f);                     /* 3.0 */
float b = ebilerp(10, 20, 30, 40, 0.5f, 0.5f); /* 25.0 */
```

### Trigonometry

```c
float s = esin(E_PI / 4);   /* ≈ 0.7071 */
float c = ecos(E_PI / 3);   /* 0.5     */
float t = etan(E_PI / 4);   /* 1.0     */
```

### Quaternion

```c
/* Create from axis-angle (axis must be normalized) */
float axis_data[] = {0, 0, 1};
evector axis = evec_make(axis_data, 3);
equat q = equat_from_axis_angle(&axis, E_PI_2);   /* 90° about Z */

/* Quaternion operations */
equat p       = equat_mul(&q, &r);         /* Hamilton product */
equat n       = equat_normalize(&q);       /* unit quaternion */
equat conj    = equat_conjugate(&q);       /* conjugate */
equat mid     = equat_slerp(&a, &b, 0.5f); /* spherical lerp */

/* Convert to 3×3 rotation matrix */
float md[9];
ematrix M = emat_make(md, 3, 3);
equat_to_matrix(&q, &M);

/* Rotate a vector */
float vd[] = {1, 0, 0}, out[3];
evector v = evec_make(vd, 3);
evector o = evec_make(out, 3);
equat_rotate_vec(&q, &v, &o);              /* (1,0,0) → (0,1,0) */
```

### Euler angles

```c
/* Extract roll-pitch-yaw from a quaternion (XYZ intrinsic / Tait-Bryan) */
float ed[3];
evector euler = evec_make(ed, 3);
euler_from_quat(&q, &euler);               /* ed[0]=roll, ed[1]=pitch, ed[2]=yaw */

/* Extract from a 3×3 rotation matrix */
float rd[9];
ematrix R = emat_make(rd, 3, 3);
emat_rot_z(E_PI / 4.0f, &R);
euler_from_matrix(&R, &euler);             /* yaw = π/4, roll = pitch = 0 */
```

---

## API summary

| Category | Function | Description |
|----------|----------|-------------|
| **Vector** | `evec_make(ptr, len)` | Wrap float array into evector |
| | `evec_dot / evec_norm / evec_distance` | Dot, magnitude, distance |
| | `evec_cross / evec_normalize / evec_scale` | Cross, normalize, scale |
| | `evec_add / evec_mul` | Add, Hadamard product |
| **Matrix** | `emat_make / emat_at / emat_set` | Construct, access |
| | `etranspose / emat_mul / emat_vec_mul` | Transpose, multiply |
| | `emat_identity / emat_scale / emat_add / emat_sub` | Basic ops |
| | `emat_trace` | Diagonal sum |
| | `emat_rot_x / emat_rot_y / emat_rot_z` | Rotation matrices |
| | `emat_skew` | Skew-symmetric from vector |
| | `edet` / `einverse` | Determinant / inverse (dispatch by size) |
| | `emat_frobenius` | Frobenius norm √(Σm²ᵢⱼ) |
| | `emat_solve` | Solve Ax = b (Gaussian elimination w/ partial pivoting) |
| | `emat_is_orthogonal` | Check if QᵀQ = I |
| | `emat_lu` | LU decomposition with partial pivoting |
| | `emat_qr` | QR decomposition (modified Gram-Schmidt) |
| | `emat_eigen_3x3_symmetric` | Eigenvalues of symmetric 3×3 matrix |
| | `emat_exp` | Matrix exponential e^A |
| **Scalar** | `eabs / esign / emod` | Absolute value, signum, modulo |
| | `esqrt / eexp / eln / epow` | Sqrt, exp, log, power |
| | `eclamp / elerp` | Clamp, linear interpolation |
| | `easin / eacos / eatan / eatan2` | Inverse trig |
| | `emap` | Linear re-scaling |
| | `esigmoid / etanh` | Sigmoid, hyperbolic tangent |
| | `efloor / eceil / eround` | Floor, ceiling, round |
| | `ebilerp` | Bilinear interpolation |
| | `edeg2rad` / `erad2deg` | Degree ↔ radian conversion |
| **Trig** | `esin / ecos / etan` | Taylor series |
| **Quaternion** | `equat_make` | Construct from w,x,y,z |
| | `equat_from_axis_angle` | Construct from axis + angle |
| | `equat_mul` | Hamilton product |
| | `equat_normalize / equat_conjugate` | Normalize, conjugate |
| | `equat_to_matrix` | Convert to 3×3 rotation matrix |
| | `equat_rotate_vec` | Rotate a 3-vector |
| | `equat_slerp` | Spherical linear interpolation |
| **Euler** | `euler_from_quat` | Quaternion → roll-pitch-yaw (XYZ intrinsic) |
| | `euler_from_matrix` | 3×3 rotation matrix → roll-pitch-yaw |

---

## Design decisions

### Data representation
Both `evector` and `ematrix` are thin wrappers (pointer + dimensions) over **caller-owned storage**. The library never allocates memory. Quaternions use a value-type `equat` struct (4 floats) since they are small and frequently returned by value.

### Determinant and inverse dispatch
`edet()` and `einverse()` inspect `m->rows` and delegate to the appropriate closed-form implementation (`_2x2`, `_3x3`) or fall back to the general cofactor-expansion path (`_nxn`). This gives maximum performance for the common small-matrix cases while still supporting arbitrary sizes.

### Exponential and logarithm
`eexp(x)` uses a Taylor series with range reduction: split `x = k·ln2 + r`, compute `e^r` via Taylor (7 terms), then multiply by `2^k`. This keeps `|r| < ln2/2 ≈ 0.347` for fast convergence across the entire domain.

`eln(x)` range-reduces by factoring out powers of 2 so the mantissa `m ∈ [1, 2)`, then uses Newton-Raphson on `e^y − m = 0`.

`epow(base, exp)` is `eexp(exp * eln(base))`.

### Trigonometric function implementation
`esin(x)` uses a **Maclaurin series** (Taylor series centered at zero):

```
sin(x) = x − x³/3! + x⁵/5! − x⁷/7! + x⁹/9! − x¹¹/11! + x¹³/13!
```

Before the series is evaluated, the input angle is **range-reduced** to `[−π/2, π/2]` using trigonometric identities:
1. Subtract whole multiples of `2π` to get into `[−π, π]`
2. Reflect `[π/2, π]` back via `sin(π − x) = sin(x)`

This reduction is essential because the Taylor series converges rapidly near zero but diverges for large arguments. After reduction, 7 terms give `float`-level precision across the entire input domain.

**`ecos(x)`** is implemented as `esin(x + π/2)`. This reuses the same range reduction and Taylor machinery — true DRY. A dedicated cosine Taylor series would need its own range reducer because the reflection identity `sin(π − x) = sin(x)` used by the reducer becomes `cos(π − x) = −cos(x)` for cosine, which would produce incorrect sign changes.

**`etan(x)`** is simply `esin(x) / ecos(x)`.

The series uses **Horner-like recurrence** to compute each term from the previous:

```c
term *= -x² / (2k · (2k+1));
```

This avoids computing factorials (which overflow) and is numerically stable.

### Square root — Babylonian method
`esqrt(x)` uses the classic iterative formula `r = ½(r + x/r)`. It converges quadratically — starting from the identity estimate `r₀ = x`, 8 iterations suffice for full `float` precision on the entire domain.

### Inverse trigonometric functions
- **`easin(x)`** uses Newton-Raphson iteration on `sin(y) − x = 0` with initial guess `y₀ = x`. Each iteration: `y ← y − (sin(y) − x) / cos(y)`.
- **`eacos(x)`** uses the identity `acos(x) = π/2 − asin(x)` — DRY.
- **`eatan(x)`** uses a minimax polynomial approximation on `[-1, 1]`, extended to the full real line via `atan(x) = ±π/2 − atan(1/x)` for `|x| > 1`.
- **`eatan2(y, x)`** wraps `eatan` with quadrant logic to produce the four-quadrant result in `(−π, π]`.

### Quaternions
Quaternions use the **Hamilton convention**: `q = w + xi + yj + zk`.

- **Multiplication** (`equat_mul`): Standard Hamilton product `q_a ⊗ q_b`. The resulting rotation is "apply `q_a` first, then `q_b`" (equivalent to matrix multiplication `R_b × R_a`).
- **Vector rotation** (`equat_rotate_vec`): Uses the optimized formula `v' = v + 2w(u × v) + 2(u × (u × v))` where `u = (x, y, z)` is the vector part. This avoids full quaternion triple-product and is 18 multiply / 12 add operations.
- **Matrix conversion** (`equat_to_matrix`): Direct closed-form from quaternion components. Assumes a unit quaternion for best accuracy.
- **SLERP** (`equat_slerp`): Spherical linear interpolation along the shortest arc. Handles the double-cover (negates `b` if `dot < 0`) and falls back to normalized lerp when quaternions are nearly identical (within 0.9995) to avoid division by near-zero.

### Mapping
`emap()` performs linear interpolation from an input range to an output range:

```
out = (x − in_min) × (out_max − out_min) / (in_max − in_min) + out_min
```

Useful for sensor calibration (e.g. ADC counts to engineering units).

### Linear system solver
`emat_solve()` solves `Ax = b` using **Gaussian elimination with partial pivoting**. At each elimination step, the row with the largest absolute pivot element is swapped into position — this avoids division by small (near-zero) pivots and is numerically stable. Returns `-1` if the matrix is singular.

### Euler angles
Both `euler_from_quat()` and `euler_from_matrix()` extract roll-pitch-yaw using the **XYZ intrinsic (Tait-Bryan)** convention: `R = Rz(yaw) · Ry(pitch) · Rx(roll)`. Gimbal lock is handled explicitly: when pitch approaches ±90°, yaw is set to zero and roll is computed from the remaining equations, avoiding division-by-zero. The quaternion path computes `sin(pitch) = 2(w·y − x·z)` and clamps to `[−1, 1]` before `asin`.

### Sigmoid and tanh
`esigmoid(x) = 1/(1 + e^{-x})` with overflow guards: returns 1.0 for `x > 88` and 0.0 for `x < −88`. `etanh(x)` uses `(e^x − e^{-x})/(e^x + e^{-x})` and clamps to ±1 for `|x| > 10` where tanh has saturated.

---

## Project structure

```
ealgebra/
├── CMakeLists.txt              Build system
├── src/
│   ├── ealgebra.h              Public API header
│   └── ealgebra.c              Implementation
├── test/
│   └── test_ealgebra.c         86 unit tests (uses math.h for verification only)
├── README.md                   This file
├── docs.md                     Quick reference (signatures only)
└── FUTURE_EXTENSIONS.md        Report on potential additions
```

---

## License

Public domain / CC0. Use freely for any purpose.
