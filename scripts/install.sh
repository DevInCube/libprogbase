echo "Building and installing libprogbase..."
# clean and make build directory
rm ./build -rf ; mkdir ./build
# build nd install static library with cmake
cd ./build ; cmake .. ; make ; sudo make install ; cd ../
# cleanup build directory
rm ./build -rf
