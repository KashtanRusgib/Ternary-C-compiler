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

#endif
