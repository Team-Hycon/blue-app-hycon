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
#include "hycon_api.h"

void ui_idle(void) {
	ux_step = 0;
	ux_step_count = 0;
	G_ui_state = UI_IDLE;

#if defined(TARGET_BLUE)
	UX_DISPLAY(ui_idle_blue, NULL);
#elif defined(TARGET_NANOS)
	UX_MENU_DISPLAY(0, menu_main, NULL);
#endif // TARGET_ID
}

/* ------------------------------------------------------------------------- */
/* ---                           MENU PREPROS                            --- */
/* ------------------------------------------------------------------------- */

#if defined(TARGET_NANOS)
const ux_menu_entry_t menu_main[] = {
	{
		NULL, NULL, 0, NULL, "Use wallet to", "view accounts", 0, 0
	},

	{
		menu_about, NULL, 0, NULL, "About", NULL, 0, 0
	},

	{
		NULL, os_sched_exit, 0, &C_nanos_icon_dashboard, "Quit app", NULL, 50,
		29
	},

	UX_MENU_END
};

const ux_menu_entry_t menu_about[] = {
	{
		NULL, NULL, 0, NULL, "Version", APPVERSION, 0, 0
	},

	{
		menu_main, NULL, 1, &C_nanos_icon_back, "Back", NULL, 61, 40
	},

	UX_MENU_END
};
#endif // TARGET_NANOS

