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

START_TEST(test_protobuf_small)
{
	hycon_tx tx = HYCON_TX_INIT_ZERO;
	ck_assert_int_eq(decode_tx(ENCODED_TXS[0], ENCODED_TXS_LEN[0], &tx), 1);
	ck_assert_int_eq(tx.amount, 10000);
	ck_assert_int_eq(tx.fee, 100);
}
END_TEST

START_TEST(test_protobuf_medium)
{
	hycon_tx tx = HYCON_TX_INIT_ZERO;
	ck_assert_int_eq(decode_tx(ENCODED_TXS[1], ENCODED_TXS_LEN[1], &tx), 1);
	ck_assert_int_eq(tx.amount, 10);
	ck_assert_int_eq(tx.fee, 1);
}
END_TEST

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

START_TEST(test_decode_varint)
{
	uint8_t skip_bytes;
	uint64_t decoded_amount;

	uint8_t encoded_amount_small[] = {0x90, 0x4e};
	decoded_amount = decode_varint(encoded_amount_small, &skip_bytes);
	ck_assert_int_eq(decoded_amount, 10000);

	uint8_t encoded_amount_medium[] = {0x95, 0xbe, 0xc3, 0x8b, 0xa8, 0xee, 0x67};
	decoded_amount = decode_varint(encoded_amount_medium, &skip_bytes);
	ck_assert_int_eq(decoded_amount, 456789123456789);
}
END_TEST

/*START_TEST(test_address_limit)
{
	uint8_t ENCODED_TXS_LEN[] =
		{30, 50};
	uint8_t ENCODED_TXS[][50] =
		{{0x12, 0x14, 0xdb, 0x66, 0x12, 0xb3, 0x09, 0xd2, 0x57, 0xa2,
		  0xae, 0xbd, 0x59, 0xb7, 0x44, 0x5a, 0x90, 0x0e, 0xd7, 0x75,
		  0xd9, 0x21, 0x18, 0x90, 0x4e, 0x20, 0x64, 0x28, 0xd2, 0x09},
		 {0x0a, 0x14, 0x62, 0x20, 0xef, 0xa7, 0xb2, 0xbd, 0x85, 0x50,
		  0xc3, 0x3d, 0xa0, 0x79, 0x6c, 0x77, 0x91, 0x70, 0xd3, 0xfb,
		  0x9d, 0xc2, 0x12, 0x14, 0xe1, 0x61, 0x12, 0x4d, 0x4a, 0xa4,
		  0x1c, 0xa0, 0xdf, 0x6b, 0xaf, 0xec, 0xb0, 0x40, 0x89, 0x71,
		  0xcf, 0xf6, 0xc0, 0x96, 0x18, 0x0a, 0x20, 0x01, 0x28, 0x00}};

	hycon_tx tx = HYCON_TX_INIT_ZERO;
	ck_assert_int_eq(decode_tx(INVALID_ADDR_LEN_ENCODED_TX, ENCODED_TXS_LEN[1], &tx), 0);
}
END_TEST*/

Suite *hash_suite(void)
{
	Suite *s;
	TCase *tc;

	s = suite_create("Protobuf");

	tc = tcase_create("Test case");

	tcase_add_checked_fixture(tc, setup, teardown);
	tcase_add_test(tc, test_decode_varint);
	tcase_add_test(tc, test_protobuf_small);
	tcase_add_test(tc, test_protobuf_medium);
	tcase_add_test(tc, test_displayable_amount_conversion);
	suite_add_tcase(s, tc);

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
