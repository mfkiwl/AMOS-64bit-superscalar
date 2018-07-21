/// Instantiates model and provides an environment around it

#include <stdio.h>
#include "Vamos_fifo.h"
#include "verilated.h"

int main(int argc, char **argv, char **env) {

  vluint64_t main_time;       // Current simulation time

  Verilated::commandArgs(argc, argv);
  Vamos_fifo* top = new Vamos_fifo;

  top->rst_ni = 0;
  top->flush_i = 0;
  top->testmode_i = 0;

  while (!Verilated::gotFinish()) {

    if (main_time > 40) {
        top->rst_ni = 1; // de-assert reset
    }
    // ATI
    // toggle clock
    if ((main_time % 10) == 0) {
      top->clk_i = 1;
    }

    if ((main_time % 10) == 6) {
      top->clk_i = 0;
    }

    // apply
    if ((main_time % 10) == 2) {
      top->data_i = main_time;
      top->push_i = rand() % 2;
    }

    // test
    if ((main_time % 10) == 9) {
      if (!top->empty_o) {
        top->pop_i = 1;
        printf("Pop from fifo: %x\n", top->data_o);
      } else {
        top->pop_i = 0;
      }
    }

    // advance simulation
    top->eval();
    main_time++;
  }
  delete top;
  exit(0);
  return 0;
}
