/*
 * Public domain C implementation of IEEE 754 decimal128
 * Pavel Shramov <shramov@mexmat.net>, 2021
 */

#ifndef _DECIMAL128_H
#define _DECIMAL128_H

#ifdef __cplusplus
extern "C" {
#endif

static const short D128_EXP_MAX = 6111;
static const short D128_EXP_MIN = -6176;

static const short D128_EXP_INF = 10000;
static const short D128_EXP_NAN = 10001;
static const short D128_EXP_SNAN = 10002;

static const uint64_t D128_HISIG_MAX = 0x1ed09bead87c0ull; // (10 ** 34) >> 64

static const uint32_t D128_HIGH_MASK = 0xc0u << 6;

static const uint32_t D128_INF = 0xf0u << 6;
static const uint32_t D128_INF_MASK = 0xf8u << 6;

static const uint32_t D128_NAN = 0xf8u << 6;
static const uint32_t D128_NAN_MASK = 0xfcu << 6;

static const uint32_t D128_SNAN = 0xfcu << 6;
static const uint32_t D128_SNAN_MASK = 0xfcu << 6;

#pragma pack(push, 1)

typedef struct
{
	uint64_t lo;
	union {
		uint64_t hi;
		struct {
			uint64_t hisig:49;
			uint64_t combination:14;
			uint64_t sign:1;
		};
	};
} d128_t;

#pragma pack(pop)

typedef struct
{
	int sign;
	int exponent;
	union {
		struct {
			uint64_t lo;
			uint64_t hi;
		};
		__uint128_t value;
	} mantissa;
} d128_unpacked_t;

static inline int d128_pack(d128_t * d, const d128_unpacked_t * u)
{
	if (u->exponent > D128_EXP_MAX) {
		d->hi = d->lo = 0;
		switch (u->exponent) {
		case D128_EXP_INF:
			d->combination = D128_INF;
			d->sign = u->sign;
			return 0;
		case D128_EXP_NAN:
			d->combination = D128_NAN;
			return 0;
		case D128_EXP_SNAN:
			d->combination = D128_SNAN;
			return 0;
		default:
			return ERANGE;
		}
	} else if (u->exponent < D128_EXP_MIN)
		return ERANGE;
	if (u->mantissa.hi > D128_HISIG_MAX)
		return ERANGE;
	d->sign = u->sign;
	d->combination = u->exponent - D128_EXP_MIN;
	d->hisig = u->mantissa.hi;
	d->lo = u->mantissa.lo;
	return 0;
}

static inline int d128_unpack(d128_unpacked_t * u, const d128_t * d)
{
	u->sign = d->sign;
	u->exponent = 0;
	u->mantissa.value = 0;
	if ((d->combination & D128_HIGH_MASK) == D128_HIGH_MASK) {
		if ((d->combination & D128_INF_MASK) == D128_INF) {
			u->exponent = D128_EXP_INF;
		} else if ((d->combination & D128_NAN_MASK) == D128_NAN) {
			u->exponent = D128_EXP_NAN;
		} else if ((d->combination & D128_SNAN_MASK) == D128_SNAN) {
			u->exponent = D128_EXP_SNAN;
		} else {
			/*
			 * The "11" 2-bit sequence after the sign bit indicates that there is an implicit "100" 3-bit prefix to the significand.
			 * Treat as overflow
			 */
		}
		return 0;
	}
	u->exponent = d->combination + D128_EXP_MIN;
	u->mantissa.hi = d->hisig;
	u->mantissa.lo = d->lo;
	return 0;
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif//_DECIMAL128_H
