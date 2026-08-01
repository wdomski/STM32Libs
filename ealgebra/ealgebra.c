/*
 * ealgebra.c — Embedded algebra library (single source)
 */
#include "ealgebra.h"

/* ====================================================== *
 *  Internal helpers                                      *
 * ====================================================== */

static ematrix submatrix(const ematrix *m, float *buf, int r, int c) {
    int sub_cols = m->cols - 1;
    ematrix sub  = emat_make(buf, m->rows - 1, sub_cols);
    int si = 0;
    for (int i = 0; i < m->rows; i++) {
        if (i == r) continue;
        int sj = 0;
        for (int j = 0; j < m->cols; j++) {
            if (j == c) continue;
            sub.data[si * sub_cols + sj] = m->data[i * m->cols + j];
            sj++;
        }
        si++;
    }
    return sub;
}

static float reduce_angle(float x) {
    x -= (float)((int)(x / E_2PI)) * E_2PI;
    if (x >  E_PI)  x -= E_2PI;
    if (x < -E_PI)  x += E_2PI;
    if (x >  E_PI_2) x =  E_PI - x;
    if (x < -E_PI_2) x = -E_PI - x;
    return x;
}

/* ====================================================== *
 *  Vector                                                 *
 * ====================================================== */

float evec_dot(const evector *a, const evector *b) {
    float result = 0.0f;
    for (int i = 0; i < a->len; i++)
        result += a->data[i] * b->data[i];
    return result;
}

float evec_norm(const evector *v) {
    return esqrt(evec_dot(v, v));
}

float evec_distance(const evector *a, const evector *b) {
    int len = a->len;
    float sum = 0.0f;
    for (int i = 0; i < len; i++) {
        float d = a->data[i] - b->data[i];
        sum += d * d;
    }
    return esqrt(sum);
}

void evec_cross(const evector *a, const evector *b, evector *result) {
    const float *x = a->data, *y = b->data;
    result->data[0] = x[1] * y[2] - x[2] * y[1];
    result->data[1] = x[2] * y[0] - x[0] * y[2];
    result->data[2] = x[0] * y[1] - x[1] * y[0];
}

void evec_normalize(const evector *v, evector *out) {
    float mag = esqrt(evec_dot(v, v));
    if (mag > 0.0f) {
        float inv = 1.0f / mag;
        for (int i = 0; i < v->len; i++)
            out->data[i] = v->data[i] * inv;
    }
}

void evec_scale(const evector *v, float k, evector *out) {
    for (int i = 0; i < v->len; i++)
        out->data[i] = v->data[i] * k;
}

void evec_add(const evector *a, const evector *b, evector *out) {
    for (int i = 0; i < a->len; i++)
        out->data[i] = a->data[i] + b->data[i];
}

void evec_mul(const evector *a, const evector *b, evector *out) {
    for (int i = 0; i < a->len; i++)
        out->data[i] = a->data[i] * b->data[i];
}

/* ====================================================== *
 *  Matrix — basic operations                             *
 * ====================================================== */

void etranspose(const ematrix *src, ematrix *dst) {
    for (int i = 0; i < src->rows; i++)
        for (int j = 0; j < src->cols; j++)
            dst->data[j * dst->cols + i] = src->data[i * src->cols + j];
}

void emat_mul(const ematrix *a, const ematrix *b, ematrix *out) {
    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < b->cols; j++) {
            float sum = 0.0f;
            for (int k = 0; k < a->cols; k++)
                sum += a->data[i * a->cols + k] * b->data[k * b->cols + j];
            out->data[i * out->cols + j] = sum;
        }
    }
}

void emat_vec_mul(const ematrix *m, const evector *v, evector *out) {
    for (int i = 0; i < m->rows; i++) {
        float sum = 0.0f;
        for (int j = 0; j < m->cols; j++)
            sum += m->data[i * m->cols + j] * v->data[j];
        out->data[i] = sum;
    }
}

void emat_identity(ematrix *m) {
    for (int i = 0; i < m->rows; i++)
        for (int j = 0; j < m->cols; j++)
            m->data[i * m->cols + j] = (i == j) ? 1.0f : 0.0f;
}

void emat_scale(const ematrix *m, float k, ematrix *out) {
    int total = m->rows * m->cols;
    for (int i = 0; i < total; i++)
        out->data[i] = m->data[i] * k;
}

void emat_add(const ematrix *a, const ematrix *b, ematrix *out) {
    int total = a->rows * a->cols;
    for (int i = 0; i < total; i++)
        out->data[i] = a->data[i] + b->data[i];
}

void emat_sub(const ematrix *a, const ematrix *b, ematrix *out) {
    int total = a->rows * a->cols;
    for (int i = 0; i < total; i++)
        out->data[i] = a->data[i] - b->data[i];
}

float emat_trace(const ematrix *m) {
    int n = (m->rows < m->cols) ? m->rows : m->cols;
    float sum = 0.0f;
    for (int i = 0; i < n; i++)
        sum += m->data[i * m->cols + i];
    return sum;
}

