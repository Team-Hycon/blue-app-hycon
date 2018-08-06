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

#ifndef UI_H
#define UI_H

#include "os_io_seproxyhal.h"
#include "hycon_types.h"

#define COLOR_BG_1 0xF9F9F9
#define COLOR_APP 0x0ebdcf
#define COLOR_APP_LIGHT 0x87dee6
#define BAGL_FONT_OPEN_SANS_LIGHT_16_22PX_AVG_WIDTH 10
#define BAGL_FONT_OPEN_SANS_REGULAR_10_13PX_AVG_WIDTH 8

/* ------------------------------------------------------------------------- */
/* ---                           MENU BUTTONS                            --- */
/* ------------------------------------------------------------------------- */

unsigned int ui_approval_nanos_button(unsigned int button_mask,
	unsigned int button_mask_counter);
unsigned int ui_address_nanos_button(unsigned int button_mask,
	unsigned int button_mask_counter);
unsigned int ui_address_blue_button(unsigned int button_mask,
	unsigned int button_mask_counter);
unsigned int ui_approval_blue_button(unsigned int button_mask,
	unsigned int button_mask_counter);
unsigned int ui_details_blue_button(unsigned int button_mask,
	unsigned int button_mask_counter);
unsigned int ui_idle_blue_button(unsigned int button_mask,
	unsigned int button_mask_counter);

/* ------------------------------------------------------------------------- */
/* ---                           MENUS                                   --- */
/* ------------------------------------------------------------------------- */

#if defined(TARGET_NANOS)
extern const ux_menu_entry_t menu_about[];
extern const ux_menu_entry_t menu_main[];

extern const bagl_element_t ui_approval_nanos[11];
extern const bagl_element_t ui_address_nanos[7];
#endif // TARGET_NANOS

#if defined(TARGET_BLUE)
extern const bagl_element_t ui_approval_blue[28];
extern const bagl_element_t ui_address_blue[8];
extern const bagl_element_t ui_idle_blue[8];
extern const bagl_element_t ui_details_blue[16];
#endif // TARGET_BLUE

/* ------------------------------------------------------------------------- */
/* ---                           MENU PREPROS                            --- */
/* ------------------------------------------------------------------------- */

unsigned int ui_approval_prepro(const bagl_element_t *element);
unsigned int ui_address_prepro(const bagl_element_t *element);

#if defined(TARGET_BLUE)
unsigned int ui_address_blue_prepro(const bagl_element_t *element);
const bagl_element_t *ui_details_blue_prepro(const bagl_element_t *element);
const bagl_element_t* ui_approval_blue_prepro(const bagl_element_t *element);
void ui_approval_blue_init(void);
void ui_approval_transaction_blue_init(void);
void ui_approval_blue_1_details(const bagl_element_t *element);
void ui_approval_blue_2_details(const bagl_element_t *element);
void ui_approval_blue_3_details(const bagl_element_t *element);
void ui_details_init(const char *title, const char *content,
	callback_t back_callback);
const bagl_element_t *ui_menu_item_out_over(const bagl_element_t *element);
#endif // TARGET_BLUE

/* ------------------------------------------------------------------------- */
/* ---                           UI LOGICS                               --- */
/* ------------------------------------------------------------------------- */

#if defined(TARGET_BLUE)
unsigned int io_seproxyhal_touch_settings(const bagl_element_t *element);
#endif // TARGET_BLUE

unsigned int io_seproxyhal_touch_exit(const bagl_element_t *element);
unsigned int io_seproxyhal_touch_tx_ok(const bagl_element_t *element);
unsigned int io_seproxyhal_touch_tx_cancel(const bagl_element_t *element);
unsigned int io_seproxyhal_touch_address_ok(const bagl_element_t *element);
unsigned int io_seproxyhal_touch_address_cancel(const bagl_element_t *element);

void ui_idle(void);

/* ------------------------------------------------------------------------- */
/* ---                           CALLBACKS                               --- */
/* ------------------------------------------------------------------------- */

#if defined(TARGET_BLUE)
extern bagl_element_callback_t ui_approval_blue_ok;
extern bagl_element_callback_t ui_approval_blue_cancel;
const bagl_element_t
	*ui_approval_blue_ok_callback(const bagl_element_t *element);
const bagl_element_t
	*ui_approval_blue_cancel_callback(const bagl_element_t *element);
const bagl_element_t
	*ui_details_blue_back_callback(const bagl_element_t* element);
#endif // TARGET_BLUE

/* ------------------------------------------------------------------------- */
/* ---                           VARIABLES                               --- */
/* ------------------------------------------------------------------------- */

#if defined(TARGET_BLUE)
extern const char *ui_approval_blue_values[3];
extern enum UI_BLUE_APPROVAL_STATE ui_approval_blue_state;
extern const char *ui_details_title;
extern const char *ui_details_content;
extern callback_t ui_details_back_callback;
extern const char *const ui_approval_blue_details_name[][5];
extern bagl_element_t tmp_element;
#endif // TARGET_BLUE

#endif  // UI_H
