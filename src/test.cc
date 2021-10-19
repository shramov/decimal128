#include <gtest/gtest.h>

#include <cstring>
#include <cmath>

#include "decimal128.h"

#if defined(__GLIBCXX__) && !defined(__clang__)
#include <decimal/decimal>

#define CHECK_STD128(dec, bin) do { \
		std::decimal::decimal128 std(dec), copy; \
		memcpy((void *) &copy, &bin, sizeof(copy)); \
		ASSERT_EQ(std, copy); \
	} while (0)
#else
#define CHECK_STD128(dec, bin)
#endif

#define CHECK_D128(binary, stddecimal, sig, m, e) do { \
		d128_unpacked_t u = {}; \
		u.sign = sig; \
		u.mantissa.value = (m); \
		u.exponent = e; \
		d128_t dec = {}; \
		ASSERT_EQ(d128_pack(&dec, &u), 0); \
		d128_t bindec = {}; \
		memcpy(&bindec, binary, sizeof(bindec)); \
		ASSERT_EQ(dec.lo, bindec.lo); \
		ASSERT_EQ(dec.hi, bindec.hi); \
		if (e != D128_EXP_NAN && e != D128_EXP_SNAN) \
			CHECK_STD128(stddecimal, dec); \
		u = {}; \
		ASSERT_EQ(d128_unpack(&u, &dec), 0); \
		ASSERT_EQ(u.sign, sig); \
		ASSERT_EQ(u.mantissa.value, (m)); \
		EXPECT_EQ(u.exponent, e); \
	} while (0)

constexpr __uint128_t u128_build_18(uint64_t large, uint64_t small)
{
	constexpr uint64_t exp18 = 1000ull * 1000 * 1000 * 1000 * 1000 * 1000; // 10 ^ 18
	__uint128_t r = large;
	return r * exp18 + small;
}

TEST(Decimal128, Generated)
{
#include "generated.h"
}

int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