/* ====================================================== *
 *  Matrix — rotation & skew                              *
 * ====================================================== */

void emat_rot_x(float angle, ematrix *out) {
    float c = ecos(angle), s = esin(angle);
    emat_identity(out);
    out->data[4] =  c;  out->data[5] = -s;
    out->data[7] =  s;  out->data[8] =  c;
}

void emat_rot_y(float angle, ematrix *out) {
    float c = ecos(angle), s = esin(angle);
    emat_identity(out);
    out->data[0] =  c;  out->data[2] =  s;
    out->data[6] = -s;  out->data[8] =  c;
}

void emat_rot_z(float angle, ematrix *out) {
    float c = ecos(angle), s = esin(angle);
    emat_identity(out);
    out->data[0] =  c;  out->data[1] = -s;
    out->data[3] =  s;  out->data[4] =  c;
}

void emat_skew(const evector *v, ematrix *out) {
    /* [  0  -v2  v1]
     * [ v2   0  -v0]
     * [-v1  v0   0] */
    float v0 = v->data[0], v1 = v->data[1], v2 = v->data[2];
    out->data[0] = 0.0f;  out->data[1] = -v2;    out->data[2] =  v1;
    out->data[3] = v2;    out->data[4] = 0.0f;   out->data[5] = -v0;
    out->data[6] = -v1;   out->data[7] = v0;     out->data[8] = 0.0f;
}

/* ====================================================== *
 *  Matrix — determinant                                   *
 * ====================================================== */

float edet_2x2(const ematrix *m) {
    return m->data[0] * m->data[3] - m->data[1] * m->data[2];
}

float edet_3x3(const ematrix *m) {
    const float *d = m->data;
    return d[0] * (d[4] * d[8] - d[5] * d[7])
         - d[1] * (d[3] * d[8] - d[5] * d[6])
         + d[2] * (d[3] * d[7] - d[4] * d[6]);
}

float edet_nxn(const ematrix *m) {
    int n  = m->rows;
    int sz = n - 1;
    float buf[sz * sz];
    float result = 0.0f;
    for (int j = 0; j < n; j++) {
        ematrix sub = submatrix(m, buf, 0, j);
        float cofactor = m->data[j] * edet(&sub);
        result += (j & 1) ? -cofactor : cofactor;
    }
    return result;
}

float edet(const ematrix *m) {
    if (m->rows != m->cols)
        return 0.0f;
    switch (m->rows) {
        case 1:  return m->data[0];
        case 2:  return edet_2x2(m);
        case 3:  return edet_3x3(m);
        default: return edet_nxn(m);
    }
}

/* ====================================================== *
 *  Matrix — inverse                                       *
 * ====================================================== */

int einverse_2x2(const ematrix *src, ematrix *dst) {
    float det = edet_2x2(src);
    if (det == 0.0f) return -1;
    float inv_det = 1.0f / det;
    float *d = src->data;
    dst->data[0] =  d[3] * inv_det;
    dst->data[1] = -d[1] * inv_det;
    dst->data[2] = -d[2] * inv_det;
    dst->data[3] =  d[0] * inv_det;
    return 0;
}

int einverse_3x3(const ematrix *src, ematrix *dst) {
    float det = edet_3x3(src);
    if (det == 0.0f) return -1;
    float inv_det = 1.0f / det;
    const float *d = src->data;
    dst->data[0] =  (d[4] * d[8] - d[5] * d[7]) * inv_det;
    dst->data[1] = -(d[1] * d[8] - d[2] * d[7]) * inv_det;
    dst->data[2] =  (d[1] * d[5] - d[2] * d[4]) * inv_det;
    dst->data[3] = -(d[3] * d[8] - d[5] * d[6]) * inv_det;
    dst->data[4] =  (d[0] * d[8] - d[2] * d[6]) * inv_det;
    dst->data[5] = -(d[0] * d[5] - d[2] * d[3]) * inv_det;
    dst->data[6] =  (d[3] * d[7] - d[4] * d[6]) * inv_det;
    dst->data[7] = -(d[0] * d[7] - d[1] * d[6]) * inv_det;
    dst->data[8] =  (d[0] * d[4] - d[1] * d[3]) * inv_det;
    return 0;
}

int einverse_nxn(const ematrix *src, ematrix *dst) {
    int n  = src->rows;
    float det = edet_nxn(src);
    if (det == 0.0f) return -1;
    float inv_det = 1.0f / det;
    int sz = n - 1;
    float buf[sz * sz];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            ematrix sub = submatrix(src, buf, j, i);
            float minor = edet(&sub);
            float sign  = ((i + j) & 1) ? -1.0f : 1.0f;
            dst->data[i * n + j] = sign * minor * inv_det;
        }
    }
    return 0;
}

