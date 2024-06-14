# Kompilacja i uruchomienie programu

## Dependencies

- C++17
- CMake >= 3.10
- Docker (opcjonalnie)

## Kompilacja z CMake

```sh
mkdir build
cd build
cmake ..

# Kopiowanie executable to root'a projektu
cd -
cp build/build/bin/checkers_client .
```

## Kompilacja w kontenerze Ubuntu

Plik executable zostanie skopiowany do CWD

```sh
./build-in-container
```
