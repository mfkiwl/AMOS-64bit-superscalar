// Copyright (c) 2018 AMOS Developers
#ifndef INCLUDE_FRONTEND_H_
#define INCLUDE_FRONTEND_H_
#include "simlib.hpp"

#include "devices.h"

/// contains a valid instruction word together with its PC
struct instr_t {
  reg_t pc;
  uint32_t instr_word;
};

/// This class manages AMOS instruction front-end
/// Ultimately it should model cache behaviour and branch-prediction
class frontend {
 public:
  // right now let the instruction be 32-bit
  ChannelTx<instr_t> instr;
  // address from which to start fetching instructions
  reg_t bootaddr;
  // bus interface, at the moment let this be an abstract interface
  bus_t instr_if;
  // reset module
  void reset();
  // sequential update
  void update();
 // private:
  // current program counter
  reg_t pc;
};

#endif  // INCLUDE_FRONTEND_H_