int einverse(const ematrix *src, ematrix *dst) {
    if (src->rows != src->cols)
        return -1;
    switch (src->rows) {
        case 1:
            if (src->data[0] == 0.0f) return -1;
            dst->data[0] = 1.0f / src->data[0];
            return 0;
        case 2:  return einverse_2x2(src, dst);
        case 3:  return einverse_3x3(src, dst);
        default: return einverse_nxn(src, dst);
    }
}

/* ====================================================== *
 *  Matrix — Frobenius norm & linear-system solver        *
 * ====================================================== */

float emat_frobenius(const ematrix *m) {
    /* Frobenius norm: sqrt(Σ m_ij²)
     * Directly sums squares and takes sqrt once for accuracy. */
    int total = m->rows * m->cols;
    float sum = 0.0f;
    for (int i = 0; i < total; i++)
        sum += m->data[i] * m->data[i];
    return esqrt(sum);
}

int emat_solve(const ematrix *A, const evector *b, evector *x) {
    /* Solve A·x = b using Gaussian elimination with partial pivoting.
     * A must be n×n, b and x must have n elements.
     * Modifies a local copy of A and b; writes result into x.
     * Returns 0 on success, -1 if A is singular. */
    int n = A->rows;
    if (A->rows != A->cols || b->len != n || x->len != n)
        return -1;

    /* Build augmented matrix [A | b] in caller-provided-sized local buffer.
     * VLAs are used consistently with edet_nxn style. */
    float aug[n * (n + 1)];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            aug[i * (n + 1) + j] = A->data[i * n + j];
        aug[i * (n + 1) + n] = b->data[i];
    }

    /* Forward elimination with partial pivoting */
    for (int col = 0; col < n; col++) {
        /* Find pivot row (largest absolute value in this column at/below `col`) */
        int pivot = col;
        float max_val = eabs(aug[col * (n + 1) + col]);
        for (int row = col + 1; row < n; row++) {
            float val = eabs(aug[row * (n + 1) + col]);
            if (val > max_val) {
                max_val = val;
                pivot = row;
            }
        }
        if (max_val == 0.0f)
            return -1;   /* singular */

        /* Swap rows if necessary */
        if (pivot != col) {
            for (int j = 0; j <= n; j++) {
                float tmp = aug[col * (n + 1) + j];
                aug[col * (n + 1) + j] = aug[pivot * (n + 1) + j];
                aug[pivot * (n + 1) + j] = tmp;
            }
        }

        /* Eliminate this column in rows below */
        for (int row = col + 1; row < n; row++) {
            float factor = aug[row * (n + 1) + col] / aug[col * (n + 1) + col];
            for (int j = col; j <= n; j++)
                aug[row * (n + 1) + j] -= factor * aug[col * (n + 1) + j];
        }
    }

    /* Back-substitution */
    for (int i = n - 1; i >= 0; i--) {
        float s = aug[i * (n + 1) + n];
        for (int j = i + 1; j < n; j++)
            s -= aug[i * (n + 1) + j] * x->data[j];
        x->data[i] = s / aug[i * (n + 1) + i];
    }
    return 0;
}

/* ====================================================== *
 *  Matrix — orthogonal check, LU, QR, eigen, exp         *
 * ====================================================== */

int emat_is_orthogonal(const ematrix *m) {
    /* Returns 1 if Qᵀ·Q ≈ I (within 1e-3), 0 otherwise.
     * Uses caller-provided temp buffers via VLAs. */
    int n = m->rows;
    if (m->rows != m->cols)
        return 0;

    float td[n * n];   /* transpose */
    float pd[n * n];   /* product */
    ematrix T = emat_make(td, n, n);
    ematrix P = emat_make(pd, n, n);

    etranspose(m, &T);
    emat_mul(&T, m, &P);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            float expected = (i == j) ? 1.0f : 0.0f;
            if (eabs(P.data[i * n + j] - expected) > 1e-3f)
                return 0;
        }
    return 1;
}

int emat_lu(const ematrix *A, ematrix *L, ematrix *U) {
    /* LU decomposition with partial pivoting (Doolittle form).
     * L has unit diagonal, U is upper-triangular.
     * Returns 0 on success, -1 if singular. */
    int n = A->rows;
    if (A->rows != A->cols || L->rows != n || U->rows != n)
        return -1;

    /* Copy A into U, start L as identity */
    for (int i = 0; i < n * n; i++)
        U->data[i] = A->data[i];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            L->data[i * n + j] = (i == j) ? 1.0f : 0.0f;

    for (int k = 0; k < n; k++) {
        /* Partial pivoting: find max element in column k, rows k..n-1 */
        int pivot = k;
        float max_val = eabs(U->data[k * n + k]);
        for (int i = k + 1; i < n; i++) {
            float val = eabs(U->data[i * n + k]);
            if (val > max_val) {
                max_val = val;
                pivot = i;
            }
        }
        if (max_val == 0.0f)
            return -1;  /* singular */

        /* Swap rows in U and L (L columns 0..k-1) */
        if (pivot != k) {
            for (int j = 0; j < n; j++) {
                float utmp = U->data[k * n + j];
                U->data[k * n + j] = U->data[pivot * n + j];
                U->data[pivot * n + j] = utmp;
                if (j < k) {
                    float ltmp = L->data[k * n + j];
                    L->data[k * n + j] = L->data[pivot * n + j];
                    L->data[pivot * n + j] = ltmp;
                }
            }
        }

        /* Eliminate below diagonal */
        for (int i = k + 1; i < n; i++) {
            float factor = U->data[i * n + k] / U->data[k * n + k];
            L->data[i * n + k] = factor;
            for (int j = k; j < n; j++)
                U->data[i * n + j] -= factor * U->data[k * n + j];
        }
    }
    return 0;
}

