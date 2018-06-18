/******************************************************************************
*   $HYCON Wallet for Ledger Nano S
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

#include "os.h"
#include "os_io_seproxyhal.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "tx.pb.h"

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
				handleGetPublicKey(G_io_apdu_buffer[ISO_OFFSET_P1],
					G_io_apdu_buffer[ISO_OFFSET_P2],
					G_io_apdu_buffer + ISO_OFFSET_CDATA,
					G_io_apdu_buffer[ISO_OFFSET_LC], flags, tx);
				break;

			case INS_SIGN:
				handleSign(G_io_apdu_buffer[ISO_OFFSET_P1],
					G_io_apdu_buffer[ISO_OFFSET_P2],
					G_io_apdu_buffer + ISO_OFFSET_CDATA,
					G_io_apdu_buffer[ISO_OFFSET_LC], flags, tx);
				break;

			case INS_GET_APP_CONFIGURATION:
				handleGetAppConfig(G_io_apdu_buffer[ISO_OFFSET_P1],
					G_io_apdu_buffer[ISO_OFFSET_P2], G_io_apdu_buffer + ISO_OFFSET_CDATA,
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
			os_memset(&G_tx_content, 0, sizeof(G_tx_content));
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

void handleSign(uint8_t p1, uint8_t p2, uint8_t *data_buffer,
                uint16_t data_length, volatile unsigned int *flags,
                volatile unsigned int *tx) {
	UNUSED(tx);
	UNUSED(data_length);
	uint8_t i;
	G_bip32_path_length = *(data_buffer++);
	if ((G_bip32_path_length < 0x01) || (G_bip32_path_length > MAX_BIP32_PATH)) {
		THROW(HYCON_SW_INCORRECT_DATA);
	}
	if ((p1 != P1_CONFIRM) && (p1 != P1_NON_CONFIRM)) {
		THROW(HYCON_SW_INCORRECT_P1_P2);
	}

	for (i = 0; i < G_bip32_path_length; i++) {
		G_bip32_path[i] = (data_buffer[0] << 24) | (data_buffer[1] << 16) |
			(data_buffer[2] << 8) | (data_buffer[3]);
		data_buffer += 4;
	}

	uint8_t msg_length = *(data_buffer++);
	pb_istream_t stream = pb_istream_from_buffer(data_buffer, msg_length);
	Tx tx_content = Tx_init_zero;

	//TODO: fix pb_decode (it gets stuck at runtime!)
	if (1/*pb_decode(&stream, Tx_fields, &tx_content)*/) {
		// UI variables setting
		int_to_displayable_chars(tx_content.amount, G_amount);
		int_to_displayable_chars(tx_content.fee, G_fee);

		//TODO: address to dispayable chars
		// get full address
		/*if (pb_decode(&stream, &Tx_fields[1], G_full_address)) {
			G_full_address[4] = '\0';
		} else {
			G_full_address[0] = 'T';
			G_full_address[1] = 'E';
			G_full_address[2] = 'S';
			G_full_address[3] = 'T';
			G_full_address[4] = '\0';
		}*/

		// hash tx
		blake2b(G_tx_hash, sizeof(G_tx_hash), data_buffer,
			msg_length, &G_blake2b_state, 0);

		ux_step = 0;
		ux_step_count = 4;
		UX_DISPLAY(ui_approval_nanos, ui_approval_prepro);
		*flags |= IO_ASYNCH_REPLY;
	} else {
		THROW(HYCON_SW_INCORRECT_DATA);
	}
}

void handleGetPublicKey(uint8_t p1, uint8_t p2, uint8_t *data_buffer,
                        uint16_t data_length, volatile unsigned int *flags,
                        volatile unsigned int *tx) {
	UNUSED(data_length);

	uint8_t privateKeyData[32];
	uint32_t bip32Path[MAX_BIP32_PATH];
	uint32_t i;
	uint8_t bip32PathLength = *(data_buffer++);
	cx_ecfp_private_key_t privateKey;
	cx_ecfp_public_key_t publicKey;
	uint8_t address[21];

	if ((bip32PathLength < 0x01) || (bip32PathLength > MAX_BIP32_PATH)) {
		THROW(HYCON_SW_INCORRECT_DATA);
	}
	if ((p1 != P1_CONFIRM) && (p1 != P1_NON_CONFIRM)) {
		THROW(HYCON_SW_INCORRECT_P1_P2);
	}

	for (i = 0; i < bip32PathLength; i++) {
		bip32Path[i] = (data_buffer[0] << 24) | (data_buffer[1] << 16) |
			(data_buffer[2] << 8) | (data_buffer[3]);
		data_buffer += 4;
	}

	os_perso_derive_node_bip32(CX_CURVE_256K1, bip32Path, bip32PathLength,
		privateKeyData, NULL);
	cx_ecfp_init_private_key(CX_CURVE_256K1, privateKeyData, 32, &privateKey);
	cx_ecfp_generate_pair(CX_CURVE_256K1, &publicKey, &privateKey, 1);
	os_memset(&privateKey, 0, sizeof(privateKey));
	os_memset(privateKeyData, 0, sizeof(privateKeyData));

	if (p1 == P1_NON_CONFIRM) {
		*tx = set_result_publicKey(publicKey);
		THROW(HYCON_SW_OK);
	} else {
		// TODO: complete UI INS_GET_PUBLIC_KEY
		// prepare for a UI based reply
		snprintf(G_public_key_value, sizeof(G_public_key_value), "0x%.*s", 65,
			publicKey.W);

		ux_step = 0;
		ux_step_count = 2;
		UX_DISPLAY(ui_address_nanos, ui_address_prepro);

		*flags |= IO_ASYNCH_REPLY;
	}
}

void handleGetAppConfig(uint8_t p1, uint8_t p2, uint8_t *data_buffer,
                        uint16_t data_length, volatile unsigned int *flags,
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
