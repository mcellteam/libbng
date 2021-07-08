# libBNG
BioNetGen library

Build:

    mkdir -p build/release
    cd build/release
    cmake ../../bng/ -DCMAKE_BUILD_TYPE=Release
    make -j


Running parser test:

    cd test
    python test.py
    
    
Build API tests (not automated yet):

    mkdir -p build/test
    cd build/test
    cmake ../../test/api -DCMAKE_BUILD_TYPE=Debug
    make -j
    
    
