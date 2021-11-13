# hashed

## Dynamic Linking

Linking to `hashed`:

```bash
# 1) include and use hashed in your project
# 2) generate object file for your project
gcc -I ../path/to/hashed -c main.c -o main.o
# 3) generate shared object file
make
# 4) link your project to hashed
gcc -o main main.o -L../path/to/hashed -lhashed
# you may need to add the lib location to your PATH
```