unsigned int ui_approval_prepro(const bagl_element_t *element) {
	bool display = 1;
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

#if defined(TARGET_BLUE)
unsigned int ui_address_blue_prepro(const bagl_element_t *element) {
	os_memmove(&tmp_element, element, sizeof(bagl_element_t));
	if(element->component.userid > 0) {
		unsigned int length = strlen(G_ram.ui_full_address);
		if (length >= (element->component.userid & 0xF) * MAX_CHAR_PER_LINE) {
			os_memset(G_ram.ui_address_summary, 0, MAX_CHAR_PER_LINE+1);
			os_memmove(
				G_ram.ui_address_summary,
				G_ram.ui_full_address +
					(element->component.userid & 0xF) * MAX_CHAR_PER_LINE,
				MIN(length -
					(element->component.userid & 0xF) * MAX_CHAR_PER_LINE,
					MAX_CHAR_PER_LINE));
			return &tmp_element;
		}
		// nothing to draw for this line
		return 0;
	}
	return &tmp_element;
}

const bagl_element_t *ui_details_blue_prepro(const bagl_element_t *element) {
	if (element->component.userid == 1) {
		os_memmove(&tmp_element, element, sizeof(bagl_element_t));
		tmp_element.text = ui_details_title;
		return &tmp_element;
	} else if (element->component.userid > 0) {
		unsigned int length = strlen(ui_details_content);
		if (length >= (element->component.userid & 0xF) * MAX_CHAR_PER_LINE) {
			os_memset(G_ram.ui_address_summary, 0, MAX_CHAR_PER_LINE + 1);
			os_memmove(
				(char*) G_ram.ui_address_summary,
				ui_details_content +
					(element->component.userid & 0xF) * MAX_CHAR_PER_LINE,
				MIN(length -
					(element->component.userid & 0xF) * MAX_CHAR_PER_LINE,
					MAX_CHAR_PER_LINE));
			return 1;
		}
		// nothing to draw for this line
		return 0;
	}
	return 1;
}

const bagl_element_t* ui_approval_blue_prepro(const bagl_element_t* element) {
	os_memmove(&tmp_element, element, sizeof(bagl_element_t));
	if (element->component.userid == 0) {
		return &tmp_element;
	}
	// none elements are skipped
	if ((element->component.type&(~BAGL_FLAG_TOUCHABLE)) == BAGL_NONE) {
		return 0;
	}
	else {
		switch(element->component.userid&0xF0) {

		// icon
		case 0x40:
			return &tmp_element;

		// TITLE
		case 0x60:
			tmp_element.text = ui_approval_blue_details_name[ui_approval_blue_state][3];
			return &tmp_element;

		// SUBLINE
		case 0x50:
			tmp_element.text = ui_approval_blue_details_name[ui_approval_blue_state][4];
			return &tmp_element;

		// details label
		case 0x70:
			if (!ui_approval_blue_details_name[ui_approval_blue_state][element->component.userid&0xF])
				return NULL;
			tmp_element.text = ui_approval_blue_details_name[ui_approval_blue_state][element->component.userid&0xF];
			return &tmp_element;

		// detail value
		case 0x10:
			if (!ui_approval_blue_details_name[ui_approval_blue_state][element->component.userid&0xF])
				return NULL;
			tmp_element.text = ui_approval_blue_values[(element->component.userid&0xF)];

			// x -= 18 when overflow is detected
			if (strlen(ui_approval_blue_values[(element->component.userid&0xF)])*BAGL_FONT_OPEN_SANS_LIGHT_16_22PX_AVG_WIDTH >= 160)
				tmp_element.component.x -= 18;

			return &tmp_element;

		// right arrow and left selection rectangle
		case 0x20:
			if (!ui_approval_blue_details_name[ui_approval_blue_state][element->component.userid&0xF])
				return NULL;
			if (strlen(ui_approval_blue_values[(element->component.userid&0xF)])*BAGL_FONT_OPEN_SANS_LIGHT_16_22PX_AVG_WIDTH < 160)
				return NULL;

		case 0x30:
			return ui_approval_blue_details_name[ui_approval_blue_state][element->component.userid&0xF]!=NULL?&tmp_element:NULL;
		case 0x05:
			return &tmp_element;
		case 0x06:
			return NULL;
		}
	}
	return &tmp_element;
}

void ui_approval_blue_init(void) {
	UX_DISPLAY(ui_approval_blue, ui_approval_blue_prepro);
}

void ui_approval_transaction_blue_init(void) {
	ui_approval_blue_ok = (bagl_element_callback_t)io_seproxyhal_touch_tx_ok;
	ui_approval_blue_cancel =
		(bagl_element_callback_t)io_seproxyhal_touch_tx_cancel;
	ui_approval_blue_state = APPROVAL_TRANSACTION;
	ui_approval_blue_values[0] = (const char*) G_ram.ui_amount;
	ui_approval_blue_values[1] = (const char*) G_ram.ui_full_address;
	ui_approval_blue_values[2] = (const char*) G_ram.ui_fee;
	ui_approval_blue_init();
}

void ui_approval_blue_1_details(const bagl_element_t *e) {
	if (strlen(ui_approval_blue_values[0]) *
			BAGL_FONT_OPEN_SANS_LIGHT_16_22PX_AVG_WIDTH >=
			160) {
		ui_details_init(
			ui_approval_blue_details_name[ui_approval_blue_state][0],
			ui_approval_blue_values[0], ui_approval_blue_init);
	}
};

void ui_approval_blue_2_details(const bagl_element_t *e) {
	if (strlen(ui_approval_blue_values[1]) *
			BAGL_FONT_OPEN_SANS_REGULAR_10_13PX_AVG_WIDTH >=
			160) {
		ui_details_init(
			ui_approval_blue_details_name[ui_approval_blue_state][1],
			ui_approval_blue_values[1], ui_approval_blue_init);
	}
};

void ui_approval_blue_3_details(const bagl_element_t *e) {
	if (strlen(ui_approval_blue_values[2]) *
			BAGL_FONT_OPEN_SANS_REGULAR_10_13PX_AVG_WIDTH >=
			160) {
		ui_details_init(
			ui_approval_blue_details_name[ui_approval_blue_state][2],
			ui_approval_blue_values[2], ui_approval_blue_init);
	}
};

void ui_details_init(const char *title, const char *content,
                     callback_t back_callback) {
	ui_details_title = title;
	ui_details_content = content;
	ui_details_back_callback = back_callback;
	UX_DISPLAY(ui_details_blue, ui_details_blue_prepro);
}

const bagl_element_t *ui_menu_item_out_over(const bagl_element_t *e) {
	// the selection rectangle is after the none|touchable
	e = (const bagl_element_t *)(((unsigned int)e) + sizeof(bagl_element_t));
	return e;
}
#endif // TARGET_BLUE

/* ------------------------------------------------------------------------- */
/* ---                           MENUS                                   --- */
/* ------------------------------------------------------------------------- */

#if defined(TARGET_NANOS)
const bagl_element_t ui_approval_nanos[] = {
	{
		{
			BAGL_RECTANGLE, 0x00, 0, 0, 128, 32, 0, 0, BAGL_FILL, 0x000000,
			0xFFFFFF, 0, 0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_ICON, 0x00, 3, 12, 7, 7, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
			BAGL_GLYPH_ICON_CROSS
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_ICON, 0x00, 117, 13, 8, 6, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
			BAGL_GLYPH_ICON_CHECK
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x01, 0, 12, 128, 12, 0, 0, 0, 0xFFFFFF, 0x000000,
			BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0
		},
		"Confirm",
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x01, 0, 26, 128, 12, 0, 0, 0, 0xFFFFFF, 0x000000,
			BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0
		},
		"transaction",
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x02, 0, 12, 128, 12, 0, 0, 0, 0xFFFFFF, 0x000000,
			BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0
		},
		"Amount",
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x02, 23, 26, 82, 12, 0x80 | 10, 0, 0, 0xFFFFFF,
			0x000000,
			BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 26
		},
		G_ram.ui_amount,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x03, 0, 12, 128, 12, 0, 0, 0, 0xFFFFFF, 0x000000,
			BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0
		},
		"Address",
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x03, 23, 26, 82, 12, 0x80 | 10, 0, 0, 0xFFFFFF,
			0x000000,
			BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 50
		},
		G_ram.ui_full_address,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x04, 0, 12, 128, 12, 0, 0, 0, 0xFFFFFF, 0x000000,
			BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0
		},
		"Fees",
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x04, 23, 26, 82, 12, 0x80 | 10, 0, 0, 0xFFFFFF,
			0x000000,
			BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 26
		},
		G_ram.ui_fee,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	}
};

