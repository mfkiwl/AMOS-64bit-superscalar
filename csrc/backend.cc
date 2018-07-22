// Copyright (c) 2018 AMOS Developers
#include "backend.h"

#include <vector>

#include "decode.h"

Backend::Backend() {
}

Backend::~Backend() {
}

void Backend::reset() {
}

void Backend::update() {
}

reg_t illegal_instruction(Backend* p, insn_t insn, reg_t pc) {
  // throw trap_illegal_instruction(0);
  return 0;
}

void Backend::build_opcode_map() {
  struct cmp {
    bool operator()(const insn_desc_t& lhs, const insn_desc_t& rhs) {
      if (lhs.match == rhs.match)
        return lhs.mask > rhs.mask;
      return lhs.match > rhs.match;
    }
  };

  std::sort(instructions.begin(), instructions.end(), cmp());

  for (size_t i = 0; i < OPCODE_CACHE_SIZE; i++)
    opcode_cache[i] = {0, 0, &illegal_instruction, &illegal_instruction};
}


void Backend::register_insn(insn_desc_t desc) {
  instructions.push_back(desc);
}

void Backend::register_base_instructions() {
  #define DECLARE_INSN(name, match, mask) \
    insn_bits_t name##_match = (match), name##_mask = (mask);
  #include "encoding.h"
  #undef DECLARE_INSN

  #define DEFINE_INSN(name) \
    REGISTER_INSN(this, name, name##_match, name##_mask)
  #include "insn_list.h"
  #undef DEFINE_INSN

  register_insn({0, 0, &illegal_instruction, &illegal_instruction});
  build_opcode_map();
}
