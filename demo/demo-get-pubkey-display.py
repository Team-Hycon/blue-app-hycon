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

keypath = parse_bip32_path(bip32_path)
apdu = []
writeUint8BE(0xE0, apdu)	# CLA
writeUint8BE(0x02, apdu)	# INS (INS_GET_PUBLIC_KEY)
writeUint8BE(0x01, apdu)	# P1
writeUint8BE(0x00, apdu)	# P2
writeUint8BE(len(keypath), apdu)	# bip32 length
apdu.extend(keypath)	# keypath

dongle = getDongle(True)
result = dongle.exchange(bytes(apdu))

offset = 1 + result[0]
hex_address = result[offset + 1 : offset + 1 + result[offset]]
offset = offset + 1 + result[offset]
hycon_address = result[offset + 1 : offset + 1 + result[offset]]

print ("Public key = " + binascii.hexlify(bytearray(result[1 : 1 + result[0]])).decode('ascii'))
print ("Hex Address = 0x" + binascii.hexlify(hex_address).decode('ascii'))
print ("Hycon Address = " + (hycon_address).decode('ascii'))
