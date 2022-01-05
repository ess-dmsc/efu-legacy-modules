# Copyright (C) 2022 European Spallation Source, ERIC. See LICENSE file

# efu-legacy-modules

This repository contains legacy efu modules that are no longer under
active development and are now provided as references without support.

These modules were removed from the
[event-formation-unit](https://github.com/ess-dmsc/event-formation-unit.git)
repo.

This code is not being built on a regular bases, nor are the unit tests
executed. In fact some binaries have been removed so do not expect full
functionality.

## build
To build these modules you should issue the following command from the
event-formation-unit build directory:

    build> cmake -DEFU_EXTERNAL_DIR="/tmp/efu-legacy-modules" ..

specifying the absolute path to the efu-legacy-modules folder.

then

    build> make
