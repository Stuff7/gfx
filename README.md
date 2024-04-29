# Build

```bash
cmake -S lib/glfw -B lib/glfw
make -C lib/glfw
cmake -S lib/glad -B lib/glad
make -C lib/glad

# For the LSP
cmake . -DCMAKE_EXPORT_COMPILE_COMMANDS=1
make
```

# Run

```bash
# Ignore memory leaks from unknown modules
export LSAN_OPTIONS=suppressions=lsan.supp
bin/gfx

# Run cmds.gdb for debugging logs
gdb --nx --batch --command=cmds.gdb --args bin/gfx
```
