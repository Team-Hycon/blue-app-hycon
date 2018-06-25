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

#include "ui.h"
#include "glyphs.h"
#include "ram_variables.h"

void ui_idle(void) {
	ux_step = 0;
	ux_step_count = 0;
	G_ui_state = UI_IDLE;
	UX_MENU_DISPLAY(0, menu_main, NULL);
}

/* ------------------------------------------------------------------------- */
/* ---                           MENU PREPROS                            --- */
/* ------------------------------------------------------------------------- */

const ux_menu_entry_t menu_main[] = {
	{NULL, NULL, 0, NULL, "Use wallet to",
	"view accounts", 0, 0},
	{menu_about, NULL, 0, NULL, "About", NULL, 0, 0},
	{NULL, os_sched_exit, 0, &C_nanos_icon_dashboard, "Quit app", NULL, 50, 29},
	UX_MENU_END
};

const ux_menu_entry_t menu_about[] = {
	{NULL, NULL, 0, NULL, "Version", APPVERSION, 0, 0},
	{menu_main, NULL, 1, &C_nanos_icon_back, "Back", NULL, 61, 40},
	UX_MENU_END
};

unsigned int ui_approval_prepro(const bagl_element_t *element) {
	unsigned int display = 1;
	if (element->component.userid > 0) {
		display = (ux_step == element->component.userid - 1);
		if (display) {
			switch (element->component.userid) {
			case 1:
				UX_CALLBACK_SET_INTERVAL(2000);
				break;
			case 2:
				UX_CALLBACK_SET_INTERVAL(MAX(
					3000, 1000 + bagl_label_roundtrip_duration_ms(element, 7)));
				break;
			case 3:
				UX_CALLBACK_SET_INTERVAL(MAX(
					4000, 2000 + bagl_label_roundtrip_duration_ms(element, 7)));
				break;
			case 4:
				UX_CALLBACK_SET_INTERVAL(MAX(
					3000, 1000 + bagl_label_roundtrip_duration_ms(element, 7)));
				break;
			}
		}
	}

	return display;
}

unsigned int ui_address_prepro(const bagl_element_t *element) {
	if (element->component.userid > 0) {
		unsigned int display = (ux_step == element->component.userid - 1);
		if (display) {
			switch (element->component.userid) {
			case 1:
				UX_CALLBACK_SET_INTERVAL(2000);
				break;
			case 2:
				UX_CALLBACK_SET_INTERVAL(MAX(
					3000, 1000 + bagl_label_roundtrip_duration_ms(element, 7)));
				break;
			}
		}
		return display;
	}

	return 1;
}

/* ------------------------------------------------------------------------- */
/* ---                           MENUS                                   --- */
/* ------------------------------------------------------------------------- */