const bagl_element_t ui_address_nanos[] = {
	{
		{
			BAGL_RECTANGLE, 0x00, 0, 0, 128, 32, 0, 0, BAGL_FILL, 0x000000,
			0xFFFFFF, 0, 0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_ICON, 0x00, 3, 12, 7, 7, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
			BAGL_GLYPH_ICON_CROSS
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_ICON, 0x00, 117, 13, 8, 6, 0, 0, 0, 0xFFFFFF, 0x000000, 0,
			BAGL_GLYPH_ICON_CHECK
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x01, 0, 12, 128, 12, 0, 0, 0, 0xFFFFFF, 0x000000,
			BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0
		},
		"Confirm",
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x01, 0, 26, 128, 12, 0, 0, 0, 0xFFFFFF, 0x000000,
			BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER, 0
		},
		"address",
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x02, 0, 12, 128, 12, 0, 0, 0, 0xFFFFFF, 0x000000,
			BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, 0
		},
		"Address",
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x02, 23, 26, 82, 12, 0x80 | 10, 0, 0, 0xFFFFFF,
			0x000000,
			BAGL_FONT_OPEN_SANS_EXTRABOLD_11px | BAGL_FONT_ALIGNMENT_CENTER,
			26
		},
		G_ram.ui_full_address,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	}
};
#endif // TARGET_NANOS

