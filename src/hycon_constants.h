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

#ifndef HYCON_CONSTANTS_H
#define HYCON_CONSTANTS_H

/* ------------------------------------------------------------------------- */
/* ---                            Error constants                        --- */
/* ------------------------------------------------------------------------- */

#define HYCON_SW_PIN_REMAINING_ATTEMPTS 0x63C0
#define HYCON_SW_INCORRECT_LENGTH 0x6700
#define HYCON_SW_COMMAND_INCOMPATIBLE_FILE_STRUCTURE 0x6981
#define HYCON_SW_SECURITY_STATUS_NOT_SATISFIED 0x6982
#define HYCON_SW_CONDITIONS_OF_USE_NOT_SATISFIED 0x6985
#define HYCON_SW_INCORRECT_DATA 0x6A80
#define HYCON_SW_NOT_ENOUGH_MEMORY_SPACE 0x6A84
#define HYCON_SW_REFERENCED_DATA_NOT_FOUND 0x6A88
#define HYCON_SW_FILE_ALREADY_EXISTS 0x6A89
#define HYCON_SW_INCORRECT_P1_P2 0x6B00
#define HYCON_SW_INS_NOT_SUPPORTED 0x6D00
#define HYCON_SW_CLA_NOT_SUPPORTED 0x6E00
#define HYCON_SW_TECHNICAL_PROBLEM 0x6F00
#define HYCON_SW_OK 0x9000
#define HYCON_SW_MEMORY_PROBLEM 0x9240
#define HYCON_SW_NO_EF_SELECTED 0x9400
#define HYCON_SW_INVALID_OFFSET 0x9402
#define HYCON_SW_FILE_NOT_FOUND 0x9404
#define HYCON_SW_INCONSISTENT_FILE 0x9408
#define HYCON_SW_ALGORITHM_NOT_SUPPORTED 0x9484
#define HYCON_SW_INVALID_KCV 0x9485
#define HYCON_SW_CODE_NOT_INITIALIZED 0x9802
#define HYCON_SW_ACCESS_CONDITION_NOT_FULFILLED 0x9804
#define HYCON_SW_CONTRADICTION_SECRET_CODE_STATUS 0x9808
#define HYCON_SW_CONTRADICTION_INVALIDATION 0x9810
#define HYCON_SW_CODE_BLOCKED 0x9840
#define HYCON_SW_MAX_VALUE_REACHED 0x9850
#define HYCON_SW_GP_AUTH_FAILED 0x6300
#define HYCON_SW_LICENSING 0x6F42
#define HYCON_SW_HALTED 0x6FAA
#define HYCON_SW_APP_HALTED HYCON_SW_CONDITIONS_OF_USE_NOT_SATISFIED

/* ------------------------------------------------------------------------- */
/* ---                            APDU constants                         --- */
/* ------------------------------------------------------------------------- */

#define ISO_OFFSET_CLA 0x00
#define ISO_OFFSET_INS 0x01
#define ISO_OFFSET_P1 0x02
#define ISO_OFFSET_P2 0x03
#define ISO_OFFSET_LC 0x04
#define ISO_OFFSET_CDATA 0x05

#define INS_GET_PUBLIC_KEY 0x02
#define INS_SIGN 0x04
#define INS_GET_APP_CONFIGURATION 0x06
#define P1_CONFIRM 0x01
#define P1_NON_CONFIRM 0x00
#define P1_FIRST 0x00
#define P1_MORE 0x80

#define CLA 0xE0

#define MAX_BIP32_PATH 10

#endif  // HYCON_CONSTANTS_H
