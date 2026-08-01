/*
 * ealgebra.h — Embedded algebra library (single header)
 *
 * Vector, matrix, quaternion, scalar, and trigonometric math
 * in pure C without math.h.
 *
 * Naming: all public identifiers carry the 'e' prefix (evector, esin, …)
 * to avoid clashes when linked alongside code that uses <math.h>.
 */
#ifndef EALGEBRA_H
#define EALGEBRA_H

#ifdef __cplusplus
extern "C" {
#endif

/* ===== Constants ===== */

#define E_PI    3.14159265358979323846f
#define E_PI_2  (E_PI * 0.5f)
#define E_2PI   (E_PI * 2.0f)
#define E_E     2.71828182845904523536f   /* Euler's number */

/* ===== Vector ===== */

typedef struct {
    float *data;   /* pointer to element storage (caller-owned) */
    int    len;    /* number of elements                        */
} evector;

static inline evector evec_make(float *data, int len) {
    evector v;
    v.data = data;
    v.len  = len;
    return v;
}

float  evec_dot      (const evector *a, const evector *b);
float  evec_norm     (const evector *v);                               /* L2 magnitude */
float  evec_distance (const evector *a, const evector *b);            /* ‖a − b‖    */
void   evec_cross    (const evector *a, const evector *b, evector *result);
void   evec_normalize(const evector *v, evector *out);
void   evec_scale    (const evector *v, float k, evector *out);
void   evec_add      (const evector *a, const evector *b, evector *out);
void   evec_mul      (const evector *a, const evector *b, evector *out);

/* ===== Matrix (row-major storage) ===== */

typedef struct {
    float *data;   /* row-major: data[row * cols + col] */
    int    rows;
    int    cols;
} ematrix;

static inline ematrix emat_make(float *data, int rows, int cols) {
    ematrix m;
    m.data = data;
    m.rows = rows;
    m.cols = cols;
    return m;
}

static inline float emat_at(const ematrix *m, int row, int col) {
    return m->data[row * m->cols + col];
}

static inline void emat_set(ematrix *m, int row, int col, float val) {
    m->data[row * m->cols + col] = val;
}

void   etranspose  (const ematrix *src, ematrix *dst);
void   emat_mul    (const ematrix *a, const ematrix *b, ematrix *out);
void   emat_vec_mul(const ematrix *m, const evector *v, evector *out);
void   emat_identity(ematrix *m);
void   emat_scale  (const ematrix *m, float k, ematrix *out);
void   emat_add    (const ematrix *a, const ematrix *b, ematrix *out);
void   emat_sub    (const ematrix *a, const ematrix *b, ematrix *out);
float  emat_trace  (const ematrix *m);

/* Rotation matrices (3×3, caller provides storage) */
void emat_rot_x(float angle, ematrix *out);
void emat_rot_y(float angle, ematrix *out);
void emat_rot_z(float angle, ematrix *out);

/* Skew-symmetric matrix from a 3-vector:
 *   [0  −v₂  v₁]
 *   [v₂  0  −v₀]
 *   [−v₁ v₀  0]    */
void emat_skew(const evector *v, ematrix *out);

/* Determinant — generic interface delegates by size */
float edet     (const ematrix *m);
float edet_2x2 (const ematrix *m);
float edet_3x3 (const ematrix *m);
float edet_nxn (const ematrix *m);

/* Inverse — returns 0 on success, -1 if singular or non-square */
int einverse     (const ematrix *src, ematrix *dst);
int einverse_2x2 (const ematrix *src, ematrix *dst);
int einverse_3x3 (const ematrix *src, ematrix *dst);
int einverse_nxn (const ematrix *src, ematrix *dst);

/* Frobenius norm — sqrt(sum of squares of all elements) */
float emat_frobenius(const ematrix *m);

/* Solve Ax = b via Gaussian elimination with partial pivoting.
 * A must be n×n, b must have n elements, x must have n elements.
 * Returns 0 on success, -1 if A is singular. */
int emat_solve(const ematrix *A, const evector *b, evector *x);

/* Orthogonality check — returns 1 if QᵀQ ≈ I, 0 otherwise.
 * m must be square (n×n). */
int emat_is_orthogonal(const ematrix *m);

/* LU decomposition with partial pivoting.
 * Decomposes A (n×n) into lower-triangular L (unit diagonal) and
 * upper-triangular U. Returns 0 on success, -1 if singular.
 * L and U must be n×n, caller-allocated. */
int emat_lu(const ematrix *A, ematrix *L, ematrix *U);

/* QR decomposition via modified Gram-Schmidt.
 * Decomposes A (m×n, m≥n) into orthonormal Q (m×n) and upper-
 * triangular R (n×n). Q and R must be caller-allocated. */
void emat_qr(const ematrix *A, ematrix *Q, ematrix *R);

/* Eigenvalues of a symmetric 3×3 matrix using closed-form characteristic
 * polynomial. Results are sorted descending into vals.
 * Works only for 3×3 symmetric matrices. */
void emat_eigen_3x3_symmetric(const ematrix *m, float vals[3]);

/* Matrix exponential e^A via Taylor series with scaling-and-squaring.
 * A must be n×n. out must be n×n, caller-allocated. */
void emat_exp(const ematrix *A, ematrix *out);

/* ===== Scalar math (no math.h) ===== */

float eabs  (float x);                    /* absolute value */
float esign (float x);                    /* signum: -1, 0, or +1 */
float emod  (float a, float b);           /* floating-point modulo: a - floor(a/b)*b */
float esqrt (float x);
float eexp  (float x);                    /* Taylor series */
float eln   (float x);                    /* Newton-Raphson */
float epow  (float base, float exponent);
float eclamp(float x, float lo, float hi);
float elerp (float a, float b, float t);  /* a + t(b − a) */
float easin (float x);
float eacos (float x);
float eatan (float x);
float eatan2(float y, float x);
float emap  (float x, float in_min, float in_max,
             float out_min, float out_max);

float esigmoid(float x);                   /* 1 / (1 + e^-x)    */
float etanh  (float x);                    /* hyperbolic tangent */
float efloor (float x);                    /* floor              */
float eceil  (float x);                    /* ceiling            */
float eround (float x);                    /* round to nearest   */

/* Bilinear interpolation across a 2×2 grid of values.
 * fx, fy ∈ [0,1] are fractional coordinates. */
float ebilerp(float v00, float v10, float v01, float v11,
              float fx, float fy);

/* ===== Angle conversion ===== */

float edeg2rad(float deg);               /* degrees → radians */
float erad2deg(float rad);               /* radians → degrees */

/* ===== Trigonometry (Taylor series) ===== */

float esin(float x);
float ecos(float x);
float etan(float x);

/* ===== Quaternion ===== */
/* Stored as (w, x, y, z) = w + xi + yj + zk  (Hamilton convention) */

typedef struct {
    float w, x, y, z;
} equat;

static inline equat equat_make(float w, float x, float y, float z) {
    equat q = {w, x, y, z};
    return q;
}

/* Build from axis-angle (axis must be normalized, angle in radians) */
equat equat_from_axis_angle(const evector *axis, float angle);

/* Hamilton product: result = a ⊗ b */
equat equat_mul(const equat *a, const equat *b);

/* Normalize to unit quaternion */
equat equat_normalize(const equat *q);

/* Conjugate (negate vector part) */
equat equat_conjugate(const equat *q);

/* Convert to 3×3 rotation matrix (caller provides 9-float storage) */
void equat_to_matrix(const equat *q, ematrix *out);

/* Rotate a 3-vector by quaternion */
void equat_rotate_vec(const equat *q, const evector *v, evector *out);

/* Spherical linear interpolation (shortest path) */
equat equat_slerp(const equat *a, const equat *b, float t);

/* ===== Euler angles (roll-pitch-yaw, intrinsic XYZ / Tait-Bryan) ===== */

/* Convert quaternion to Euler angles (roll, pitch, yaw) in radians.
 * out->data[0] = roll (X), out->data[1] = pitch (Y), out->data[2] = yaw (Z).
 * Uses the standard XYZ intrinsic sequence. */
void euler_from_quat(const equat *q, evector *out);

/* Convert a 3×3 rotation matrix to Euler angles (roll, pitch, yaw) in radians.
 * m must be 3×3. out->data[0] = roll, [1] = pitch, [2] = yaw. */
void euler_from_matrix(const ematrix *m, evector *out);

#ifdef __cplusplus
}
#endif

#endif /* EALGEBRA_H */
