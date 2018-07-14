// Copyright (c) 2018 AMOS Developers
#include <simlib.hpp>
#include <iostream>

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

	void update() {
		if (in && out) {
			auto v = in.pop();
			out = v ^ (v << 13);
		}
	}
};

int
main (int argc, char **argv) {

	Simulation sim;
	Builder builder(&sim);
	auto c0 = builder.make_channel<uint64_t>();
	auto c1 = builder.make_channel<uint64_t>();
	auto c2 = builder.make_channel<uint64_t>();
	builder.add_component(Producer { .out = c0.tx, .counter = 0 });
	builder.add_component(Mangler { .in = c0.rx, .out = c1.tx });
	builder.add_component(Mangler { .in = c1.rx, .out = c2.tx });
	builder.add_component(Consumer { .in = c2.rx });

	for (int i = 0; i < 10; i++)
		sim.step();

	return 0;
}
