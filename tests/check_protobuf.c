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

Suite *hash_suite(void)
{
	Suite *s;
	TCase *tc;

	s = suite_create("Protobuf");

	tc = tcase_create("Test case");

	tcase_add_checked_fixture(tc, setup, teardown);
	tcase_add_test(tc, test_protobuf_small);
	tcase_add_test(tc, test_protobuf_medium);
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
