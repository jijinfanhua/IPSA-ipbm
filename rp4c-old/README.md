# Reconfigurable P4 (rP4)

Install dependencies

```sh
sudo apt install flex bison nlohmann-json3-dev
```

Build

```sh
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
```

Run

```sh
./bin/rp4 ../testcases/firewall-tmp.rp4
```
