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
	const uint8_t const encoded_tx[] =
		{0x12, 0x14, 0xdb, 0x66, 0x12, 0xb3, 0x09, 0xd2, 0x57, 0xa2,
		 0xae, 0xbd, 0x59, 0xb7, 0x44, 0x5a, 0x90, 0x0e, 0xd7, 0x75,
		 0xd9, 0x21, 0x18, 0x90, 0x4e, 0x20, 0x64, 0x28, 0xd2, 0x09};

	hycon_tx tx = {0};
	ck_assert_int_eq(decode_tx(encoded_tx, sizeof(encoded_tx), &tx), 1);
	ck_assert_int_eq(tx.amount, 10000);
	ck_assert_int_eq(tx.fee, 100);
}
END_TEST

START_TEST(test_protobuf_medium)
{
	const uint8_t const encoded_tx[] =
	{0x0a, 0x14, 0x62, 0x20, 0xef, 0xa7, 0xb2, 0xbd, 0x85, 0x50,
	 0xc3, 0x3d, 0xa0, 0x79, 0x6c, 0x77, 0x91, 0x70, 0xd3, 0xfb,
	 0x9d, 0xc2, 0x12, 0x14, 0xe1, 0x61, 0x12, 0x4d, 0x4a, 0xa4,
	 0x1c, 0xa0, 0xdf, 0x6b, 0xaf, 0xec, 0xb0, 0x40, 0x89, 0x71,
	 0xcf, 0xf6, 0xc0, 0x96, 0x18, 0x0a, 0x20, 0x01, 0x28, 0x00};

	hycon_tx tx = {0};
	ck_assert_int_eq(decode_tx(encoded_tx, sizeof(encoded_tx), &tx), 1);
	ck_assert_int_eq(tx.amount, 10);
	ck_assert_int_eq(tx.fee, 1);
}
END_TEST

START_TEST(test_protobuf_large)
{
	const uint8_t const encoded_tx[] =
	{0x0a, 0x14, 0xe7, 0xbc, 0x78, 0x74, 0xee, 0xbe, 0x21, 0xbb, 0x46, 0x08,
	 0xf4, 0xb6, 0xb5, 0xd5, 0x2e, 0x29, 0xab, 0x8f, 0xd6, 0x01, 0x12, 0x14,
	 0xe1, 0x61, 0x12, 0x4d, 0x4a, 0xa4, 0x1c, 0xa0, 0xdf, 0x6b, 0xaf, 0xec,
	 0xb0, 0x40, 0x89, 0x71, 0xcf, 0xf6, 0xc0, 0x96, 0x18, 0x95, 0xbe, 0xc1,
	 0xe6, 0xba, 0xe9, 0xa6, 0xdb, 0x01, 0x20, 0x95, 0xbe, 0xc1, 0xe6, 0xba,
	 0xe9, 0xa6, 0xdb, 0x01, 0x28, 0x95, 0x9a, 0xef, 0x3a};

	hycon_tx tx = {0};
	ck_assert_int_eq(decode_tx(encoded_tx, sizeof(encoded_tx), &tx), 1);
	ck_assert_int_eq(tx.amount, 123456789123456789);
	ck_assert_int_eq(tx.fee, 123456789123456789);
}
END_TEST

START_TEST(test_decode_varint_small)
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

START_TEST(test_decode_varint_medium)
{
	uint8_t skip_bytes;
	uint64_t decoded_amount;

	uint8_t encoded_amount_medium[] = {0x95, 0xbe, 0xc3, 0x8b, 0xa8, 0xee, 0x67};
	decoded_amount = decode_varint(encoded_amount_medium, &skip_bytes);
	ck_assert_int_eq(decoded_amount, 456789123456789);
}
END_TEST

START_TEST(test_decode_varint_large)
{
	uint8_t skip_bytes;
	uint64_t decoded_amount;

	uint8_t encoded_amount_medium[] = {0x95, 0xbe, 0xc1, 0xe6, 0xba,
	                                   0xe9, 0xa6, 0xdb, 0x01};
	decoded_amount = decode_varint(encoded_amount_medium, &skip_bytes);
	ck_assert_int_eq(decoded_amount, 123456789123456789);
}
END_TEST

Suite *hash_suite(void)
{
	Suite *s;
	TCase *tc_1, *tc_2;

	s = suite_create("Protobuf");

	tc_1 = tcase_create("Encoded Tx");
	tcase_add_checked_fixture(tc_1, setup, teardown);
	tcase_add_test(tc_1, test_protobuf_small);
	tcase_add_test(tc_1, test_protobuf_medium);
	tcase_add_test(tc_1, test_protobuf_large);

	tc_2 = tcase_create("Decode varint");
	tcase_add_checked_fixture(tc_2, setup, teardown);
	tcase_add_test(tc_2, test_decode_varint_small);
	tcase_add_test(tc_2, test_decode_varint_medium);
	tcase_add_test(tc_2, test_decode_varint_large);

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