void emat_qr(const ematrix *A, ematrix *Q, ematrix *R) {
    /* QR decomposition via modified Gram-Schmidt.
     * A is m×n (m≥n). Q is m×n with orthonormal columns, R is n×n upper-triangular. */
    int m = A->rows, n = A->cols;
    int qn = (m < n) ? m : n;  /* min(m,n) */

    /* Initialize Q = A, R = 0 */
    for (int i = 0; i < m * n; i++)
        Q->data[i] = A->data[i];
    for (int i = 0; i < qn * qn; i++)
        R->data[i] = 0.0f;

    for (int k = 0; k < qn; k++) {
        /* R[k][k] = ||Q[:,k]|| */
        float norm = 0.0f;
        for (int i = 0; i < m; i++)
            norm += Q->data[i * n + k] * Q->data[i * n + k];
        norm = esqrt(norm);

        if (norm < 1e-12f) {
            R->data[k * qn + k] = 0.0f;
            continue;
        }

        R->data[k * qn + k] = norm;

        /* Normalize column k of Q */
        float inv_norm = 1.0f / norm;
        for (int i = 0; i < m; i++)
            Q->data[i * n + k] *= inv_norm;

        /* Orthogonalize remaining columns against column k */
        for (int j = k + 1; j < n; j++) {
            float dot = 0.0f;
            for (int i = 0; i < m; i++)
                dot += Q->data[i * n + k] * Q->data[i * n + j];
            R->data[k * qn + j] = dot;
            for (int i = 0; i < m; i++)
                Q->data[i * n + j] -= dot * Q->data[i * n + k];
        }
    }
}

void emat_eigen_3x3_symmetric(const ematrix *m, float vals[3]) {
    /* Closed-form eigenvalues for a 3×3 symmetric matrix via the
     * characteristic polynomial. Uses Cardano's formula.
     *
     * For symmetric A = [[a,b,c],[b,d,e],[c,e,f]] the characteristic
     * polynomial is: λ³ - p·λ² + q·λ - r = 0 where:
     *   p = trace(A)
     *   q = (a·d + d·f + f·a) - (b² + c² + e²)    [sum of 2×2 principal minors]
     *   r = det(A)
     *
     * The depressed cubic is: x³ - αx - β = 0, where:
     *   α = p²/3 - q
     *   β = -(2p³ - 9pq + 27r) / 27
     *
     * Solutions via trigonometric method (guaranteed 3 real roots for
     * symmetric matrices). */
    const float *d = m->data;
    float a = d[0], b = d[1], c = d[2];
    float             dd = d[4], e = d[5];
    float                          f = d[8];

    float p1 = b*b + c*c + e*e;          /* sum of off-diagonal squares */

    if (p1 == 0.0f) {
        /* Diagonal matrix — eigenvalues are the diagonal entries */
        vals[0] = a; vals[1] = dd; vals[2] = f;
        /* sort descending */
        if (vals[0] < vals[1]) { float t = vals[0]; vals[0] = vals[1]; vals[1] = t; }
        if (vals[1] < vals[2]) { float t = vals[1]; vals[1] = vals[2]; vals[2] = t; }
        if (vals[0] < vals[1]) { float t = vals[0]; vals[0] = vals[1]; vals[1] = t; }
        return;
    }

    float q_val = (a + dd + f) / 3.0f;           /* mean of diagonal */
    float p2 = (a - q_val)*(a - q_val) + (dd - q_val)*(dd - q_val) + (f - q_val)*(f - q_val)
             + 2.0f * p1;
    p2 = esqrt(p2 / 6.0f);

    /* B = (1/p2) · (A - q·I)  →  det(B) */
    float b00 = (a - q_val) / p2, b01 = b / p2, b02 = c / p2;
    float b11 = (dd - q_val) / p2, b12 = e / p2;
    float b22 = (f - q_val) / p2;

    float detB = b00 * (b11 * b22 - b12 * b12)
               - b01 * (b01 * b22 - b12 * b02)
               + b02 * (b01 * b12 - b11 * b02);
    float r_val = detB / 2.0f;

    /* Clamp to [-1, 1] for acos domain safety */
    if (r_val > 1.0f)  r_val = 1.0f;
    if (r_val < -1.0f) r_val = -1.0f;

    float phi = eacos(r_val) / 3.0f;

    /* Three eigenvalues via trigonometric method */
    vals[0] = q_val + 2.0f * p2 * ecos(phi);
    vals[1] = q_val + 2.0f * p2 * ecos(phi + 2.0f * E_PI / 3.0f);
    vals[2] = q_val + 2.0f * p2 * ecos(phi + 4.0f * E_PI / 3.0f);

    /* Sort descending (simple sort for 3 elements) */
    if (vals[0] < vals[1]) { float t = vals[0]; vals[0] = vals[1]; vals[1] = t; }
    if (vals[1] < vals[2]) { float t = vals[1]; vals[1] = vals[2]; vals[2] = t; }
    if (vals[0] < vals[1]) { float t = vals[0]; vals[0] = vals[1]; vals[1] = t; }
}

