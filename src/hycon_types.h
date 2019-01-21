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

#ifndef HYCON_TYPES_H
#define HYCON_TYPES_H

#ifdef UNIT_TEST
#include <stdint.h>
#include <stdlib.h>
#else
#include "os.h"
#include "cx.h"
#endif

#include <stdbool.h>
#include "hycon_constants.h"

#ifdef UNIT_TEST
#include "../blake2b/blake2.h"
#endif

typedef uint8_t hycon_hash_t[32];
enum UI_STATE { UI_IDLE, UI_VERIFY };
enum UI_BLUE_APPROVAL_STATE { APPROVAL_TRANSACTION, APPROVAL_MESSAGE };

typedef struct hycon_tx_s {
	uint8_t to[21];
	uint64_t amount;
	uint64_t fee;
	uint32_t nonce;
} hycon_tx;

typedef struct ram_s {
	hycon_hash_t tx_hash;

	// UI
	volatile char ui_amount[50];
	volatile char ui_fee[50];
	volatile char ui_full_address[50];
	volatile char ui_address_summary[32];
} ram;

typedef void (*callback_t)(void);
#endif // HYCON_TYPES_H