const bagl_element_t ui_approval_nanos[] = {
	// type                               userid    x    y   w    h  str rad
	// fill      fg        bg      fid iid  txt   touchparams...       ]
	{{BAGL_RECTANGLE, 0x00, 0, 0, 128, 32, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF,
		0, 0},
	NULL,
	0,
	0,
	0,
	NULL,
	NULL,
	NULL},
	{{BAGL_ICON, 0x00, 3, 12, 7, 7, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
		BAGL_GLYPH_ICON_CROSS},
	NULL,
	0,
	0,
	0,
	NULL,
	NULL,
	NULL},
	{{BAGL_ICON, 0x00, 117, 13, 8, 6, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
		BAGL_GLYPH_ICON_CHECK},
	NULL,
	0,
	0,
	0,
	NULL,
	NULL,
	NULL},

	//{{BAGL_ICON                           , 0x01,  21,   9,  14,  14, 0, 0, 0
	//, 0xFFFFFF, 0x000000, 0, BAGL_GLYPH_ICON_TRANSACTION_BADGE  }, NULL, 0, 0,
	//0, NULL, NULL, NULL },
	{{BAGL_LABELINE, 0x01, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
		BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
	"Confirm",
	0,
	0,
	0,
	NULL,
	NULL,
	NULL},
	{{BAGL_LABELINE, 0x01, 0, 26, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
		BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
	"transaction",
	0,
	0,
	0,
	NULL,
	NULL,
	NULL},

	{{BAGL_LABELINE, 0x02, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
		BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
	"Amount",
	0,
	0,
	0,
	NULL,
	NULL,
	NULL},
	{{BAGL_LABELINE, 0x02, 23, 26, 82, 12, 0x80 | 10, 0, 0, 0xFFFFFF, 0x000000,
		BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 26},
	(char *)G_ram.ui_amount,
	0,
	0,
	0,
	NULL,
	NULL,
	NULL},

	{{BAGL_LABELINE, 0x03, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
		BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
	"Address",
	0,
	0,
	0,
	NULL,
	NULL,
	NULL},
	{{BAGL_LABELINE, 0x03, 23, 26, 82, 12, 0x80 | 10, 0, 0, 0xFFFFFF, 0x000000,
		BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 50},
	(char *)G_ram.ui_full_address,
	0,
	0,
	0,
	NULL,
	NULL,
	NULL},

	{{BAGL_LABELINE, 0x04, 0, 12, 128, 32, 0, 0, 0, 0xFFFFFF, 0x000000,
		BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
	"Fees",
	0,
	0,
	0,
	NULL,
	NULL,
	NULL},
	{{BAGL_LABELINE, 0x04, 23, 26, 82, 12, 0x80 | 10, 0, 0, 0xFFFFFF, 0x000000,
		BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 26},
	(char *)G_ram.ui_fee,
	0,
	0,
	0,
	NULL,
	NULL,
	NULL}
};

const bagl_element_t ui_address_nanos[] = {
	// type                               userid    x    y   w    h  str rad
	// fill      fg        bg      fid iid  txt   touchparams...       ]
	{{BAGL_RECTANGLE, 0x00, 0, 0, 128, 32, 0, 0, BAGL_FILL, 0x000000, 0xFFFFFF,
		0, 0},
	NULL,
	0,
	0,
	0,
	NULL,
	NULL,
	NULL},

	{{BAGL_ICON, 0x00, 3, 12, 7, 7, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
		BAGL_GLYPH_ICON_CROSS},
	NULL,
	0,
	0,
	0,
	NULL,
	NULL,
	NULL},

	{{BAGL_ICON, 0x00, 117, 13, 8, 6, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
		BAGL_GLYPH_ICON_CHECK},
	NULL,
	0,
	0,
	0,
	NULL,
	NULL,
	NULL},

	//{{BAGL_ICON                           , 0x01,  31,   9,  14,  14, 0, 0, 0
	//, 0xFFFFFF, 0x000000, 0, BAGL_GLYPH_ICON_EYE_BADGE  }, NULL, 0, 0, 0,
	//NULL, NULL, NULL },
	{{BAGL_LABELINE, 0x01, 0, 12, 128, 12, 0, 0, 0, 0xFFFFFF, 0x000000,
		BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
	"Confirm",
	0,
	0,
	0,
	NULL,
	NULL,
	NULL},

	{{BAGL_LABELINE, 0x01, 0, 26, 128, 12, 0, 0, 0, 0xFFFFFF, 0x000000,
		BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
	"publicKey",
	0,
	0,
	0,
	NULL,
	NULL,
	NULL},

	{{BAGL_LABELINE, 0x02, 0, 12, 128, 12, 0, 0, 0, 0xFFFFFF, 0x000000,
		BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0},
	"PublicKey",
	0,
	0,
	0,
	NULL,
	NULL,
	NULL},

	{{BAGL_LABELINE, 0x02, 23, 26, 82, 12, 0x80 | 10, 0, 0, 0xFFFFFF, 0x000000,
		BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 26},
	(char *)G_public_key_value,
	0,
	0,
	0,
	NULL,
	NULL,
	NULL}
};

/* ------------------------------------------------------------------------- */
/* ---                           MENU BUTTONS                            --- */
/* ------------------------------------------------------------------------- */

unsigned int ui_approval_nanos_button(unsigned int button_mask,
                                      unsigned int button_mask_counter) {
	switch (button_mask) {
	case BUTTON_EVT_RELEASED | BUTTON_LEFT: // CANCEL
		io_seproxyhal_touch_tx_cancel(NULL);
		break;

	case BUTTON_EVT_RELEASED | BUTTON_RIGHT: // OK
		io_seproxyhal_touch_tx_ok(NULL);
		break;
	}

	return 0;
}

unsigned int ui_address_nanos_button(unsigned int button_mask,
                                     unsigned int button_mask_counter) {
	switch (button_mask) {
	case BUTTON_EVT_RELEASED | BUTTON_LEFT: // CANCEL
		io_seproxyhal_touch_address_cancel(NULL);
		break;

	case BUTTON_EVT_RELEASED | BUTTON_RIGHT: { // OK
		io_seproxyhal_touch_address_ok(NULL);
		break;
	}
	}

	return 0;
}

/* ------------------------------------------------------------------------- */
/* ---                           UI LOGICS                               --- */
/* ------------------------------------------------------------------------- */

unsigned int io_seproxyhal_touch_address_ok(const bagl_element_t *e) {
	//uint32_t tx = set_result_get_publicKey();
	G_io_apdu_buffer[0] = 0x90;
	G_io_apdu_buffer[1] = 0x00;
	// Send back the response, do not restart the event loop
	io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, 2);
	// Display back the original UX
	ui_idle();

	return 0; // do not redraw the widget
}

unsigned int io_seproxyhal_touch_address_cancel(const bagl_element_t *e) {
	G_io_apdu_buffer[0] = 0x69;
	G_io_apdu_buffer[1] = 0x85;
	// Send back the response, do not restart the event loop
	io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, 2);
	// Display back the original UX
	ui_idle();

	return 0; // do not redraw the widget
}

unsigned int io_seproxyhal_touch_tx_ok(const bagl_element_t *e) {
	uint8_t privateComponent[32];
	uint8_t signature[100];
	uint8_t signatureLength;
	cx_ecfp_private_key_t privateKey;
	uint32_t tx = 0;
	uint8_t r_length, s_length;
	uint8_t r_offset, s_offset;

	os_perso_derive_node_bip32(CX_CURVE_256K1, G_bip32_path, G_bip32_path_length,
		privateComponent, NULL);
	cx_ecfp_init_private_key(CX_CURVE_256K1, privateComponent, 32, &privateKey);
	os_memset(privateComponent, 0, sizeof(privateComponent));

	unsigned int info = 0;
	signatureLength = cx_ecdsa_sign(&privateKey, CX_RND_RFC6979 | CX_LAST,
		CX_SHA256, G_tx_hash, sizeof(G_tx_hash), signature, 6+2*(32+1), &info);
	os_memset(&privateKey, 0, sizeof(privateKey));

	if (info & CX_ECCINFO_PARITY_ODD)
		signature[0] |= 0x01;

	G_io_apdu_buffer[0] = signature[0] & 0x01;
	r_length = signature[3];
	s_length = signature[4 + r_length + 1];
	r_offset = (r_length == 33 ? 1 : 0);
	s_offset = (s_length == 33 ? 1 : 0);
	os_memmove(G_io_apdu_buffer + 1, signature + 4 + r_offset, 32);
	os_memmove(G_io_apdu_buffer + 1 + 32, signature + 4 + r_length + 2 + s_offset,
		32);
	tx = 65;
	G_io_apdu_buffer[tx++] = 0x90;
	G_io_apdu_buffer[tx++] = 0x00;

	// Send back the response, do not restart the event loop
	io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, tx);

	// Display back the original UX
	ui_idle();
	return 0; // do not redraw the widget
}

unsigned int io_seproxyhal_touch_tx_cancel(const bagl_element_t *e) {
	G_io_apdu_buffer[0] = 0x69;
	G_io_apdu_buffer[1] = 0x85;

	// Send back the response, do not restart the event loop
	io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, 2);
	// Display back the original UX
	ui_idle();
	return 0; // do not redraw the widget
}