void emat_exp(const ematrix *A, ematrix *out) {
    /* Matrix exponential via Taylor series with scaling-and-squaring.
     *
     * e^A = I + A + A²/2! + A³/3! + ...
     *
     * Scaling: find s such that ||A/2^s|| < 0.5, compute e^(A/2^s),
     * then square the result s times.
     *
     * Uses VLAs for temporary matrix storage consistent with the
     * rest of the library (edet_nxn, emat_solve). */
    int n = A->rows;

    /* Determine scaling factor */
    float norm = emat_frobenius(A);
    int s = 0;
    if (norm > 0.5f) {
        /* s = ceil(log2(norm / 0.5)) */
        float ratio = norm / 0.5f;
        while (ratio > 1.0f) {
            ratio *= 0.5f;
            s++;
        }
    }

    /* Scaled matrix: As = A / 2^s */
    float scale = 1.0f;
    for (int i = 0; i < s; i++)
        scale *= 2.0f;
    float inv_scale = 1.0f / scale;

    float asd[n * n];   /* scaled A */
    ematrix As = emat_make(asd, n, n);
    emat_scale(A, inv_scale, &As);

    /* Taylor series: sum = I + As + As²/2! + ... (10 terms) */
    float termd[n * n];   /* current term (As^k / k!) */
    ematrix Term = emat_make(termd, n, n);
    float tmpd[n * n];
    ematrix Tmp = emat_make(tmpd, n, n);

    /* out = I (result accumulator) */
    emat_identity(out);
    /* Term = As (k=1 term) */
    for (int i = 0; i < n * n; i++)
        Term.data[i] = As.data[i];
    /* Add Term to result */
    emat_add(out, &Term, out);

    for (int k = 2; k <= 10; k++) {
        /* Term = Term * As / k  →  As^k / k! */
        emat_mul(&Term, &As, &Tmp);
        float inv_k = 1.0f / (float)k;
        emat_scale(&Tmp, inv_k, &Term);
        emat_add(out, &Term, out);
    }

    /* Squaring phase: square result s times */
    float sqd[n * n];
    ematrix Sq = emat_make(sqd, n, n);
    for (int i = 0; i < s; i++) {
        emat_mul(out, out, &Sq);
        for (int j = 0; j < n * n; j++)
            out->data[j] = Sq.data[j];
    }
}

/* ====================================================== *
 *  Scalar math — exp, ln, pow, clamp, lerp, etc.         *
 * ====================================================== */

float eabs(float x) {
    return (x < 0.0f) ? -x : x;
}

float esign(float x) {
    if (x > 0.0f) return  1.0f;
    if (x < 0.0f) return -1.0f;
    return 0.0f;
}

float emod(float a, float b) {
    /* Floating-point modulo: result has same sign as a.
     * Equivalent to C fmodf(). Returns 0 if b is 0. */
    if (b == 0.0f) return 0.0f;
    return a - (float)((int)(a / b)) * b;
}

float esqrt(float x) {
    /* Babylonian method with bit-hack initial estimate.
     * The initial guess uses the IEEE-754 exponent to get within
     * a factor of 2 of the true result, so 6 iterations suffice
     * for full float precision on the entire domain. */
    if (x < 0.0f) return 0.0f;
    if (x == 0.0f) return 0.0f;

    /* Initial estimate via IEEE-754 bit manipulation.
     * For float, halving the exponent ≈ sqrt. Adjust the mantissa
     * with the magic constant to improve the guess. */
    union { float f; int i; } u;
    u.f = x;
    u.i = (u.i >> 1) + 0x1fbd1df5;   /* magic constant for log2(1+√2)/2 */

    float r = u.f;
    for (int i = 0; i < 6; i++)
        r = 0.5f * (r + x / r);
    return r;
}

