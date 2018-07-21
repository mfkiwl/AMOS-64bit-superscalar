# AMOS

## Getting Started

Install [Verilator](https://www.veripool.org/projects/verilator/wiki/Installing).
Install [riscv-fesvr](https://github.com/riscv/riscv-fesvr.git).

```
make
```

We provide the following targets:

- `./build/Vamos_fifo`: Verilog simulation
- `./build/amos`: Main simulation binary, fails all tests for the moment
- `./build/simlib-test`: Unit test for the simulation library

## Styleguide

See [style-guidlines](https://github.com/pulp-platform/style-guidelines)
