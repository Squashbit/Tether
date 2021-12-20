rm -rf Build
cmake -S . -B Build
make -j8 -C Build all