#if defined(TARGET_BLUE)
const bagl_element_t ui_approval_blue[] = {
	{
		{
			BAGL_RECTANGLE, 0x00, 0, 68, 320, 413, 0, 0, BAGL_FILL, COLOR_BG_1,
			0x000000, 0, 0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_RECTANGLE, 0x00, 0, 20, 320, 48, 0, 0, BAGL_FILL, COLOR_APP,
			COLOR_APP, 0, 0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x60, 0, 45, 320, 30, 0, 0, BAGL_FILL, 0xFFFFFF,
			COLOR_APP,
			BAGL_FONT_OPEN_SANS_SEMIBOLD_10_13PX | BAGL_FONT_ALIGNMENT_CENTER,
			0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_ICON, 0x40, 30, 98, 50, 50, 0, 0, BAGL_FILL, 0, COLOR_BG_1, 0,
			0
		},
		&C_badge_transaction,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x50, 100, 117, 320, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1, BAGL_FONT_OPEN_SANS_REGULAR_10_13PX, 0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x00, 100, 138, 320, 30, 0, 0, BAGL_FILL, 0x999999,
			COLOR_BG_1, BAGL_FONT_OPEN_SANS_REGULAR_8_11PX, 0
		},
		"Check and confirm values",
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x70, 30, 196, 100, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1, BAGL_FONT_OPEN_SANS_SEMIBOLD_8_11PX, 0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x10, 130, 200, 160, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1,
			BAGL_FONT_OPEN_SANS_LIGHT_16_22PX | BAGL_FONT_ALIGNMENT_RIGHT, 0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x20, 284, 196, 6, 16, 0, 0, BAGL_FILL, 0x999999,
			COLOR_BG_1, BAGL_FONT_SYMBOLS_0 | BAGL_FONT_ALIGNMENT_RIGHT, 0
		},
		BAGL_FONT_SYMBOLS_0_MINIRIGHT,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_NONE | BAGL_FLAG_TOUCHABLE, 0x00, 0, 168, 320, 48, 0, 9,
			BAGL_FILL, 0xFFFFFF, 0x000000, 0, 0
		},
		NULL,
		0,
		0xEEEEEE,
		0x000000,
		ui_approval_blue_1_details,
		ui_menu_item_out_over,
		ui_menu_item_out_over
	},

	{
		{
			BAGL_RECTANGLE, 0x20, 0, 168, 5, 48, 0, 0, BAGL_FILL, COLOR_BG_1,
			COLOR_BG_1, 0, 0
		},
		NULL,
		0,
		0x41CCB4,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_RECTANGLE, 0x31, 30, 216, 260, 1, 1, 0, 0, 0xEEEEEE,
			COLOR_BG_1, 0, 0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x71, 30, 245, 100, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1, BAGL_FONT_OPEN_SANS_SEMIBOLD_8_11PX, 0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x11, 130, 245, 160, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1,
			BAGL_FONT_OPEN_SANS_REGULAR_10_13PX | BAGL_FONT_ALIGNMENT_RIGHT,
			0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x21, 284, 245, 6, 16, 0, 0, BAGL_FILL, 0x999999,
			COLOR_BG_1, BAGL_FONT_SYMBOLS_0 | BAGL_FONT_ALIGNMENT_RIGHT, 0
		},
		BAGL_FONT_SYMBOLS_0_MINIRIGHT,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_NONE | BAGL_FLAG_TOUCHABLE, 0x00, 0, 217, 320, 48, 0, 9,
			BAGL_FILL, 0xFFFFFF, 0x000000, 0, 0
		},
		NULL,
		0,
		0xEEEEEE,
		0x000000,
		ui_approval_blue_2_details,
		ui_menu_item_out_over,
		ui_menu_item_out_over
	},

	{
		{
			BAGL_RECTANGLE, 0x21, 0, 217, 5, 48, 0, 0, BAGL_FILL, COLOR_BG_1,
			COLOR_BG_1, 0, 0
		},
		NULL,
		0,
		0x41CCB4,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_RECTANGLE, 0x32, 30, 265, 260, 1, 1, 0, 0, 0xEEEEEE,
			COLOR_BG_1, 0, 0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x72, 30, 294, 100, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1, BAGL_FONT_OPEN_SANS_SEMIBOLD_8_11PX, 0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x12, 130, 294, 160, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1,
			BAGL_FONT_OPEN_SANS_REGULAR_10_13PX | BAGL_FONT_ALIGNMENT_RIGHT, 0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x22, 284, 294, 6, 16, 0, 0, BAGL_FILL, 0x999999,
			COLOR_BG_1, BAGL_FONT_SYMBOLS_0 | BAGL_FONT_ALIGNMENT_RIGHT, 0
		},
		BAGL_FONT_SYMBOLS_0_MINIRIGHT,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_NONE | BAGL_FLAG_TOUCHABLE, 0x00, 0, 266, 320, 48, 0, 9,
			BAGL_FILL, 0xFFFFFF, 0x000000, 0, 0
		},
		NULL,
		0,
		0xEEEEEE,
		0x000000,
		ui_approval_blue_3_details,
		ui_menu_item_out_over,
		ui_menu_item_out_over
	},

	{
		{
			BAGL_RECTANGLE, 0x22, 0, 266, 5, 48, 0, 0, BAGL_FILL, COLOR_BG_1,
			COLOR_BG_1, 0, 0
		},
		NULL,
		0,
		0x41CCB4,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_RECTANGLE, 0x06, 30, 314, 260, 1, 1, 0, 0, 0xEEEEEE,
			COLOR_BG_1, 0, 0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_RECTANGLE | BAGL_FLAG_TOUCHABLE, 0x00, 40, 414, 115, 36, 0,
			18, BAGL_FILL, 0xCCCCCC, COLOR_BG_1,
			BAGL_FONT_OPEN_SANS_REGULAR_11_14PX | BAGL_FONT_ALIGNMENT_CENTER |
			BAGL_FONT_ALIGNMENT_MIDDLE, 0
		},
		"REJECT",
		0,
		0xB7B7B7,
		COLOR_BG_1,
		ui_approval_blue_cancel_callback,
		NULL,
		NULL
	},

	{
		{
			BAGL_RECTANGLE | BAGL_FLAG_TOUCHABLE, 0x00, 165, 414, 115, 36, 0,
			18, BAGL_FILL, 0x41ccb4, COLOR_BG_1,
			BAGL_FONT_OPEN_SANS_REGULAR_11_14PX | BAGL_FONT_ALIGNMENT_CENTER |
			BAGL_FONT_ALIGNMENT_MIDDLE, 0
		},
		"CONFIRM",
		0,
		0x3ab7a2,
		COLOR_BG_1,
		ui_approval_blue_ok_callback,
		NULL,
		NULL
	}
};

