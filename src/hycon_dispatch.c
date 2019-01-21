/******************************************************************************
*   HYCON Wallet for Ledger Nano S
*   (c) 2018 Dulguun Batmunkh
*   (c) 2019 Joonbum Lee <jbamlee65@gmail.com>
*   (c) 2018~2019 Hycon
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

#include "os.h"
#include "os_io_seproxyhal.h"

#include "ui.h"
#include "hycon_constants.h"
#include "ram_variables.h"

void handleApdu(volatile unsigned int *flags, volatile unsigned int *tx) {
	unsigned short sw = 0;

	BEGIN_TRY {
		TRY {
			if (G_io_apdu_buffer[ISO_OFFSET_CLA] != CLA) {
				THROW(HYCON_SW_CLA_NOT_SUPPORTED);
			}

			switch (G_io_apdu_buffer[ISO_OFFSET_INS]) {
			case INS_GET_PUBLIC_KEY:
				handle_get_public_key(G_io_apdu_buffer[ISO_OFFSET_P1],
					G_io_apdu_buffer[ISO_OFFSET_P2],
					G_io_apdu_buffer + ISO_OFFSET_CDATA,
					G_io_apdu_buffer[ISO_OFFSET_LC], flags, tx);
				break;

			case INS_SIGN:
				handle_sign(G_io_apdu_buffer[ISO_OFFSET_P1],
					G_io_apdu_buffer[ISO_OFFSET_P2],
					G_io_apdu_buffer + ISO_OFFSET_CDATA,
					G_io_apdu_buffer[ISO_OFFSET_LC], flags, tx);
				break;

			case INS_GET_APP_CONFIGURATION:
				handle_get_app_config(G_io_apdu_buffer[ISO_OFFSET_P1],
					G_io_apdu_buffer[ISO_OFFSET_P2],
					G_io_apdu_buffer + ISO_OFFSET_CDATA,
					G_io_apdu_buffer[ISO_OFFSET_LC], flags, tx);
				break;

			default:
				THROW(HYCON_SW_INS_NOT_SUPPORTED);
				break;
			}
	}
	CATCH_OTHER(e) {
		switch (e & 0xF000) {
		case 0x6000:
			// Wipe the transaction context and report the exception
			sw = e;
			os_memset(&G_ram, 0, sizeof(G_ram));
			break;
		case HYCON_SW_OK:
			// All is well
			sw = e;
			break;
		default:
			// Internal error
			sw = 0x6800 | (e & 0x7FF);
			break;
		}
		// Unexpected exception => report
		G_io_apdu_buffer[*tx] = sw >> 8;
		G_io_apdu_buffer[*tx + 1] = sw;
		*tx += 2;
	}
	FINALLY {
	}
	}
	END_TRY;
}

void handle_sign(uint8_t p1, uint8_t p2, uint8_t *data_buffer,
                 uint8_t data_len, volatile unsigned int *flags,
                 volatile unsigned int *tx) {
	UNUSED(tx);
	G_bip32_path_len = (*data_buffer++);
	data_len--;

	if ((G_bip32_path_len < 0x01) || (G_bip32_path_len > MAX_BIP32_PATH)) {
		THROW(HYCON_SW_INCORRECT_DATA);
	}

	if (p1 != P1_FIRST || p2 != P1_FIRST)
		THROW(HYCON_SW_INCORRECT_P1_P2);

	uint8_t i;
	for (i = 0; i < G_bip32_path_len; i++) {
		G_bip32_path[i] =
			((uint32_t)data_buffer[0] << 24) | ((uint32_t)data_buffer[1] << 16)
			| ((uint32_t)data_buffer[2] << 8) | (data_buffer[3]);
		data_buffer += 4;
		data_len -= 4;
	}

	hycon_tx tx_content = {0};
	if (decode_tx(data_buffer, data_len, &tx_content))
	{
		// UI variables setting
		uint8_t ticker_offset = 0;
		while (HYC_TICKER[ticker_offset]) {
			G_ram.ui_amount[ticker_offset] = HYC_TICKER[ticker_offset];
			ticker_offset++;
		}
		coin_amount_to_displayable_chars(tx_content.amount,
			G_ram.ui_amount + ticker_offset);

		ticker_offset = 0;
		while (HYC_TICKER[ticker_offset]) {
			G_ram.ui_fee[ticker_offset] = HYC_TICKER[ticker_offset];
			ticker_offset++;
		}
		coin_amount_to_displayable_chars(tx_content.fee,
			G_ram.ui_fee + ticker_offset);

		bin_addr_to_hycon_address(tx_content.to, G_ram.ui_full_address);

	// hash tx
#ifndef UNIT_TEST
	cx_blake2b_init(&G_blake2b_hash.header, 256);
	cx_hash(&G_blake2b_hash.header, CX_LAST, data_buffer, data_len, G_ram.tx_hash, sizeof(G_ram.tx_hash));
#else
	blake2b(G_ram.tx_hash, sizeof(G_ram.tx_hash), data_buffer, data_len, &G_blake2b_state, 0);
#endif

#if defined(TARGET_BLUE)
		ui_approval_transaction_blue_init();
#elif defined(TARGET_NANOS)
		ux_step = 0;
		ux_step_count = 4;
		UX_DISPLAY(ui_approval_nanos, ui_approval_prepro);
#endif // #if TARGET_ID

		*flags |= IO_ASYNCH_REPLY;
	} else {
		THROW(HYCON_SW_INCORRECT_DATA);
	}
}

void handle_get_public_key(uint8_t p1, uint8_t p2, uint8_t *data_buffer,
                           uint8_t data_len, volatile unsigned int *flags,
                           volatile unsigned int *tx) {
	UNUSED(data_len);

	uint8_t private_component[32];
	uint32_t i;
	uint8_t G_bip32_path_len = *(data_buffer++);
	cx_ecfp_private_key_t private_key;

	if ((G_bip32_path_len < 0x01) || (G_bip32_path_len > MAX_BIP32_PATH)) {
		THROW(HYCON_SW_INCORRECT_DATA);
	}
	if ((p1 != P1_CONFIRM) && (p1 != P1_NON_CONFIRM)) {
		THROW(HYCON_SW_INCORRECT_P1_P2);
	}

	for (i = 0; i < G_bip32_path_len; i++) {
		G_bip32_path[i] = (data_buffer[0] << 24) | (data_buffer[1] << 16) |
			(data_buffer[2] << 8) | (data_buffer[3]);
		data_buffer += 4;
	}

	os_perso_derive_node_bip32(CX_CURVE_256K1, G_bip32_path, G_bip32_path_len,
		private_component, NULL);
	cx_ecfp_init_private_key(CX_CURVE_256K1, private_component, 32, &private_key);
	cx_ecfp_generate_pair(CX_CURVE_256K1, &G_public_key, &private_key, 1);
	os_memset(&private_key, 0, sizeof(private_key));
	os_memset(private_component, 0, sizeof(private_component));

	if (p1 == P1_NON_CONFIRM) {
		*tx = set_result_public_key();
		THROW(HYCON_SW_OK);
	} else {
		uint8_t hex_address[21];
		get_address_string_from_key(G_public_key, hex_address);
		bin_addr_to_hycon_address(hex_address, G_ram.ui_full_address);

#if defined(TARGET_BLUE)
		UX_DISPLAY(ui_address_blue, ui_address_blue_prepro);
#elif defined(TARGET_NANOS)
		ux_step = 0;
		ux_step_count = 2;
		UX_DISPLAY(ui_address_nanos, ui_address_prepro);
#endif // #if TARGET_ID

		*flags |= IO_ASYNCH_REPLY;
	}
}

void handle_get_app_config(uint8_t p1, uint8_t p2, uint8_t *data_buffer,
                           uint8_t data_length, volatile unsigned int *flags,
                           volatile unsigned int *tx) {
	UNUSED(p1);
	UNUSED(p2);
	UNUSED(data_buffer);
	UNUSED(data_length);
	UNUSED(flags);

	G_io_apdu_buffer[0] = LEDGER_MAJOR_VERSION;
	G_io_apdu_buffer[1] = LEDGER_MINOR_VERSION;
	G_io_apdu_buffer[2] = LEDGER_PATCH_VERSION;
	*tx = 3;

	THROW(HYCON_SW_OK);
}
