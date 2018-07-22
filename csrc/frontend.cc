// Copyright (c) 2018 AMOS Developers
#include "frontend.h"

#include "simlib.hpp"

void Frontend::reset() {
  // start fetching from the boot address
  pc = bootaddr;
}

// sequential part
void Frontend::update() {
  uint32_t loaded_value [2];
  // load 64-bit from the bus
  instr_if.load(pc, 8, (uint8_t*) loaded_value);

  // check availability of channel
  if (instr) {
    // push a new fetch
    instr = instr_t { .pc = pc, .instr_word = loaded_value[0] };
    // continue linear fetching
    pc = pc + 4;
  }
}