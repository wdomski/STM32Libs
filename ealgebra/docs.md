# ealgebra — Quick Reference

## Data types

```c
evector  { float *data; int len; }
ematrix  { float *data; int rows, cols; }   // row-major
equat    { float w, x, y, z; }               // Hamilton convention
```

## Constants

```c
E_PI     3.14159265f
E_PI_2   π/2
E_2PI    2π
E_E      2.71828183f
```

## Vector

| Signature | Description |
|-----------|-------------|
| `evector evec_make(float *data, int len)` | Wrap array into evector |
| `float evec_dot(const evector *a, const evector *b)` | Dot product |
| `float evec_norm(const evector *v)` | L2 magnitude ‖v‖ |
| `float evec_distance(const evector *a, const evector *b)` | ‖a − b‖ |
| `void evec_cross(const evector *a, const evector *b, evector *result)` | 3D cross product |
| `void evec_normalize(const evector *v, evector *out)` | Scale to unit length |
| `void evec_scale(const evector *v, float k, evector *out)` | out = v × k |
| `void evec_add(const evector *a, const evector *b, evector *out)` | Element-wise add |
| `void evec_mul(const evector *a, const evector *b, evector *out)` | Element-wise (Hadamard) product |

## Matrix

| Signature | Description |
|-----------|-------------|
| `ematrix emat_make(float *data, int rows, int cols)` | Wrap array into ematrix |
| `float emat_at(const ematrix *m, int row, int col)` | Read element |
| `void emat_set(ematrix *m, int row, int col, float val)` | Write element |
| `void etranspose(const ematrix *src, ematrix *dst)` | Transpose |
| `void emat_mul(const ematrix *a, const ematrix *b, ematrix *out)` | C = A × B |
| `void emat_vec_mul(const ematrix *m, const evector *v, evector *out)` | y = M × x |
| `void emat_identity(ematrix *m)` | Fill identity matrix |
| `void emat_scale(const ematrix *m, float k, ematrix *out)` | Multiply all by scalar |
| `void emat_add(const ematrix *a, const ematrix *b, ematrix *out)` | Element-wise add |
| `void emat_sub(const ematrix *a, const ematrix *b, ematrix *out)` | Element-wise subtract |
| `float emat_trace(const ematrix *m)` | Sum of diagonal |
| `void emat_rot_x(float angle, ematrix *out)` | 3×3 rotation about X axis |
| `void emat_rot_y(float angle, ematrix *out)` | 3×3 rotation about Y axis |
| `void emat_rot_z(float angle, ematrix *out)` | 3×3 rotation about Z axis |
| `void emat_skew(const evector *v, ematrix *out)` | Skew-symmetric [v×] from 3-vector |
| `float edet(const ematrix *m)` | Determinant (dispatches by size) |
| `int einverse(const ematrix *src, ematrix *dst)` | Inverse (returns 0 ok, −1 singular) |
| `float emat_frobenius(const ematrix *m)` | Frobenius norm √(Σm²ᵢⱼ) |
| `int emat_solve(const ematrix *A, const evector *b, evector *x)` | Solve A·x = b via Gaussian elimination with partial pivoting (returns 0 ok, −1 singular) |
| `int emat_is_orthogonal(const ematrix *m)` | Returns 1 if QᵀQ ≈ I, 0 otherwise |
| `int emat_lu(const ematrix *A, ematrix *L, ematrix *U)` | LU decomposition with partial pivoting (returns 0 ok, −1 singular) |
| `void emat_qr(const ematrix *A, ematrix *Q, ematrix *R)` | QR decomposition via modified Gram-Schmidt |
| `void emat_eigen_3x3_symmetric(const ematrix *m, float vals[3])` | Eigenvalues of symmetric 3×3 matrix (sorted descending) |
| `void emat_exp(const ematrix *A, ematrix *out)` | Matrix exponential e^A via Taylor series with scaling-and-squaring |

## Scalar math

| Signature | Description |
|-----------|-------------|
| `float eabs(float x)` | Absolute value |x| |
| `float esign(float x)` | Signum: −1, 0, or +1 |
| `float emod(float a, float b)` | Floating-point modulo (result sign follows a) |
| `float esqrt(float x)` | Square root (Babylonian method) |
| `float eexp(float x)` | Exponential eˣ |
| `float eln(float x)` | Natural log ln(x) |
| `float epow(float base, float exponent)` | base^exponent |
| `float eclamp(float x, float lo, float hi)` | Clamp x to [lo, hi] |
| `float elerp(float a, float b, float t)` | Linear interp: a + t(b−a) |
| `float emap(float x, float in_min, float in_max, float out_min, float out_max)` | Re-scale from input range to output range |
| `float esigmoid(float x)` | Sigmoid: 1/(1+e⁻ˣ) |
| `float etanh(float x)` | Hyperbolic tangent |
| `float efloor(float x)` | Floor (largest integer ≤ x) |
| `float eceil(float x)` | Ceiling (smallest integer ≥ x) |
| `float eround(float x)` | Round to nearest (ties away from zero) |
| `float ebilerp(float v00, float v10, float v01, float v11, float fx, float fy)` | Bilinear interpolation over a 2×2 grid |
| `float edeg2rad(float deg)` | Degrees → radians |
| `float erad2deg(float rad)` | Radians → degrees |

## Trigonometric

| Signature | Description |
|-----------|-------------|
| `float esin(float x)` | Sine (Maclaurin series) |
| `float ecos(float x)` | Cosine |
| `float etan(float x)` | Tangent |
| `float easin(float x)` | Arcsine (domain [−1,1]) |
| `float eacos(float x)` | Arccosine (domain [−1,1]) |
| `float eatan(float x)` | Arctangent |
| `float eatan2(float y, float x)` | Four-quadrant arctangent, range (−π, π] |

## Quaternion

| Signature | Description |
|-----------|-------------|
| `equat equat_make(float w, float x, float y, float z)` | Construct quaternion |
| `equat equat_from_axis_angle(const evector *axis, float angle)` | Build from axis-angle |
| `equat equat_mul(const equat *a, const equat *b)` | Hamilton product a ⊗ b |
| `equat equat_normalize(const equat *q)` | Unit quaternion |
| `equat equat_conjugate(const equat *q)` | Conjugate (w, −x, −y, −z) |
| `void equat_to_matrix(const equat *q, ematrix *out)` | Convert to 3×3 rotation matrix |
| `void equat_rotate_vec(const equat *q, const evector *v, evector *out)` | Rotate 3-vector by quaternion |
| `equat equat_slerp(const equat *a, const equat *b, float t)` | Spherical linear interpolation |

## Euler angles

| Signature | Description |
|-----------|-------------|
| `void euler_from_quat(const equat *q, evector *out)` | Convert quaternion to Euler angles (roll, pitch, yaw), XYZ intrinsic |
| `void euler_from_matrix(const ematrix *m, evector *out)` | Convert 3×3 rotation matrix to Euler angles (roll, pitch, yaw), XYZ intrinsic |
