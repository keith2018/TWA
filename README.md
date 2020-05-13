# TWA
Tiny WebAssembly Runtime with full [MVP](https://webassembly.github.io/spec/core/) features support

## Build

```
mkdir build
cd build
cmake ..
make
```

## Run

```
Usage:
  twa version
  twa run <file>
  twa run <file> --func <function> [args...]
  twa cmd
```

fibnacci

```
./twa run ../test/fib.c.wasm --func fib 30
```

command line

```
:init
:load <file>
<function> [args...]
```

## Test

```
cd test
python3 run-spec-test.py
```

## License
This code is licensed under the MIT License (see [LICENSE](LICENSE)).