float eexp(float x) {
    /* Taylor series for e^x with range reduction via:
     * e^x = e^n · e^r  where x = n·ln2 + r, |r| < ln2/2
     * e^n = (2)^n by squaring. */
    if (x < -88.0f) return 0.0f;   /* underflow */
    if (x >  88.0f) return 1e38f;  /* overflow guard */
    /* Split x = k * ln(2) + r */
    float ln2 = 0.6931471805599453f;
    int k = (int)(x / ln2);
    float r = x - (float)k * ln2;
    /* Taylor for e^r where |r| < ln2/2 ~ 0.347 */
    float term = 1.0f;
    float sum  = term;
    term *= r / 1.0f; sum += term;
    term *= r / 2.0f; sum += term;
    term *= r / 3.0f; sum += term;
    term *= r / 4.0f; sum += term;
    term *= r / 5.0f; sum += term;
    term *= r / 6.0f; sum += term;
    term *= r / 7.0f; sum += term;
    /* Multiply by 2^k */
    float pow2k = 1.0f;
    if (k >= 0) {
        for (int i = 0; i < k; i++) pow2k *= 2.0f;
    } else {
        for (int i = 0; i < -k; i++) pow2k *= 0.5f;
    }
    return sum * pow2k;
}

float eln(float x) {
    /* Natural log via Newton-Raphson.
     * Solve e^y − x = 0  →  y_{n+1} = y_n + (x · e^{−y_n} − 1)
     * Range-reduce x first: factor out powers of 2 so x ∈ [1, 2). */
    if (x <= 0.0f) return -1e38f;   /* domain error */
    /* Find k such that x / 2^k ∈ [1, 2) */
    int k = 0;
    float m = x;
    while (m >= 2.0f) { m *= 0.5f; k++; }
    while (m <  1.0f) { m *= 2.0f; k--; }
    /* Now x = m * 2^k,  m ∈ [1,2).
     * ln(x) = k*ln(2) + ln(m).  Solve ln(m) with Newton on e^y − m = 0. */
    float y = 0.0f;
    for (int i = 0; i < 15; i++)
        y = y + m * eexp(-y) - 1.0f;
    return y + (float)k * 0.6931471805599453f;
}

float epow(float base, float exponent) {
    if (base == 0.0f) return 0.0f;
    return eexp(exponent * eln(base));
}

