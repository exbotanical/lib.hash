# lib.hash

Collision-free hash tables for C.

Implemented as an open-addressed, double-hashed table.

For best performance, initialize with a prime number.

## Dynamic Linking

Linking to `lib.hash`:

```bash
# 1) include and use lib.hash in your project
# 2) generate object file for your project
gcc -I ../path/to/lib.hash -c main.c -o main.o
# 3) generate shared object file
make
# 4) link your project to lib.hash
gcc -o main main.o -L../path/to/lib.hash -lhash
# you may need to add the lib location to your PATH
```

## TODOs

- [ ] add docs
- [ ] remove dev deps from prod compilation steps
