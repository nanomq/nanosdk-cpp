# nanosdk-cpp
This project is a cpp wrapper for [NanoSDK](https://github.com/emqx/NanoSDK).

### DEPENDENCIES
- install depencies
```shell
apt update
apt install cmake g++
apt install libssl-dev
```
- build && install msquic

```shell
git clone https://github.com/nanomq/nanosdk-cpp.git
cd nanosdk-cpp
git submodule update --init --recursive
cd NanoSDK/extern/msquic/
mkdir build
cd build
cmake ..
make -j8
make install
```

### BUILD NANOSDK-CPP
```shell
cd nanosdk-cpp
mkdir build
cd build
cmake -DNNG_ENABLE_QUIC=ON ..
make -j8
```


### RUN
```shell
./sync_client
```