const bagl_element_t ui_address_blue[] = {
	{
		{
			BAGL_RECTANGLE, 0x00, 0, 68, 320, 413, 0, 0, BAGL_FILL, COLOR_BG_1,
			0x000000, 0, 0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_RECTANGLE, 0x00, 0, 20, 320, 48, 0, 0, BAGL_FILL, COLOR_APP,
			COLOR_APP, 0, 0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x00, 0, 45, 320, 30, 0, 0, BAGL_FILL, 0xFFFFFF,
			COLOR_APP,
			BAGL_FONT_OPEN_SANS_SEMIBOLD_10_13PX | BAGL_FONT_ALIGNMENT_CENTER,
			0
		},
		"CONFIRM ACCOUNT",
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x00, 30, 106, 320, 30, 0, 0, BAGL_FILL, 0x999999,
			COLOR_BG_1, BAGL_FONT_OPEN_SANS_SEMIBOLD_8_11PX, 0
		},
		"ACCOUNT",
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x10, 30, 136, 260, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1, BAGL_FONT_OPEN_SANS_REGULAR_10_13PX, 0
		},
		(const char*) G_ram.ui_address_summary,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x11,  30, 159, 260,  30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1, BAGL_FONT_OPEN_SANS_REGULAR_10_13PX, 0
		},
		(const char*) G_ram.ui_address_summary,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_RECTANGLE | BAGL_FLAG_TOUCHABLE, 0x00, 40, 414, 115, 36, 0,
			18, BAGL_FILL, 0xCCCCCC, COLOR_BG_1,
			BAGL_FONT_OPEN_SANS_REGULAR_11_14PX | BAGL_FONT_ALIGNMENT_CENTER |
			BAGL_FONT_ALIGNMENT_MIDDLE, 0
		},
		"REJECT",
		0,
		0xB7B7B7,
		COLOR_BG_1,
		io_seproxyhal_touch_address_cancel,
		NULL,
		NULL
	},

	{
		{
			BAGL_RECTANGLE | BAGL_FLAG_TOUCHABLE, 0x00, 165, 414, 115, 36, 0,
			18, BAGL_FILL, 0x41ccb4, COLOR_BG_1,
			BAGL_FONT_OPEN_SANS_REGULAR_11_14PX | BAGL_FONT_ALIGNMENT_CENTER |
			BAGL_FONT_ALIGNMENT_MIDDLE, 0
		},
		"CONFIRM",
		0,
		0x3ab7a2,
		COLOR_BG_1,
		io_seproxyhal_touch_address_ok,
		NULL,
		NULL
	},
};