float eclamp(float x, float lo, float hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

float elerp(float a, float b, float t) {
    return a + t * (b - a);
}

float easin(float x) {
    if (x <= -1.0f) return -E_PI_2;
    if (x >=  1.0f) return  E_PI_2;
    float y = x;
    for (int i = 0; i < 8; i++) {
        float s = esin(y);
        float c = ecos(y);
        y = y - (s - x) / c;
    }
    return y;
}

float eacos(float x) {
    return E_PI_2 - easin(x);
}

float eatan(float x) {
    if (x > 1.0f)  return  E_PI_2 - eatan(1.0f / x);
    if (x < -1.0f) return -E_PI_2 - eatan(1.0f / x);
    float x2 = x * x;
    return x * (0.9998660f
              + x2 * (-0.3302995f
              + x2 * ( 0.1801410f
              + x2 * (-0.0851330f
              + x2 * ( 0.0208351f)))));
}

float eatan2(float y, float x) {
    if (x > 0.0f)          return eatan(y / x);
    else if (x < 0.0f && y >= 0.0f)  return eatan(y / x) + E_PI;
    else if (x < 0.0f && y <  0.0f)  return eatan(y / x) - E_PI;
    else if (y > 0.0f)     return  E_PI_2;
    else if (y < 0.0f)     return -E_PI_2;
    else                   return 0.0f;
}

float emap(float x, float in_min, float in_max,
           float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float esigmoid(float x) {
    /* σ(x) = 1 / (1 + e^{-x})
     * For very negative x, e^{-x} overflows; clamp to 0.0.
     * For very positive x, e^{-x} → 0, σ → 1.0. */
    if (x >  88.0f) return 1.0f;       /* overflow guard */
    if (x < -88.0f) return 0.0f;       /* e^{-x} overflow */
    return 1.0f / (1.0f + eexp(-x));
}

float etanh(float x) {
    /* tanh(x) = (e^x − e^{−x}) / (e^x + e^{−x})
     * For |x| > ~10, tanh saturates at ±1 — clamp directly. */
    if (x >  10.0f) return  1.0f;
    if (x < -10.0f) return -1.0f;
    float ep = eexp(x);
    float em = eexp(-x);
    return (ep - em) / (ep + em);
}

float efloor(float x) {
    /* floor(x) = largest integer ≤ x.
     * Implementation uses the bit pattern for speed on hardware FPU. */
    float r = (float)((int)x);
    if (x < 0.0f && x != r)
        r -= 1.0f;
    return r;
}

float eceil(float x) {
    float r = (float)((int)x);
    if (x > 0.0f && x != r)
        r += 1.0f;
    return r;
}

float eround(float x) {
    /* Round to nearest integer (ties go away from zero). */
    if (x >= 0.0f)
        return (float)((int)(x + 0.5f));
    else
        return (float)((int)(x - 0.5f));
}

float ebilerp(float v00, float v10, float v01, float v11,
              float fx, float fy) {
    /* Bilinear interpolation:
     *   top    = lerp(v00, v10, fx)
     *   bottom = lerp(v01, v11, fx)
     *   result = lerp(top, bottom, fy) */
    float top    = elerp(v00, v10, fx);
    float bottom = elerp(v01, v11, fx);
    return elerp(top, bottom, fy);
}

/* ====================================================== *
 *  Angle conversion                                       *
 * ====================================================== */

float edeg2rad(float deg) {
    return deg * (E_PI / 180.0f);
}

float erad2deg(float rad) {
    return rad * (180.0f / E_PI);
}

/* ====================================================== *
 *  Trigonometry — Maclaurin series with range reduction   *
 * ====================================================== */

float esin(float x) {
    x = reduce_angle(x);
    float x2   = x * x;
    float term = x;
    float sum  = term;
    term *= -x2 / ( 2.0f *  3.0f); sum += term;
    term *= -x2 / ( 4.0f *  5.0f); sum += term;
    term *= -x2 / ( 6.0f *  7.0f); sum += term;
    term *= -x2 / ( 8.0f *  9.0f); sum += term;
    term *= -x2 / (10.0f * 11.0f); sum += term;
    term *= -x2 / (12.0f * 13.0f); sum += term;
    return sum;
}

float ecos(float x) {
    return esin(x + E_PI_2);
}

float etan(float x) {
    return esin(x) / ecos(x);
}

/* ====================================================== *
 *  Quaternions (Hamilton convention: w + xi + yj + zk)   *
 * ====================================================== */

equat equat_from_axis_angle(const evector *axis, float angle) {
    /* q = [cos(θ/2), sin(θ/2) * axis] */
    float half = angle * 0.5f;
    float s = esin(half);
    equat q;
    q.w = ecos(half);
    q.x = s * axis->data[0];
    q.y = s * axis->data[1];
    q.z = s * axis->data[2];
    return q;
}

equat equat_mul(const equat *a, const equat *b) {
    /* Hamilton product:
     * (a_w·b_w − a·b,  a_w·b + b_w·a + a×b) */
    equat r;
    r.w = a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z;
    r.x = a->w * b->x + a->x * b->w + a->y * b->z - a->z * b->y;
    r.y = a->w * b->y - a->x * b->z + a->y * b->w + a->z * b->x;
    r.z = a->w * b->z + a->x * b->y - a->y * b->x + a->z * b->w;
    return r;
}

equat equat_normalize(const equat *q) {
    float mag = esqrt(q->w * q->w + q->x * q->x + q->y * q->y + q->z * q->z);
    equat r;
    if (mag > 0.0f) {
        float inv = 1.0f / mag;
        r.w = q->w * inv;
        r.x = q->x * inv;
        r.y = q->y * inv;
        r.z = q->z * inv;
    } else {
        r.w = 1.0f; r.x = r.y = r.z = 0.0f;
    }
    return r;
}

equat equat_conjugate(const equat *q) {
    equat r;
    r.w =  q->w;
    r.x = -q->x;
    r.y = -q->y;
    r.z = -q->z;
    return r;
}

void equat_to_matrix(const equat *q, ematrix *out) {
    /* Convert unit quaternion to 3x3 rotation matrix.
     * Assumes q is already normalized for optimal accuracy. */
    float ww = q->w * q->w;
    float xx = q->x * q->x;
    float yy = q->y * q->y;
    float zz = q->z * q->z;
    float wx = q->w * q->x;
    float wy = q->w * q->y;
    float wz = q->w * q->z;
    float xy = q->x * q->y;
    float xz = q->x * q->z;
    float yz = q->y * q->z;

    out->data[0] = ww + xx - yy - zz;
    out->data[1] = 2.0f * (xy - wz);
    out->data[2] = 2.0f * (xz + wy);

    out->data[3] = 2.0f * (xy + wz);
    out->data[4] = ww - xx + yy - zz;
    out->data[5] = 2.0f * (yz - wx);

    out->data[6] = 2.0f * (xz - wy);
    out->data[7] = 2.0f * (yz + wx);
    out->data[8] = ww - xx - yy + zz;
}

void equat_rotate_vec(const equat *q, const evector *v, evector *out) {
    /* Rotate v by q:  v' = q ⊗ [0,v] ⊗ q*
     * Optimized formula avoids full quaternion multiply:
     * v' = v + 2*w*(u × v) + 2*(u × (u × v))
     * where u = (x,y,z) is the vector part of q. */
    float ux = q->x, uy = q->y, uz = q->z;
    float vx = v->data[0], vy = v->data[1], vz = v->data[2];

    /* t = 2 * (u × v) */
    float tx = 2.0f * (uy * vz - uz * vy);
    float ty = 2.0f * (uz * vx - ux * vz);
    float tz = 2.0f * (ux * vy - uy * vx);

    /* v' = v + w*t + (u × t) */
    out->data[0] = vx + q->w * tx + (uy * tz - uz * ty);
    out->data[1] = vy + q->w * ty + (uz * tx - ux * tz);
    out->data[2] = vz + q->w * tz + (ux * ty - uy * tx);
}

equat equat_slerp(const equat *a, const equat *b, float t) {
    /* Spherical linear interpolation between two unit quaternions.
     * Finds the shortest arc on the 4D unit sphere. */
    float dot = a->w*b->w + a->x*b->x + a->y*b->y + a->z*b->z;

    /* If dot < 0, negate one quaternion to take the shortest path */
    equat b_adj = *b;
    if (dot < 0.0f) {
        b_adj.w = -b_adj.w;
        b_adj.x = -b_adj.x;
        b_adj.y = -b_adj.y;
        b_adj.z = -b_adj.z;
        dot = -dot;
    }

    /* If very close, use linear interpolation to avoid div-by-zero */
    if (dot > 0.9995f) {
        equat r;
        r.w = elerp(a->w, b_adj.w, t);
        r.x = elerp(a->x, b_adj.x, t);
        r.y = elerp(a->y, b_adj.y, t);
        r.z = elerp(a->z, b_adj.z, t);
        return equat_normalize(&r);
    }

    float theta_0   = eacos(dot);
    float sin_theta_0 = esin(theta_0);
    float theta     = theta_0 * t;
    float sin_theta = esin(theta);
    float s0 = ecos(theta) - dot * sin_theta / sin_theta_0;
    float s1 = sin_theta / sin_theta_0;

    equat r;
    r.w = s0 * a->w + s1 * b_adj.w;
    r.x = s0 * a->x + s1 * b_adj.x;
    r.y = s0 * a->y + s1 * b_adj.y;
    r.z = s0 * a->z + s1 * b_adj.z;
    return r;
}

/* ====================================================== *
 *  Euler angles (XYZ intrinsic / Tait-Bryan)             *
 * ====================================================== */

void euler_from_quat(const equat *q, evector *out) {
    /* Extract roll (φ), pitch (θ), yaw (ψ) from a quaternion.
     * Uses the standard XYZ intrinsic rotation sequence:
     *   R = Rz(ψ) · Ry(θ) · Rx(φ)
     * The formulas are derived from the rotation matrix built from q. */

    /* Sin of 2·pitch: sp = 2(w·y − x·z).
     * Clamp to [−1, 1] to avoid NaN from rounding near ±90° (gimbal lock). */
    float sp = 2.0f * (q->w * q->y - q->x * q->z);

    if (sp >= 1.0f) {
        /* Gimbal lock: pitch = +90° */
        out->data[0] = 2.0f * eatan2(q->x, q->w);            /* roll  */
        out->data[1] = E_PI_2;                               /* pitch */
        out->data[2] = 0.0f;                                 /* yaw   */
        return;
    }
    if (sp <= -1.0f) {
        /* Gimbal lock: pitch = −90° */
        out->data[0] = -2.0f * eatan2(q->x, q->w);
        out->data[1] = -E_PI_2;
        out->data[2] = 0.0f;
        return;
    }

    out->data[0] = eatan2(2.0f * (q->w * q->x + q->y * q->z),
                          1.0f - 2.0f * (q->x * q->x + q->y * q->y));
    out->data[1] = easin(sp);
    out->data[2] = eatan2(2.0f * (q->w * q->z + q->x * q->y),
                          1.0f - 2.0f * (q->y * q->y + q->z * q->z));
}

void euler_from_matrix(const ematrix *m, evector *out) {
    /* Extract roll (φ), pitch (θ), yaw (ψ) from a 3×3 rotation matrix R.
     * Convention: R = Rz(yaw) · Ry(pitch) · Rx(roll), row-major.
     *
     * Key entries (row-major: data[r*3 + c]):
     *   R[2][0] = d[6] = −sin(pitch)
     *   R[2][1] = d[7] =  cos(pitch)·sin(roll)
     *   R[2][2] = d[8] =  cos(pitch)·cos(roll)
     *   R[1][0] = d[3] =  sin(yaw)·cos(pitch)
     *   R[0][0] = d[0] =  cos(yaw)·cos(pitch)   */
    const float *d = m->data;

    float sp = -d[6];   /* sin(pitch) */

    if (sp >= 1.0f) {
        /* Gimbal lock: pitch = +90° — set yaw = 0, solve roll */
        out->data[0] = eatan2(d[1], d[4]);
        out->data[1] = E_PI_2;
        out->data[2] = 0.0f;
        return;
    }
    if (sp <= -1.0f) {
        /* Gimbal lock: pitch = −90° — set yaw = 0, solve roll */
        out->data[0] = eatan2(-d[1], d[4]);
        out->data[1] = -E_PI_2;
        out->data[2] = 0.0f;
        return;
    }

    out->data[0] = eatan2(d[7], d[8]);   /* roll  */
    out->data[1] = easin(sp);            /* pitch */
    out->data[2] = eatan2(d[3], d[0]);   /* yaw   */
}
