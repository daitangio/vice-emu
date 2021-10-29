#!/bin/bash
# sudo apt-get install -y subversion
 sudo apt-get install -y autoconf
 sudo apt-get install -y automake
 sudo apt-get install -y build-essential  # (contains gcc, make)
 sudo apt-get install -y byacc
 sudo apt-get install -y flex
 sudo apt-get install -y xa65             # needed to build the vsid driver
 sudo apt-get install -y libgtk-3-dev
 sudo apt-get install -y texinfo
 sudo apt-get install -y texlive-fonts-recommended
 sudo apt-get install -y dos2unix         # only used when doing `make dist`
 sudo apt-get install -y libpulse-dev     # for Pulse Audio sound support
 sudo apt-get install -y libasound2-dev   # for ALSA sound support
 sudo apt-get install -y libglew-dev      # for OpenGL hardware scaling support
 sudo apt-get install -y libgif-dev       # GIF screenshot support
 sudo apt-get install -y libjpeg-dev      # JPEG screenshot support
 sudo apt-get install -y libpcap-dev      # Ethernet support
 sudo apt-get install -y libpcap-dev        # for ethernet emulation
 sudo apt-get install -y libavcodec-dev     # for video capturing support
 sudo apt-get install -y libavformat-dev
 sudo apt-get install -y libswscale-dev
 sudo apt-get install -y libmp3lame-dev     # required for MP3 encoding
 sudo apt-get install -y libmpg123-dev      # required for MP3 decoding (and for
 sudo apt-get install -y libvorbis-de       # Ogg/Vorbis support
 sudo apt-get install -y libflac-dev        # FLAC support
 sudo apt-get install -y yasm               # required for --enable-static-ffmpeg

./autogen.sh

./configure --enable-native-gtk3ui 
make -j 2
sudo make install

# /usr/local/bin/xvic +sound +vsync
