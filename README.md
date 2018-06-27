# Blue app Hycon [![Build Status](https://travis-ci.org/Team-Hycon/blue-app-hycon.svg?branch=master)](https://travis-ci.org/Team-Hycon/blue-app-hycon)  
Hycon wallet application framework for Ledger Nano S  
This follows the specification available in the `/doc` directory.

## Getting Started

Run `make load` to build and load the application onto the device.

## Prerequisites

Setup followings according to [Ledger documentation](https://ledger.readthedocs.io/en/2/userspace/getting_started.html).
1. ARM Toolchain
2. BOLOS SDK
3. BOLOS Python Loader

And to run unit tests [check](https://libcheck.github.io/check/) needs to be installed.

## Running demos

You can run python scripts from `/demo` directory to test signing transactions or retrieving public key.  
For example:
```Shell
$ python -m demo-get-pubkey
HID => e002000015058000002c80000575800000000000000000000000
HID <= 200245c4d74989f42ebba2fa8ffaf61728aee261ea3e6567ca0562e1d547f16f9d14db6612b309d257a2aebd59b7445a900ed775d9209000
Public key 0245c4d74989f42ebba2fa8ffaf61728aee261ea3e6567ca0562e1d547f16f9d
Address 0xdb6612b309d257a2aebd59b7445a900ed775d920
```

## Testing

Run `make test` and see the result.

Example output (successful):
```Shell
tests/check_utils.o
Running suite(s): Utils
100%: Checks: 2, Failures: 0, Errors: 0
tests/check_protobuf.o
Running suite(s): Protobuf
100%: Checks: 6, Failures: 0, Errors: 0
tests/check_hash.o
Running suite(s): Hash
100%: Checks: 3, Failures: 0, Errors: 0
```
Run `make test_clean` after running tests to clean created files.

## Issues & Pull Requests

If you have an issue, feel free to add it to the [Issues](https://github.com/Team-Hycon/blue-app-hycon/issues) tab.
If you'd like to help us out, the [Pull Request](https://github.com/Team-Hycon/blue-app-hycon/pulls) tab is a great place to start.

**If you have found a security bug, please contact us at [security@glosfer.com](security@glosfer.com).**

## Authors

* **Dulguun Batmunkh** - *Initial work* <dulguun@glosfer.com>

## License

This project is licensed under the Apache 2.0 License - see the [LICENSE](LICENSE) file for details
