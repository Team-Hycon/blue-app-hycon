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

#include "os.h"
#include "cx.h"

#include "os_io_seproxyhal.h"
#include "glyphs.h"

#include "hycon_api.h"
#include "hycon_constants.h"
#include "ram_variables.h"
#include "ui.h"

/* ------------------------------------------------------------------------- */
/* ---                            Application Entry                      --- */
/* ------------------------------------------------------------------------- */
void app_main() {
	volatile unsigned int rx = 0;
	volatile unsigned int tx = 0;
	volatile unsigned int flags = 0;

	// DESIGN NOTE: the bootloader ignores the way APDU are fetched. The only
	// goal is to retrieve APDU.
	// When APDU are to be fetched from multiple IOs, like NFC+USB+BLE, make
	// sure the io_event is called with a switch event, before the apdu is
	// replied to the bootloader. This avoid APDU injection faults.
	for (;;) {
		volatile unsigned short sw = 0;
		BEGIN_TRY {
			TRY {
				rx = tx;
				tx = 0; // ensure no race in catch_other if io_exchange throws
				        // an error
				rx = io_exchange(CHANNEL_APDU | flags, rx);
				flags = 0;

				// no apdu received, well, reset the session, and reset the
				// bootloader configuration
				if (rx == 0) {
					THROW(0x6982);
				}

				handleApdu(&flags, &tx);
			}
			CATCH_OTHER(e) {
				switch (e & 0xF000) {
				case 0x6000:
					// Wipe the transaction context and report the exception
					sw = e;
					break;
				case 0x9000:
					// All is well
					sw = e;
					break;
				default:
					// Internal error
					sw = 0x6800 | (e & 0x7FF);
					break;
				}
				// Unexpected exception => report
				G_io_apdu_buffer[tx] = sw >> 8;
				G_io_apdu_buffer[tx + 1] = sw;
				tx += 2;
			}
			FINALLY {
			}
		}
		END_TRY;
	}

	// return to dashboard:
	return;
}

/** display function */
void io_seproxyhal_display(const bagl_element_t *element) {
	io_seproxyhal_display_default((bagl_element_t *) element);
}

/* ------------------------------------------------------------------------- */
/* ---                            IO EVENT LOOP                          --- */
/* ------------------------------------------------------------------------- */
unsigned char io_event(unsigned char channel) {
	// nothing done with the event, throw an error on the transport layer if
	// needed

	// can't have more than one tag in the reply, not supported yet.
	switch (G_io_seproxyhal_spi_buffer[0]) {
	case SEPROXYHAL_TAG_FINGER_EVENT:
		UX_FINGER_EVENT(G_io_seproxyhal_spi_buffer);
		break;

	// power off if long push, else pass to the application callback if any
	case SEPROXYHAL_TAG_BUTTON_PUSH_EVENT: // for Nano S
		UX_BUTTON_PUSH_EVENT(G_io_seproxyhal_spi_buffer);
		break;
	case SEPROXYHAL_TAG_STATUS_EVENT:
		if (G_io_apdu_media == IO_APDU_MEDIA_USB_HID &&
			!(U4BE(G_io_seproxyhal_spi_buffer, 3) &
			SEPROXYHAL_TAG_STATUS_EVENT_FLAG_USB_POWERED)) {
				THROW(EXCEPTION_IO_RESET);
		}
		// no break is intentional
	default:
		UX_DEFAULT_EVENT();
		break;

	case SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT:
		UX_DISPLAYED_EVENT({});
		break;

	case SEPROXYHAL_TAG_TICKER_EVENT:
		UX_TICKER_EVENT(G_io_seproxyhal_spi_buffer, {
			if (UX_ALLOWED) {
				if (ux_step_count) {
					ux_step = (ux_step + 1) % ux_step_count; // prepare next screen
					UX_REDISPLAY(); // redisplay screen
				}
			}
		});
		break;
	}

	// close the event if not done previously (by a display or whatever)
	if (!io_seproxyhal_spi_is_status_sent()) {
		io_seproxyhal_general_status();
	}
	// command has been processed, DO NOT reset the current APDU transport
	return 1;
}

/* ------------------------------------------------------------------------- */
/* ---                  SOME KIND OF EVENT LOOP                          --- */
/* ------------------------------------------------------------------------- */
unsigned short io_exchange_al(unsigned char channel, unsigned short tx_len) {
	switch (channel & ~(IO_FLAGS)) {
	case CHANNEL_KEYBOARD:
		break;

	// multiplexed io exchange over a SPI channel and TLV encapsulated protocol
	case CHANNEL_SPI:
		if (tx_len) {
			io_seproxyhal_spi_send(G_io_apdu_buffer, tx_len);

			if (channel & IO_RESET_AFTER_REPLIED) {
				reset();
			}

			// nothing received from the master so far (it's a tx transaction)
			return 0;
		} else {
			return io_seproxyhal_spi_recv(G_io_apdu_buffer, sizeof(G_io_apdu_buffer),
				0);
		}

	default:
		THROW(INVALID_PARAMETER);
	}

	return 0;
}

void app_exit(void) {
	BEGIN_TRY_L(exit) {
		TRY_L(exit) {
			os_sched_exit(-1);
		}
		FINALLY_L(exit) {
		}
	}
	END_TRY_L(exit);
}

/** boot up the app and intialize it */
__attribute__((section(".boot"))) int main(void) {
	// exit critical section
	__asm volatile ("cpsie i");

	// ensure exception will work as planned
	os_boot();

	for (;;) {
		UX_INIT();
		BEGIN_TRY {
			TRY {
				io_seproxyhal_init();

				USB_power(0);
				USB_power(1);

				// show idle screen.
				ui_idle();

				// run main event loop.
				app_main();
			}
			CATCH_OTHER(e) {
				break;
			}
			FINALLY {
			}
		}
		END_TRY;
	}
	app_exit();

	return 0;
}
