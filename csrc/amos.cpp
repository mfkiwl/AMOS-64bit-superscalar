// Copyright (c) 2018 AMOS Developers
#include <simlib.hpp>
#include <amos.hpp>
#include <vector>

amos::amos(int argc, char** argv,
           std::vector<std::pair<reg_t, mem_t*>> mems)
  : htif_t(argc, argv) {

}

amos::~amos() {

}

/// HTIF wants to read a chunk of memory
void amos::read_chunk(addr_t taddr, size_t len, void* dst) {
  assert(len == 8);

}

/// HTIF wants to write a chunk of memory
void amos::write_chunk(addr_t taddr, size_t len, const void* src) {
  assert(len == 8);

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
  // switch back to host, determine EOC and handle sys-calls with PK
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

int main (int argc, char **argv) {

  std::vector<std::pair<reg_t, mem_t*>> mem;
  amos amos_tb(argc, argv, mem);
  amos_tb.build();
  return amos_tb.run();
}