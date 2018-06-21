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

/* ------------------------------------------------------------------------- */
/* ---                           MENU BUTTONS                            --- */
/* ------------------------------------------------------------------------- */

unsigned int ui_approval_nanos_button(unsigned int button_mask,
	unsigned int button_mask_counter);
unsigned int ui_address_nanos_button(unsigned int button_mask,
	unsigned int button_mask_counter);

/* ------------------------------------------------------------------------- */
/* ---                           MENUS                                   --- */
/* ------------------------------------------------------------------------- */

extern const ux_menu_entry_t menu_about[];
extern const ux_menu_entry_t menu_main[];

extern const bagl_element_t ui_approval_nanos[11];
extern const bagl_element_t ui_address_nanos[7];

/* ------------------------------------------------------------------------- */
/* ---                           MENU PREPROS                            --- */
/* ------------------------------------------------------------------------- */

unsigned int ui_approval_prepro(const bagl_element_t *element);
unsigned int ui_address_prepro(const bagl_element_t *element);

/* ------------------------------------------------------------------------- */
/* ---                           UI LOGICS                               --- */
/* ------------------------------------------------------------------------- */

unsigned int io_seproxyhal_touch_settings(const bagl_element_t *e);
unsigned int io_seproxyhal_touch_exit(const bagl_element_t *e);
unsigned int io_seproxyhal_touch_tx_ok(const bagl_element_t *e);
unsigned int io_seproxyhal_touch_tx_cancel(const bagl_element_t *e);
unsigned int io_seproxyhal_touch_address_ok(const bagl_element_t *e);
unsigned int io_seproxyhal_touch_address_cancel(const bagl_element_t *e);
unsigned int io_seproxyhal_touch_signMessage_ok(const bagl_element_t *e);
unsigned int io_seproxyhal_touch_signMessage_cancel(const bagl_element_t *e);

void ui_idle(void);

#endif  // UI_H
