cd ../../
if ! ./Build.sh; then
    exit 1
fi
cd Tests/TestApp

rm -rf Build
cmake -S . -B Build
make -j8 -C Build all