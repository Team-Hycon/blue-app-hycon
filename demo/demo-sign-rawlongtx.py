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

bip32_path = "44'/1397'/0'/0/0"

rawTx = bytes(bytearray.fromhex("0a14e7bc7874eebe21bb4608f4b6b5d52e29ab8fd6011214e161124d4aa41ca0df6bafecb0408971cff6c0961895bec1e6bae9a6db012095bec1e6bae9a6db0128959aef3a"))
keypath = parse_bip32_path(bip32_path)

apdu = []
writeUint8BE(0xE0, apdu)	# CLA
writeUint8BE(0x04, apdu)	# INS (INS_SIGN)
writeUint8BE(0x00, apdu)	# P1
writeUint8BE(0x00, apdu)	# P2
writeUint8BE(len(keypath) + len(rawTx), apdu)	# LC
apdu.extend(keypath)	# keypath
apdu.extend(rawTx)	# encodedTx

"""
EncodedTx = {
	toAddress : new Address("H497fHm8gbPZxaXySKpV17a7beYBF9Ut3"),
	amount : 123456789.123456789,
	nonce: 123456789,
	fee: 123456789.123456789
}
"""

dongle = getDongle(True)
result = dongle.exchange(bytes(apdu))

signature = bytes(result[1: 1 + len(result)])
recovery = result[0]
print ("Signature =", binascii.hexlify(signature).decode('ascii'))
print ("Recovery =", recovery)
