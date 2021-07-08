# libBNG
BioNetGen library

Build:

    mkdir -p build/release
    cd build/release
    cmake ../../bng/ -DCMAKE_BUILD_TYPE=Release
    make -j


Running tests:

    cd test
    python test.py
