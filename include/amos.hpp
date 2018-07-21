// Copyright (c) 2018 AMOS Developers
#pragma once
#include <fesvr/htif.h>
#include <fesvr/context.h>
#include <devices.h>
#include <vector>
#include <simlib.hpp>

class amos : public htif_t
{
public:
  amos(int argc, char** argv,
           std::vector<std::pair<reg_t, mem_t*>> mems);
  ~amos();

  void build();
  int run();

private:
  void read_chunk(addr_t taddr, size_t len, void* dst);
  void write_chunk(addr_t taddr, size_t len, const void* src);
  void idle();
  void reset();

  Simulation *sim;
  Builder *builder;
  Cycle cycle_count = 0;

  void step();
  friend void sim_thread_main(void* arg);

  context_t* host;
  context_t target;

  bus_t bus;
  std::vector<std::pair<reg_t, mem_t*>> mems;

  size_t chunk_align() { return 8; }
  size_t chunk_max_size() { return 8; }
};