const bagl_element_t ui_idle_blue[] = {
	{
		{
			BAGL_RECTANGLE, 0x00, 0, 68, 320, 413, 0, 0, BAGL_FILL, COLOR_BG_1,
			0x000000, 0, 0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_RECTANGLE, 0x00, 0, 20, 320, 48, 0, 0, BAGL_FILL, COLOR_APP,
			COLOR_APP, 0, 0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x00, 0, 45, 320, 30, 0, 0, BAGL_FILL, 0xFFFFFF,
			COLOR_APP,
			BAGL_FONT_OPEN_SANS_SEMIBOLD_10_13PX | BAGL_FONT_ALIGNMENT_CENTER,
			0
		},
		"HYCON",
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_RECTANGLE | BAGL_FLAG_TOUCHABLE, 0x00, 264, 19, 56, 44, 0, 0,
			BAGL_FILL, COLOR_APP, COLOR_APP_LIGHT,
			BAGL_FONT_SYMBOLS_0 | BAGL_FONT_ALIGNMENT_CENTER |
			BAGL_FONT_ALIGNMENT_MIDDLE, 0
		},
		BAGL_FONT_SYMBOLS_0_DASHBOARD,
		0,
		COLOR_APP,
		0xFFFFFF,
		io_seproxyhal_touch_exit,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x00, 0, 270, 320, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1,
			BAGL_FONT_OPEN_SANS_LIGHT_16_22PX | BAGL_FONT_ALIGNMENT_CENTER, 0
		},
		"Open your wallet",
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x00, 0, 308, 320, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1,
			BAGL_FONT_OPEN_SANS_REGULAR_10_13PX | BAGL_FONT_ALIGNMENT_CENTER, 0
		},
		"Connect your Ledger Blue and open your",
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x00, 0, 331, 320, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1,
			BAGL_FONT_OPEN_SANS_REGULAR_10_13PX | BAGL_FONT_ALIGNMENT_CENTER, 0
		},
		"preferred wallet to view your accounts.",
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x00, 0, 450, 320, 14, 0, 0, 0, 0x999999,
			COLOR_BG_1,
			BAGL_FONT_OPEN_SANS_REGULAR_8_11PX | BAGL_FONT_ALIGNMENT_CENTER, 0
		},
		"Validation requests will show automatically.",
		10,
		0,
		COLOR_BG_1,
		NULL,
		NULL,
		NULL
	},
};

