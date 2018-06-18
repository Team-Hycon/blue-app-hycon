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

BIP32_PRIVDEV = 0x80000000

def writeUint8BE(value, buffer):
	buffer.append(value & 0xff)
	return buffer

def writeUint32BE(value, buffer):
	buffer.append((value >> 24) & 0xff)
	buffer.append((value >> 16) & 0xff)
	buffer.append((value >> 8) & 0xff)
	buffer.append(value & 0xff)
	return buffer

def parse_bip32_path(path):
	if len(path) == 0:
		return bytearray([0])
	result = []
	elements = path.split('/')
	for pathElement in elements:
		element = pathElement.split('\'')
		if len(element) == 1:
			writeUint32BE(int(element[0]), result)
		else:
			writeUint32BE(BIP32_PRIVDEV| int(element[0]), result)
	return bytearray([ len(elements) ] + result)
