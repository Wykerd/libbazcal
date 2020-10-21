# libbazcal

A native Bazcal library written in C.

# Build from source

Bazcal LF currently only supports GNU/Linux. Feel free to contribute if you wish for us to support other platforms.

Building from source requires [CMake](https://cmake.org) to generate the Makefiles.

## Dependencies to install

- sqlite3
- curl
- jansson

All other dependencies are built from source as git submodules.

## Toolchain

- A C Compiler (GCC or Clang)
- CMake

## Compiling

After installing the dependencies use CMake and build the project

```
git clone https://github.com/Wykerd/libbazcal.git
mkdir build
cd build
cmake ..
make
```

The library is output as `./libbazcal.so`

## Running

This repo also provides a simple cli to test the bot functionality, the name of this binary is `./bzcore`

# License

Copyright Daniel Wykerd 2020

```
Bazcal is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Bazcal is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with Bazcal.  If not, see <https://www.gnu.org/licenses/>.
```

**NOTE:** This software depends on other packages that may be licensed under different open source licenses.