const bagl_element_t ui_details_blue[] = {
	{
		{
			BAGL_RECTANGLE, 0x00, 0, 68, 320, 413, 0, 0, BAGL_FILL, COLOR_BG_1,
			0x000000, 0, 0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_RECTANGLE, 0x00, 0, 20, 320, 48, 0, 0, BAGL_FILL, COLOR_APP,
			COLOR_APP, 0, 0
		},
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x01, 0, 45, 320, 30, 0, 0, BAGL_FILL, 0xFFFFFF,
			COLOR_APP,
			BAGL_FONT_OPEN_SANS_SEMIBOLD_10_13PX | BAGL_FONT_ALIGNMENT_CENTER,
			0
		},
		(const char*) G_ram.ui_address_summary,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_RECTANGLE | BAGL_FLAG_TOUCHABLE, 0x00, 0, 19, 50, 44, 0, 0,
			BAGL_FILL, COLOR_APP, COLOR_APP_LIGHT,
			BAGL_FONT_SYMBOLS_0 | BAGL_FONT_ALIGNMENT_CENTER |
			BAGL_FONT_ALIGNMENT_MIDDLE, 0
		},
		BAGL_FONT_SYMBOLS_0_LEFT,
		0,
		COLOR_APP,
		0xFFFFFF,
		ui_details_blue_back_callback,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x00, 30, 106, 320, 30, 0, 0, BAGL_FILL, 0x999999,
			COLOR_BG_1, BAGL_FONT_OPEN_SANS_SEMIBOLD_8_11PX, 0
		},
		"VALUE",
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x10, 30, 136, 260, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1, BAGL_FONT_OPEN_SANS_REGULAR_10_13PX, 0
		},
		(const char*) G_ram.ui_address_summary,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x11, 30, 159, 260, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1, BAGL_FONT_OPEN_SANS_REGULAR_10_13PX, 0
		},
		(const char*) G_ram.ui_address_summary,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x12, 30, 182, 260, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1, BAGL_FONT_OPEN_SANS_REGULAR_10_13PX, 0
		},
		(const char*) G_ram.ui_address_summary,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x13, 30, 205, 260, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1, BAGL_FONT_OPEN_SANS_REGULAR_10_13PX, 0
		},
		(const char*) G_ram.ui_address_summary,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x14, 30, 228, 260, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1, BAGL_FONT_OPEN_SANS_REGULAR_10_13PX, 0
		},
		(const char*) G_ram.ui_address_summary,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x15, 30, 251, 260, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1, BAGL_FONT_OPEN_SANS_REGULAR_10_13PX, 0
		},
		(const char*) G_ram.ui_address_summary,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x16, 30, 274, 260, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1, BAGL_FONT_OPEN_SANS_REGULAR_10_13PX, 0
		},
		(const char*) G_ram.ui_address_summary,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x17, 30, 297, 260, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1, BAGL_FONT_OPEN_SANS_REGULAR_10_13PX, 0
		},
		(const char*) G_ram.ui_address_summary,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x18, 30, 320, 260, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1, BAGL_FONT_OPEN_SANS_REGULAR_10_13PX, 0
		},
		(const char*) G_ram.ui_address_summary,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x19, 30, 343, 260, 30, 0, 0, BAGL_FILL, 0x000000,
			COLOR_BG_1, BAGL_FONT_OPEN_SANS_REGULAR_10_13PX, 0
		},
		(const char*) G_ram.ui_address_summary,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL
	},

	{
		{
			BAGL_LABELINE, 0x00, 0, 450, 320, 14, 0, 0, 0, 0x999999, COLOR_BG_1,
			BAGL_FONT_OPEN_SANS_REGULAR_8_11PX | BAGL_FONT_ALIGNMENT_CENTER, 0
		},
		"Review the whole value before continuing.",
		10,
		0,
		COLOR_BG_1,
		NULL,
		NULL,
		NULL
	},
};
#endif // TARGET_BLUE

/* ------------------------------------------------------------------------- */
/* ---                           MENU BUTTONS                            --- */
/* ------------------------------------------------------------------------- */

#if defined(TARGET_NANOS)
unsigned int ui_approval_nanos_button(unsigned int button_mask,
                                      unsigned int button_mask_counter) {
	switch (button_mask) {
	case BUTTON_EVT_RELEASED | BUTTON_LEFT:	// CANCEL
		io_seproxyhal_touch_tx_cancel(NULL);
		break;

	case BUTTON_EVT_RELEASED | BUTTON_RIGHT:	// OK
		io_seproxyhal_touch_tx_ok(NULL);
		break;
	}

	return 0;
}

unsigned int ui_address_nanos_button(unsigned int button_mask,
                                     unsigned int button_mask_counter) {
	switch (button_mask) {
	case BUTTON_EVT_RELEASED | BUTTON_LEFT:	// CANCEL
		io_seproxyhal_touch_address_cancel(NULL);
		break;

	case BUTTON_EVT_RELEASED | BUTTON_RIGHT:	// OK
		io_seproxyhal_touch_address_ok(NULL);
		break;
	}

	return 0;
}
#endif // TARGET_NANOS

#if defined(TARGET_BLUE)
unsigned int ui_address_blue_button(unsigned int button_mask,
                                    unsigned int button_mask_counter) {
	return 0;
}

unsigned int ui_approval_blue_button(unsigned int button_mask,
                                     unsigned int button_mask_counter) {
	return 0;
}

unsigned int ui_details_blue_button(unsigned int button_mask,
                                    unsigned int button_mask_counter) {
	return 0;
}

