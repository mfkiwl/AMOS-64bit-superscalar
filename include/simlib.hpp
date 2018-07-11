// Copyright (c) 2018 AMOS Developers
#pragma once
#include <cstdint>
#include <cassert>

namespace simlib {

/// A number of clock cycles.
///
/// Use this type whenever it is necessary to track the number of clock cycles
/// that have passed.
typedef uint64_t Cycle;

/// An item passed through a channel, together with a cycle count.
///
/// The cycle count is used to implicitly keep track of clock cycles.
template <typename T>
struct ChannelDatum {
	typedef T Item;
	Item item;
	Cycle cycle;

	ChannelDatum(Item item, Cycle cycle) : item(item), cycle(cycle) {}
};

/// A FIFO channel
///
/// A channel emulates a first-in first-out means of communication between
/// different modules. Its semantics are the same as an actual FIFO in hardware,
/// but it serves a double-purpose as primitive to support multi-threaded
/// simulation.
template <typename T>
struct Channel {
	typedef T Item;
	typedef ChannelDatum<Item> Datum;

	/// Push an item into the FIFO.
	///
	/// If the FIFO is full, blocks until space becomes available.
	template <typename... Args> void
	push (Args... args) {
		assert(false && "not implemented");
	}

	/// Pop an item from the FIFO.
	///
	/// If the FIFO is empty, blocks until an item is available.
	Datum
	pop () {
		assert(false && "not implemented");
	}
};

} // namespace simlib
