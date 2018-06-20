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

START_TEST(test_hex_to_hycon_address)
{
	char hycon_address[HYCON_ADDRESS_LENGTH];
	bin_addr_to_hycon_address(HEX_ADDRESSES[0], hycon_address);

	size_t i;
	for (i = 0; i < HYCON_ADDRESS_LENGTH; i++) {
		ck_assert_int_eq(hycon_address[i], HYCON_ADDRESSES[0][i]);
	}
}
END_TEST

Suite *hash_suite(void)
{
	Suite *s;
	TCase *tc;

	s = suite_create("Hash");

	tc = tcase_create("Test case");

	tcase_add_checked_fixture(tc, setup, teardown);
	tcase_add_test(tc, test_hex_to_hycon_address);
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
