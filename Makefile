#*******************************************************************************
#   Ledger Blue
#   (c) 2016 Ledger
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#*******************************************************************************

ifeq ($(TRAVIS),)

ifeq ($(BOLOS_SDK),)
$(error BOLOS_SDK is not set)
endif
include $(BOLOS_SDK)/Makefile.defines

APPNAME = "Hycon"
COINID = 1397

APPVERSION_M = 0
APPVERSION_N = 2
APPVERSION_P = 0

APPVERSION=$(APPVERSION_M).$(APPVERSION_N).$(APPVERSION_P)
APP_LOAD_PARAMS = --path "44'/$(COINID)'" --appFlags 0x40 --curve secp256k1 --apdu $(COMMON_LOAD_PARAMS)
APP_DELETE_PARAMS =  --apdu $(COMMON_DELETE_PARAMS)

ifeq ($(TARGET_NAME),TARGET_BLUE)
ICONNAME=icon_blue.gif
else
ICONNAME=icon_nano.gif
endif

################
# Default rule #
################

all: default

############
# Platform #
############

DEFINES += OS_IO_SEPROXYHAL IO_SEPROXYHAL_BUFFER_SIZE_B=128
DEFINES += HAVE_BAGL HAVE_SPRINTF
DEFINES += PRINTF\(...\)=

DEFINES += HAVE_IO_USB HAVE_L4_USBLIB IO_USB_MAX_ENDPOINTS=7 IO_HID_EP_LENGTH=64 HAVE_USB_APDU
DEFINES += LEDGER_MAJOR_VERSION=$(APPVERSION_M) LEDGER_MINOR_VERSION=$(APPVERSION_N) LEDGER_PATCH_VERSION=$(APPVERSION_P) TCS_LOADER_PATCH_VERSION=0

DEFINES += UNUSED\(x\)=\(void\)x
DEFINES += APPVERSION=\"$(APPVERSION)\"

DEFINES += CUSTOM_IO_APDU_BUFFER_SIZE=\(255+5+64\)
DEFINES += HAVE_USB_CLASS_CCID

DEFINES += CX_COMLIANCE_141

##############
# Compiler #
##############

ifneq ($(BOLOS_ENV),)
$(info BOLOS_ENV=$(BOLOS_ENV))
CLANGPATH := $(BOLOS_ENV)/clang-arm-fropi/bin/
GCCPATH := $(BOLOS_ENV)/gcc-arm-none-eabi-5_3-2016q1/bin/
else
$(info BOLOS_ENV is not set: falling back to CLANGPATH and GCCPATH)
endif

ifeq ($(CLANGPATH),)
$(info CLANGPATH is not set: clang will be used from PATH)
endif

ifeq ($(GCCPATH),)
$(info GCCPATH is not set: arm-none-eabi-* will be used from PATH)
endif

CC := $(CLANGPATH)clang
CFLAGS += -O3 -Os

AS := $(GCCPATH)arm-none-eabi-gcc

LD := $(GCCPATH)arm-none-eabi-gcc
LDFLAGS += -O3 -Os
LDLIBS += -lm -lgcc -lc

# import rules to compile glyphs(/pone)
include $(BOLOS_SDK)/Makefile.glyphs

### variables processed by the common makefile.rules of the SDK to grab source files and include dirs
APP_SOURCE_PATH += src blake2b
SDK_SOURCE_PATH += lib_stusb qrcode

# U2F
DEFINES += USB_SEGMENT_SIZE=64
DEFINES += BLE_SEGMENT_SIZE=32 #max MTU
DEFINES += U2F_PROXY_MAGIC=\"HYCON\"
DEFINES += HAVE_IO_U2F HAVE_U2F

#use the SDK U2F+HIDGEN USB profile
SDK_SOURCE_PATH += lib_u2f lib_stusb_impl lib_stusb qrcode

load: all
	python -m ledgerblue.loadApp $(APP_LOAD_PARAMS)

delete:
	python -m ledgerblue.deleteApp $(APP_DELETE_PARAMS)

# import generic rules from the SDK
include $(BOLOS_SDK)/Makefile.rules

# add dependency on custom makefile filename
dep/%.d: %.c Makefile

endif

TEST_LDFLAGS = -O3 -Os -w
TEST_LDLIBS = -lgcc -lc -lcheck -pthread -lcheck_pic -lrt -lm -lsubunit
TEST_LD = gcc
TEST_SOURCE_PATH = tests
TEST_COMMON_SOURCES = blake2b/blake2b-ref.c src/hycon_helpers.c src/ram_variables.c

test:
	$(TEST_LD) -o $(TEST_SOURCE_PATH)/check_hash.o $(TEST_SOURCE_PATH)/check_hash.c $(TEST_COMMON_SOURCES) $(TEST_LDFLAGS) $(TEST_LDLIBS) -DUNIT_TEST
	$(TEST_LD) -o $(TEST_SOURCE_PATH)/check_protobuf.o $(TEST_SOURCE_PATH)/check_protobuf.c $(TEST_COMMON_SOURCES) $(TEST_LDFLAGS) $(TEST_LDLIBS) -DUNIT_TEST
	$(TEST_LD) -o $(TEST_SOURCE_PATH)/check_utils.o $(TEST_SOURCE_PATH)/check_utils.c $(TEST_COMMON_SOURCES) $(TEST_LDFLAGS) $(TEST_LDLIBS) -DUNIT_TEST

	$(TEST_SOURCE_PATH)/check_utils.o
	$(TEST_SOURCE_PATH)/check_protobuf.o
	$(TEST_SOURCE_PATH)/check_hash.o

test_clean:
	rm -rf $(TEST_SOURCE_PATH)/check_hash.o $(TEST_SOURCE_PATH)/check_protobuf.o
