#include <check.h>
#include <math.h>

#include "../src/hycon_api.h"
#include "test_data.h"

void setup(void)
{
}

void teardown(void)
{
}

START_TEST(test_displayable_amount_conversion)
{
	char displayed_amount[22];
	coin_amount_to_displayable_chars((uint64_t)100, displayed_amount);
	ck_assert_str_eq(displayed_amount, "0.0000001");

	coin_amount_to_displayable_chars((uint64_t)0, displayed_amount);
	ck_assert_str_eq(displayed_amount, "0");

	coin_amount_to_displayable_chars((uint64_t)123456789, displayed_amount);
	ck_assert_str_eq(displayed_amount, "0.123456789");

	coin_amount_to_displayable_chars((uint64_t)1123456789, displayed_amount);
	ck_assert_str_eq(displayed_amount, "1.123456789");

	coin_amount_to_displayable_chars((uint64_t)9000012345, displayed_amount);
	ck_assert_str_eq(displayed_amount, "9.000012345");

	coin_amount_to_displayable_chars((uint64_t)3001234000, displayed_amount);
	ck_assert_str_eq(displayed_amount, "3.001234");

	coin_amount_to_displayable_chars((uint64_t)123456789000000000, displayed_amount);
	ck_assert_str_eq(displayed_amount, "123456789");

	coin_amount_to_displayable_chars((uint64_t)18446744073709551615, displayed_amount);
	ck_assert_str_eq(displayed_amount, "18446744073.709551615");

	coin_amount_to_displayable_chars((uint64_t)18446744073000000000, displayed_amount);
	ck_assert_str_eq(displayed_amount, "18446744073");

	coin_amount_to_displayable_chars((uint64_t)123456789123456789, displayed_amount);
	ck_assert_str_eq(displayed_amount, "123456789.123456789");

	coin_amount_to_displayable_chars((uint64_t)123456789100000000, displayed_amount);
	ck_assert_str_eq(displayed_amount, "123456789.1");

	coin_amount_to_displayable_chars((uint64_t)123456789000000000, displayed_amount);
	ck_assert_str_eq(displayed_amount, "123456789");
}
END_TEST

START_TEST(test_hex_to_string_addr_conversion)
{
	char str_address[50];
	const uint8_t const hex_address_1[] =
		{0xe7, 0xbc, 0x78, 0x74, 0xee, 0xbe, 0x21, 0xbb, 0x46, 0x08,
		 0xf4, 0xb6, 0xb5, 0xd5, 0x2e, 0x29, 0xab, 0x8f, 0xd6, 0x01};

	bin_addr_to_hycon_address(hex_address_1, str_address);
	ck_assert_str_eq(str_address, "H4EFbCo6i9xLvJ1ZCQVmnu5sHRJ3i7zrA");

	const uint8_t const hex_address_2[] =
		{0xe1, 0x61, 0x12, 0x4d, 0x4a, 0xa4, 0x1c, 0xa0, 0xdf, 0x6b,
		 0xaf, 0xec, 0xb0, 0x40, 0x89, 0x71, 0xcf, 0xf6, 0xc0, 0x96};
	bin_addr_to_hycon_address(hex_address_2, str_address);
	ck_assert_str_eq(str_address, "H497fHm8gbPZxaXySKpV17a7beYBF9Ut3");
}
END_TEST

Suite *hash_suite(void)
{
	Suite *s;
	TCase *tc_1, *tc_2;

	s = suite_create("Utils");

	tc_1 = tcase_create("Coin amount to displayable");
	tcase_add_checked_fixture(tc_1, setup, teardown);
	tcase_add_test(tc_1, test_displayable_amount_conversion);

	tc_2 = tcase_create("Hex addr to string addr");
	tcase_add_checked_fixture(tc_2, setup, teardown);
	tcase_add_test(tc_2, test_hex_to_string_addr_conversion);

	suite_add_tcase(s, tc_1);
	suite_add_tcase(s, tc_2);

	return s;
}

int main() {
	int number_failed;
	Suite *s;
	SRunner *sr;

	s = hash_suite();
	sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
