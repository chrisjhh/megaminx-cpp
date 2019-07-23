# Megaminx solver

## Configure
```shell
mkdir _build
cd _build
cmake -G "Visual Studio 15 2017 Win64" ..
```
after the initial configure it can be configured again with just
```shell
cmake ..
```

## Build
```shell
cmake --build . --config Debug
```

## Testing
```shell
cd megaminx
ctest [-V]
```
