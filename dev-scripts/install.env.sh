# BOLOS environment
cp ./.bash_profile ~/.
source ~/.bash_profile

# USB hardware rules
mv ./20-hw1.rules /etc/udev/rules.d/.
service udev restart

# <string.h> etc.
apt-get install build-essential

# To compile 32 bit binaries on 64 bit Linux version
apt-get install libx32gcc-4.8-dev
apt-get install libc6-dev-i386

# python bootloader
apt-get install python3-pip
pip3 install virtualenv
apt-get install libusb-1.0-0-dev
apt-get install libudev-dev

# libsecp256k1
pip install pkg-config
SECP_BUNDLED_EXPERIMENTAL=1 pip --no-cache-dir install --no-binary secp256k1 secp256k1

# check - unit test
apt-get install check

# protobuf
sudo apt-get install autoconf automake libtool curl make g++ unzip

git clone https://github.com/google/protobuf.git
cd protobuf
git submodule update --init --recursive
./autogen.sh

./configure
make
make check
make install
ldconfig # refresh shared library cache.

cd python
python setup.py build
python setup.py test