unsigned int ui_idle_blue_button(unsigned int button_mask,
                                 unsigned int button_mask_counter) {
 	return 0;
}
#endif // TARGET_BLUE

/* ------------------------------------------------------------------------- */
/* ---                           UI LOGICS                               --- */
/* ------------------------------------------------------------------------- */

unsigned int io_seproxyhal_touch_exit(const bagl_element_t *element) {
	// Go back to the dashboard
	os_sched_exit(0);
	return 0; // do not redraw the widget
}

unsigned int io_seproxyhal_touch_address_ok(const bagl_element_t *element) {
	uint32_t tx = set_result_public_key();
	G_io_apdu_buffer[tx++] = 0x90;
	G_io_apdu_buffer[tx++] = 0x00;

	// Send back the response, do not restart the event loop
	io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, tx);
	// Display back the original UX
	ui_idle();

	return 0; // do not redraw the widget
}

unsigned int io_seproxyhal_touch_address_cancel(const bagl_element_t *element) {
	G_io_apdu_buffer[0] = 0x69;
	G_io_apdu_buffer[1] = 0x85;

	// Send back the response, do not restart the event loop
	io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, 2);
	// Display back the original UX
	ui_idle();

	return 0; // do not redraw the widget
}

unsigned int io_seproxyhal_touch_tx_ok(const bagl_element_t *element) {
	uint8_t private_component[32];
	uint8_t signature[100];
	uint8_t signature_len;
	cx_ecfp_private_key_t private_key;
	uint32_t tx = 0;
	uint8_t r_length, s_length;
	uint8_t r_offset, s_offset;

	os_perso_derive_node_bip32(CX_CURVE_256K1, G_bip32_path, G_bip32_path_len,
		private_component, NULL);
	cx_ecfp_init_private_key(CX_CURVE_256K1, private_component, 32, &private_key);
	os_memset(private_component, 0, sizeof(private_component));

	unsigned int info = 0;
	signature_len = cx_ecdsa_sign(&private_key, CX_RND_RFC6979 | CX_LAST,
		CX_SHA256, G_ram.tx_hash, sizeof(G_ram.tx_hash), signature, 6+2*(32+1), &info);
	os_memset(&private_key, 0, sizeof(private_key));

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

unsigned int io_seproxyhal_touch_tx_cancel(const bagl_element_t *element) {
	G_io_apdu_buffer[0] = 0x69;
	G_io_apdu_buffer[1] = 0x85;

	// Send back the response, do not restart the event loop
	io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, 2);
	// Display back the original UX
	ui_idle();
	return 0; // do not redraw the widget
}

/* ------------------------------------------------------------------------- */
/* ---                           CALLBACKS                               --- */
/* ------------------------------------------------------------------------- */

#if defined(TARGET_BLUE)
bagl_element_callback_t ui_approval_blue_ok;
bagl_element_callback_t ui_approval_blue_cancel;

const bagl_element_t
	*ui_approval_blue_ok_callback(const bagl_element_t *element) {
		return ui_approval_blue_ok(element);
	}

const bagl_element_t
	*ui_approval_blue_cancel_callback(const bagl_element_t *element) {
		return ui_approval_blue_cancel(element);
	}

const bagl_element_t
	*ui_details_blue_back_callback(const bagl_element_t* element) {
		ui_details_back_callback();
		return 0;
	}
#endif // TARGET_BLUE

/* ------------------------------------------------------------------------- */
/* ---                           VARIABLES                               --- */
/* ------------------------------------------------------------------------- */

#if defined(TARGET_BLUE)
const char *ui_approval_blue_values[3];
enum UI_BLUE_APPROVAL_STATE ui_approval_blue_state;
const char *ui_details_title;
const char *ui_details_content;
callback_t ui_details_back_callback;
const char *const ui_approval_blue_details_name[][5] = {
	/*APPROVAL_TRANSACTION*/
	{
		"AMOUNT", "ADDRESS", "MAX FEES", "CONFIRM TRANSACTION",
		"Transaction details",
	}
};
bagl_element_t tmp_element;
#endif // TARGET_BLUE