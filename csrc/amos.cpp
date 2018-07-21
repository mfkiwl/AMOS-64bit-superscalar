// Copyright (c) 2018 AMOS Developers
#include <simlib.hpp>
#include <amos.hpp>
#include <vector>
#include <riscv/encoding.h>
#include <cstdio>

// TODO(zarubaf) Re-factor this to the appropriate place
#define PGSHIFT 12
const reg_t PGSIZE = 1 << PGSHIFT;

/// Constructor of AMOS testbench
// TODO(zarubaf) Re-factor this to the appropriate place, should also be renamed
amos::amos(int argc, char** argv,
           std::vector<std::pair<reg_t, mem_t*>> mems)
  : htif_t(argc, argv), mems(mems) {

  // connect memories to bus
  for (auto& x : mems) {
    bus.add_device(x.first, x.second);
  }

}

/// Destructor
amos::~amos() {

}

/// HTIF wants to read a chunk of memory
void amos::read_chunk(addr_t taddr, size_t len, void* dst) {
  assert(len == 8);
  bus.load(taddr, len, (uint8_t*) dst);
}

/// HTIF wants to write a chunk of memory
void amos::write_chunk(addr_t taddr, size_t len, const void* src) {
  assert(len == 8);
  bus.store(taddr, len, (uint8_t*) src);
}

/// HTIF reset command
void amos::reset() {

}

/// Build phase
void amos::build() {
  // construct the processor

}

/// Make a single simulation step
void amos::step() {
  // just write one to the main memory now, super crude but EOC works
  uint8_t p = 0x1;
  bus.store(0x80001000, sizeof(uint8_t), &p);
  // switch back to host, determine EOC and handle sys-calls with RISCV-PK
  host->switch_to();
}

/// Main simulation thread
void sim_thread_main(void* arg) {
  ((amos*)arg)->step();
}

/// Start simulation
int amos::run() {
  host = context_t::current();
  target.init(sim_thread_main, this);
  return htif_t::run();
}

/// HTIF told us that it is idle. Switch back to the main simulation
void amos::idle() {
  target.switch_to();
}

static void help() {
  // TODO(zarubaf) that needs major revision once we are adding actual arguments
  fprintf(stderr, "Wrong arguments\n");
  exit(1);
}

/// Make main memories (from Spike)
// This can parse a couple of options actually:
// -m<n>                 Provide <n> MiB of target memory [default 2048]
// -m<a:m,b:n,...>       Provide memory regions of size m and n bytes
static std::vector<std::pair<reg_t, mem_t*>> make_mems(const char* arg) {
  // handle legacy mem argument
  char* p;
  auto mb = strtoull(arg, &p, 0);
  if (*p == 0) {
    reg_t size = reg_t(mb) << 20;
    return std::vector<std::pair<reg_t, mem_t*>>(1, std::make_pair(reg_t(DRAM_BASE), new mem_t(size)));
  }

  // handle base/size tuples
  std::vector<std::pair<reg_t, mem_t*>> res;
  while (true) {
    auto base = strtoull(arg, &p, 0);
    if (!*p || *p != ':')
      help();
    auto size = strtoull(p + 1, &p, 0);
    if ((size | base) % PGSIZE != 0)
      help();
    res.push_back(std::make_pair(reg_t(base), new mem_t(size)));
    if (!*p)
      break;
    if (*p != ',')
      help();
    arg = p + 1;
  }
  return res;
}

int main (int argc, char **argv) {

  std::vector<std::pair<reg_t, mem_t*>> mems;
  // TODO(zarubaf) handle this with command line arguments
  mems = make_mems("2048");

  amos amos_tb(argc, argv, mems);
  amos_tb.build();
  return amos_tb.run();
}