# Kompilacja programu

```sh
mkdir build         # stworzenie folderu build jezeli nie istnieje
make                # kompilacja
# make build_debug  # build z address sanitizerem
# make clean        # wyczyszczenie folderu ./build
```

# Wywołanie programu

```sh
./build/packet-transfer <message_length> <part_size> <message_offset> <template_file_path>
```
