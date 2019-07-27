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

## Installing sqlite
In a separate directory
```shell
git clone https://github.com/azadkuh/sqlite-amalgamation.git
cd sqlite-amalgamation
mkdir _build
cd _build
cmake ..
cmake-gui .
cmake --build . --config Release
cmake --install .
```

# Using sqlite
```cmake
find_package(Sqlite3)
target_link_libraries(${TARGET} sqlite3::sqlite3)
```