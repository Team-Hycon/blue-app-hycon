/******************************************************************************
*   HYCON Wallet for Ledger Nano S
*   (c) 2018 Dulguun Batmunkh
*   (c) 2018 Hycon
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
******************************************************************************/

#include "hycon_api.h"
#include "ram_variables.h"

void get_compressed_public_key_value(unsigned char *value, unsigned char *out) {
	memcpy(out, value, 65);
	out[0] = ((out[64] & 1) ? 0x03 : 0x02);
}

#ifndef UNIT_TEST
void get_address_string_from_key(const cx_ecfp_public_key_t public_key,
                                 uint8_t *out) {
	hycon_hash_t hash_address;

	// Compressed public key
	unsigned char tmp_pub_key[65];
	get_compressed_public_key_value(public_key.W, tmp_pub_key);

	blake2b(hash_address, sizeof(hycon_hash_t), tmp_pub_key, 32, &G_blake2b_state, 0);

	os_memmove(out, hash_address + 12, 20);
}

uint32_t set_result_publicKey(cx_ecfp_public_key_t public_key) {
	uint32_t tx = 0;
	G_io_apdu_buffer[tx++] = 32;

	// Compressed public key
	unsigned char tmp_pub_key[65];
	get_compressed_public_key_value(public_key.W, tmp_pub_key);
	os_memmove(G_io_apdu_buffer + tx, tmp_pub_key, 32);
	tx += 32;

	uint8_t address[21];
	get_address_string_from_key(public_key, address);
	G_io_apdu_buffer[tx++] = 20;
	os_memmove(G_io_apdu_buffer + tx, address, 20);
	tx += 20;

	return tx;
}
#endif	// UNIT_TEST

// Hycon protocol expects the amount in 10^(-9) (xxx.123456789)
// This conversion is only for display purpose and all math operations will be
// done without this conversion.
void coin_amount_to_displayable_chars(uint64_t number, char *out) {
	int zero_count = 0;
	char tmp[22];
	size_t i = 0;
	bool show_decimal_point = false;

	while ((number%10 == 0) && (zero_count < HYC_TO_MINIMUM_AMOUNT)) {
		zero_count++;
		number/=10;
	}

	zero_count = HYC_TO_MINIMUM_AMOUNT - zero_count;
	if (zero_count > 0)
		show_decimal_point = true;

	// handle right of decimal point
	while (zero_count--) {
		tmp[i] = number%10;
		i++;
		number/=10;
	}

	if (show_decimal_point) {
		tmp[i] = '.';
		i++;
	}

	// handle left of decimal point
	do {
		tmp[i] = number%10;
		i++;
	} while (number /= 10);

	// reverse array
	size_t j = 0;
	while (i) {
		i--;
		if (tmp[i] != '.')
			out[j] = tmp[i] + '0';
		else
			out[j] = tmp[i];
		j++;
	}

	out[j] = '\0';
}

bool decode_tx(uint8_t *data, size_t data_len, hycon_tx *tx_content) {
	size_t idx = 0;
	size_t len, i;
	uint8_t skip_bytes;

	while (idx < data_len) {
		switch ((data[idx] & 0x38)>>3) {	// field number
		case 0:	// shouldn't exist!
			return false;
		case 1:	// to
			if ((data[idx] & 0x7) != 2) return false;	// type doesn't match
			if (idx + 1 >= data_len) return false;	// overflow
			if (data[idx+1] != 20) return false;	// address size must be 20
			//TODO: from should be same with our address
			idx += data[idx+1] + 2;	// skip from address
			break;
		case 2:	// from
			if ((data[idx] & 0x7) != 2) return false;	// type doesn't match
			if (idx + 1 >= data_len) return false;	// overflow
			if (data[idx+1] != 20) return false;	// address size must be 20
			idx++;
			len = data[idx++];
			if (idx + len >= data_len) return false;	// overflow
			for (i = 0; i < len; i++) {
				tx_content->to[i] = data[idx++];
			}
			tx_content->to[i] = '\0';
			break;
		case 3:	// amount
			if ((data[idx] & 0x7) != 0) return false;	// type doesn't match
			tx_content->amount = decode_varint(&data[idx], &skip_bytes);
			idx += skip_bytes;
			break;
		case 4:	// fee
			if ((data[idx] & 0x7) != 0) return false;	// type doesn't match
			tx_content->fee = decode_varint(&data[idx], &skip_bytes);
			idx += skip_bytes;
			break;
		default:	// ignore other fields
			return true;
		}
	}

	return true;
}

void bin_addr_to_hycon_address(uint8_t addr[21], char* out) {
	out[0] = 'H';
	size_t encode_len = base58_encode(&out[1], addr, 20);
	size_t check_sum_len = check_sum(&out[encode_len+1], addr, 20);
	out[encode_len + check_sum_len + 1] = '\0';
}

uint64_t decode_varint(uint8_t *buf, uint8_t *skip_bytes) {
	uint64_t result = 0;
	(*skip_bytes) = 0;
	uint64_t val;

	do {
		buf++;
		val = (*buf) & 0x7f;
		result |= (val << ((*skip_bytes)*7));
		(*skip_bytes)++;
	} while ((*buf) & 0x80);

	(*skip_bytes)++;
	return result;
}

size_t base58_encode(char *out, const void *data, size_t data_len) {
	unsigned char tmp[164];
	unsigned char buffer[164];
	unsigned char j;
	unsigned char start_at;
	unsigned char zero_count = 0;

	if (data_len > sizeof(tmp)) {
#ifndef UNIT_TEST
		THROW(INVALID_PARAMETER);
#else
		return 0;
#endif	// UNIT_TEST
	}
	os_memmove(tmp, data, data_len);
	while ((zero_count < data_len) && (tmp[zero_count] == 0)) {
		zero_count++;
	}
	j = 2 * data_len;
	start_at = zero_count;
	while (start_at < data_len) {
		unsigned short remainder = 0;
		unsigned char div_loop;
		for (div_loop = start_at; div_loop < data_len; div_loop++) {
			unsigned short digit256 = (unsigned short)(tmp[div_loop] & 0xff);
			unsigned short tmp_div = remainder * 256 + digit256;
			tmp[div_loop] = (unsigned char)(tmp_div / 58);
			remainder = (tmp_div % 58);
		}
		if (tmp[start_at] == 0)
			start_at++;
		buffer[--j] = (unsigned char)BASE58_ALPHABET[remainder];
	}
	while ((j < (2 * data_len)) && (buffer[j] == BASE58_ALPHABET[0])) {
		j++;
	}
	while (zero_count-- > 0) {
		buffer[--j] = BASE58_ALPHABET[0];
	}
	data_len = 2 * data_len - j;
	os_memmove(out, (buffer + j), data_len);

	return data_len;
}

size_t check_sum(char *out, const void *data, size_t data_len) {
	const size_t len = 4;
	hycon_hash_t hash;

	blake2b(hash, sizeof(hycon_hash_t), data, 20, &G_blake2b_state, 0);
	unsigned char tmp[164];
	base58_encode(tmp, hash, sizeof(hycon_hash_t));
	os_memcpy(out, tmp, len);

	return len;
}
