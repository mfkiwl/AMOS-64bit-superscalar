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

/// update branch-prediction data
struct branchpredict_t {
  reg_t target_address;
  bool taken; // the branch is taken
};

/// Model the Return Address Stack
class RAS {
 public:
  // the RAS needs to figure out whether this is a call/return or nothing
  ChannelRx<instr_t> instr;
  // we've got a valid branch-prediction
  ChannelTx<branchpredict_t> bp;
};

/// Model a static predictor e.g.: bkwrd branches taken, fwd branches never taken
class StaticBP {
 public:
  // figure out whether we encountered a ctrl flow change
  ChannelRx<instr_t> instr;
};

/// This class manages AMOS instruction front-end
/// Ultimately it should model cache behaviour and branch-prediction
class Frontend {
 public:
  // right now let the instruction be 32-bit
  ChannelTx<instr_t> instr;
  // we mis-predicted
  ChannelRx<branchpredict_t> mispredict;
  // address from which to start fetching instructions
  reg_t bootaddr;
  // bus interface, at the moment let this be an abstract interface
  bus_t instr_if;
  // reset module
  void reset();
  // sequential update
  void update();
  // TODO(zarubaf) find out why this variable can't be set to private
  // private:
  // current program counter
  reg_t pc;
};

// different predictors should come later
#endif  // INCLUDE_FRONTEND_H_
