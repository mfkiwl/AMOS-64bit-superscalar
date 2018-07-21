// Copyright (c) 2018 AMOS Developers
#include "amos.hpp"

#include <vector>
#include <cstdio>
#include <iostream>
#include <utility>

#include "simlib.hpp"
#include "riscv/encoding.h"

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

  sim = new Simulation();
  builder = new Builder(sim);
  cycle_count = 0;
}

/// Destructor
amos::~amos() {
  delete sim;
  delete builder;
}

/// HTIF wants to read a chunk of memory
void amos::read_chunk(addr_t taddr, size_t len, void* dst) {
  assert(len == 8);
  bus.load(taddr, len, reinterpret_cast<uint8_t*>(dst));
}

/// HTIF wants to write a chunk of memory
void amos::write_chunk(addr_t taddr, size_t len, const void* src) {
  assert(len == 8);
  bus.store(taddr, len, reinterpret_cast<const uint8_t *>(src));
}

/// HTIF reset command
void amos::reset() {
}

struct Producer {
  ChannelTx<uint64_t> out;
  uint64_t counter;

  void update() {
    if (out) {
      out = counter++;
    }
  }
};

struct Consumer  {
  ChannelRx<uint64_t> in;

  void update() {
    if (in) {
      std::cout << "consumed " << in.pop() << "\n";
    }
  }
};

struct Mangler {
  ChannelRx<uint64_t> in;
  ChannelTx<uint64_t> out;
  // sequential part
  void update() {
    if (in && out) {
      auto v = in.pop();
      out = v ^ (v << 13);
    }
  }
};

/// Build phase
void amos::build() {
  // construct the processor
  auto c0 = builder->make_channel<uint64_t>();
  auto c1 = builder->make_channel<uint64_t>();
  auto c2 = builder->make_channel<uint64_t>();
  builder->add_component(Producer { .out = c0.tx, .counter = 0 });
  builder->add_component(Mangler { .in = c0.rx, .out = c1.tx });
  builder->add_component(Mangler { .in = c1.rx, .out = c2.tx });
  builder->add_component(Consumer { .in = c2.rx });
}

/// Make a single simulation step
void amos::step() {
  // do one step of simulation here
  sim->step();

  // just write one to the main memory now, super crude but EOC works
  // at least for standard riscv-test binaries as they all have their
  // .tohost symbol at 0x80001000
  if (cycle_count > 10000) {
    uint8_t p = 0x1;
    bus.store(0x80001000, sizeof(uint8_t), &p);
  }

  // switch back to host, determine EOC and handle sys-calls with RISCV-PK
  // only switch back every 100th cycle
  if (cycle_count % 100 == 0)
    host->switch_to();

  // increase cycle count by one
  cycle_count++;
}

/// Main simulation thread
void sim_thread_main(void* arg) {
  while (true) {
    (reinterpret_cast<amos*>(arg))->step();
  }
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
    return std::vector<std::pair<reg_t, mem_t*>>
                        (1, std::make_pair(reg_t(DRAM_BASE), new mem_t(size)));
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

int main(int argc, char **argv) {
  std::vector<std::pair<reg_t, mem_t*>> mems;
  // TODO(zarubaf) handle this with command line arguments
  mems = make_mems("2048");

  amos amos_tb(argc, argv, mems);
  amos_tb.build();
  return amos_tb.run();
}
