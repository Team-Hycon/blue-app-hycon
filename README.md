# Blue app Hycon

Hycon wallet application framework for Ledger Nano S  
This follows the specification available in the `/doc` directory.

## Getting Started

Run `make load` to build and load the application onto the device.

## Prerequisites

Setup followings according to [Ledger documentation](https://ledger.readthedocs.io/en/2/userspace/getting_started.html).
1. ARM Toolchain
2. BOLOS SDK
3. BOLOS Python Loader

## Running the tests

You can run python scripts from `/demo` directory to test signing transactions or retrieving public key.  
For example: `python -m demo-get-pubkey`

## Issues & Pull Requests

If you have an issue, feel free to add it to the [Issues]() tab.
If you'd like to help us out, the [Pull Request]() tab is a great place to start.

**If you have found a security bug, please contact us at [security@glosfer.com](security@glosfer.com).**

## Authors

* **Dulguun Batmunkh** - *Initial work* <dulguun@glosfer.com>

## License

This project is licensed under the Apache 2.0 License - see the [LICENSE](LICENSE) file for details
