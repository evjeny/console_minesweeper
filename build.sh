rf -rf build
mkdir build
cmake -S . -B "build"
make -C build/ -j 3