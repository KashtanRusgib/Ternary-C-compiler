#ifndef TERNARY_H
#define TERNARY_H

typedef signed char trit;  // -1 (N), 0 (Z), 1 (P)

#define TRIT_N (-1)
#define TRIT_Z 0
#define TRIT_P 1

// Ternary addition (with carry)
static inline trit trit_add(trit a, trit b, trit *carry) {
    int sum = (int)a + (int)b + (int)*carry;
    if (sum > 1) {
        *carry = TRIT_P;
        return (trit)(sum - 3);
    } else if (sum < -1) {
        *carry = TRIT_N;
        return (trit)(sum + 3);
    } else {
        *carry = TRIT_Z;
        return (trit)sum;
    }
}

// Similar for mul, min, max (from ternary logic gates)
static inline trit trit_mul(trit a, trit b) {
    return (trit)((int)a * (int)b);
}

static inline trit trit_min(trit a, trit b) {
    return (a < b) ? a : b;
}

static inline trit trit_max(trit a, trit b) {
    return (a > b) ? a : b;
}

/* ---- Multi-trit word operations (TASK-002) ---- */

#define WORD_SIZE 9
typedef trit trit_word[WORD_SIZE];

/* Add two trit words (LSB at [0]), result in res */
static inline void trit_word_add(const trit *a, const trit *b, trit *res) {
    trit carry = TRIT_Z;
    int i;
    for (i = 0; i < WORD_SIZE; i++) {
        int sum = (int)a[i] + (int)b[i] + (int)carry;
        if (sum > 1) {
            carry = TRIT_P;
            res[i] = (trit)(sum - 3);
        } else if (sum < -1) {
            carry = TRIT_N;
            res[i] = (trit)(sum + 3);
        } else {
            carry = TRIT_Z;
            res[i] = (trit)sum;
        }
    }
    /* Overflow if carry != 0 — silently truncated to WORD_SIZE */
}

/* Multiply two trit words (schoolbook algorithm), result in res */
static inline void trit_word_mul(const trit *a, const trit *b, trit *res) {
    trit partial[WORD_SIZE];
    trit sum_buf[WORD_SIZE];
    int i, j;

    for (i = 0; i < WORD_SIZE; i++) res[i] = TRIT_Z;

    for (i = 0; i < WORD_SIZE; i++) {
        if (b[i] == TRIT_Z) continue;

        /* Build shifted partial product: a * b[i] << i */
        for (j = 0; j < WORD_SIZE; j++) partial[j] = TRIT_Z;
        for (j = 0; j + i < WORD_SIZE; j++) {
            partial[j + i] = trit_mul(a[j], b[i]);
        }

        /* Accumulate: res += partial */
        trit_word_add(res, partial, sum_buf);
        for (j = 0; j < WORD_SIZE; j++) res[j] = sum_buf[j];
    }
}

/* Convert trit word to integer */
static inline int trit_word_to_int(const trit *w) {
    int result = 0;
    int place = 1;
    int i;
    for (i = 0; i < WORD_SIZE; i++) {
        result += (int)w[i] * place;
        place *= 3;
    }
    return result;
}

/* Convert integer to trit word (balanced ternary, LSB at [0]) */
static inline void int_to_trit_word(int val, trit *w) {
    int i;
    for (i = 0; i < WORD_SIZE; i++) w[i] = TRIT_Z;

    int neg = (val < 0);
    if (neg) val = -val;

    for (i = 0; i < WORD_SIZE && val > 0; i++) {
        int rem = val % 3;
        val /= 3;
        if (rem == 0) {
            w[i] = TRIT_Z;
        } else if (rem == 1) {
            w[i] = TRIT_P;
        } else {
            /* rem == 2: represent as -1 + carry */
            w[i] = TRIT_N;
            val++;
        }
    }

    if (neg) {
        for (i = 0; i < WORD_SIZE; i++) w[i] = (trit)(-(int)w[i]);
    }
}

#endif
