// Copyright (c) 2018 AMOS Developers
#ifndef INCLUDE_BACKEND_H_
#define INCLUDE_BACKEND_H_

#include <vector>

#include "decode.h"
class Backend;

static inline void trace_opcode(Backend *p, insn_bits_t opc, insn_t insn) {
}
// typedef reg_t (*insn_func_t)(processor_t*, insn_t, reg_t);
typedef reg_t (*insn_func_t)(Backend *, insn_t, reg_t);

struct insn_desc_t {
  insn_bits_t match;
  insn_bits_t mask;
  insn_func_t rv32;
  insn_func_t rv64;
};

typedef struct {
  uint8_t prv;
  bool step;
  bool ebreakm;
  bool ebreakh;
  bool ebreaks;
  bool ebreaku;
  bool halt;
  uint8_t cause;
} dcsr_t;

typedef enum {
  ACTION_DEBUG_EXCEPTION = MCONTROL_ACTION_DEBUG_EXCEPTION,
  ACTION_DEBUG_MODE = MCONTROL_ACTION_DEBUG_MODE,
  ACTION_TRACE_START = MCONTROL_ACTION_TRACE_START,
  ACTION_TRACE_STOP = MCONTROL_ACTION_TRACE_STOP,
  ACTION_TRACE_EMIT = MCONTROL_ACTION_TRACE_EMIT
} mcontrol_action_t;

typedef enum {
  MATCH_EQUAL = MCONTROL_MATCH_EQUAL,
  MATCH_NAPOT = MCONTROL_MATCH_NAPOT,
  MATCH_GE = MCONTROL_MATCH_GE,
  MATCH_LT = MCONTROL_MATCH_LT,
  MATCH_MASK_LOW = MCONTROL_MATCH_MASK_LOW,
  MATCH_MASK_HIGH = MCONTROL_MATCH_MASK_HIGH
} mcontrol_match_t;

typedef struct {
  uint8_t type;
  bool dmode;
  uint8_t maskmax;
  bool select;
  bool timing;
  mcontrol_action_t action;
  bool chain;
  mcontrol_match_t match;
  bool m;
  bool h;
  bool s;
  bool u;
  bool execute;
  bool store;
  bool load;
} mcontrol_t;

// architectural state of a RISC-V hart
struct state_t {
  void reset(reg_t max_isa);

  static const int num_triggers = 4;

  reg_t pc;
  regfile_t<reg_t, NXPR, true> XPR;
  regfile_t<freg_t, NFPR, false> FPR;

  // control and status registers
  reg_t prv;    // TODO: Can this be an enum instead?
  reg_t misa;
  reg_t mstatus;
  reg_t mepc;
  reg_t mtval;
  reg_t mscratch;
  reg_t mtvec;
  reg_t mcause;
  reg_t minstret;
  reg_t mie;
  reg_t mip;
  reg_t medeleg;
  reg_t mideleg;
  uint32_t mcounteren;
  uint32_t scounteren;
  reg_t sepc;
  reg_t stval;
  reg_t sscratch;
  reg_t stvec;
  reg_t satp;
  reg_t scause;
  reg_t dpc;
  reg_t dscratch;
  dcsr_t dcsr;
  reg_t tselect;
  mcontrol_t mcontrol[num_triggers];
  reg_t tdata2[num_triggers];

  uint32_t fflags;
  uint32_t frm;
  bool serialized; // whether timer CSRs are in a well-defined state

  // When true, execute a single instruction and then enter debug mode.  This
  // can only be set by executing dret.
  enum {
      STEP_NONE,
      STEP_STEPPING,
      STEP_STEPPED
  } single_step;
};

/// Processor Back-end
class Backend {
 public:
  Backend();
  ~Backend();
  void reset();
  void update();
  state_t* get_state() { return &state; }
 // lets start off from a spike implementation
 // that hopefully reduces the amount of potential errors
 private:
  std::vector<insn_desc_t> instructions;
  void register_insn(insn_desc_t desc);
  void register_base_instructions();
  // hart state
  state_t state;
  void build_opcode_map();

  static const size_t OPCODE_CACHE_SIZE = 8191;
  insn_desc_t opcode_cache[OPCODE_CACHE_SIZE];
};

reg_t illegal_instruction(Backend* p, insn_t insn, reg_t pc);

#define REGISTER_INSN(proc, name, match, mask) \
  extern reg_t rv32_##name(Backend*, insn_t, reg_t); \
  extern reg_t rv64_##name(Backend*, insn_t, reg_t); \
  proc->register_insn((insn_desc_t){match, mask, rv32_##name, rv64_##name});

#endif  // INCLUDE_BACKEND_H_
