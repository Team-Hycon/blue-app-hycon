"""
*******************************************************************************
*   Hycon Hardware Wallet Python API
*   (c) 2018 Dulguun Batmunkh
*   (c) 2018 Hycon
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*******************************************************************************
"""

#!/usr/bin/env python
from ledgerblue.comm import getDongle
from ledgerblue.commException import CommException
import binascii
from hyconHelpers import *
import tx_pb2

bip32_path = "44'/1397'/0'/0/0"

tx = tx_pb2.Tx()
tx.to = bytes(bytearray.fromhex("db6612b309d257a2aebd59b7445a900ed775d921"))
tx.amount = 10000
tx.fee = 100
tx.nonce = 1234

apdu = []
writeUint8BE(0xE0, apdu)	# CLA
writeUint8BE(0x04, apdu)	# INS (INS_SIGN)
writeUint8BE(0x00, apdu)	# P1
writeUint8BE(0x00, apdu)	# P2

keypath = parse_bip32_path(bip32_path)
writeUint8BE(len(keypath), apdu)	# bip32 length
apdu.extend(keypath)	# keypath
encodedTx = tx.SerializeToString()
writeUint8BE(len(encodedTx), apdu)	# encodedTx length
apdu.extend(encodedTx)	# encodedTx

dongle = getDongle(True)
result = dongle.exchange(bytes(apdu))

tx.recovery = 0
tx.signature = bytes(result[1: 1 + 64])
print ("Signed transaction = ", str(tx.SerializeToString()))
