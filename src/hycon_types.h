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

#ifndef HYCON_TYPES_H
#define HYCON_TYPES_H

#ifdef UNIT_TEST
#include <stdint.h>
#include <stdlib.h>
#else
#include "os.h"
#endif

#include <stdbool.h>
#include "hycon_constants.h"

typedef uint8_t hycon_hash_t[32];
enum UI_STATE { UI_IDLE, UI_VERIFY };

typedef struct hycon_tx_s {
	uint8_t from[21];
	uint8_t to[21];
	uint64_t amount;
	uint64_t fee;
	uint32_t nonce;
	uint8_t signature[21];
	uint32_t recovery;
} hycon_tx;

#define HYCON_TX_INIT_ZERO {NULL, NULL, 0, 0, 0, NULL, 0}

#endif // HYCON_TYPES_H
