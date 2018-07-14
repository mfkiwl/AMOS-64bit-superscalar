// Copyright (c) 2018 AMOS Developers
#pragma once
#include <cstdint>
#include <cassert>
#include <memory>
#include <vector>

/// A number of clock cycles.
///
/// Use this type whenever it is necessary to track the number of clock cycles
/// that have passed.
typedef uint64_t Cycle;

/// A basic channel slot.
///
/// Concrete channel slots inherit from this base class and override the virtual
/// functions with concrete implementations according to the contained type.
struct AbstractChannelSlot {
	virtual void transport() = 0;

	bool tx_valid = false;
	bool tx_ready = true;
	bool rx_valid = false;
	bool rx_ready = true;
};

/// A channel slot allocated in the simulation core.
template <typename T>
struct ChannelSlot : public AbstractChannelSlot {
	T tx_datum;
	T rx_datum;

	void transport() override {
		if (tx_valid && rx_ready) {
			tx_valid = false;
			tx_ready = true;
			rx_valid = true;
			rx_ready = false;
			std::swap(tx_datum, rx_datum);
		}
	}
};

/// A hardware component.
///
/// Behaviour and computation is represented as classes that implement this
/// interface. The simulation assumes ownership of all components and triggers
/// updates as appropriate.
struct AbstractComponent {
	virtual void update() = 0;
};

template <typename T>
struct Component : public AbstractComponent {
	T wrapped;

	Component(T wrapped) : wrapped(wrapped) {}

	void
	update() override {
		wrapped.update();
	}
};

/// A simulation core.
class Simulation {
public:
	void
	step () {
		// Simulate components.
		for (const auto &c : components) {
			c->update();
		}

		// Transport data.
		for (const auto &c : channels) {
			c->transport();
		}
	}

protected:
	friend class Builder;

	/// All channels in the simulation.
	std::vector<std::unique_ptr<AbstractChannelSlot>> channels;

	/// All components in the simulation.
	std::vector<std::unique_ptr<AbstractComponent>> components;
};

/// A sending end of a channel.
template <typename T>
class ChannelTx {
protected:
	friend struct Builder;

	ChannelSlot<T> *slot;
	ChannelTx(ChannelSlot<T> *slot) : slot(slot) {}

public:
	/// Check whether the channel is ready to accept data.
	inline bool
	ready () const {
		return slot->tx_ready;
	}

	inline
	operator bool () const {
		return ready();
	}

	/// Push a value into the channel to be moved at the next clock edge.
	inline void
	push (T datum) const {
		slot->tx_datum = datum;
		slot->tx_valid = true;
	}

	inline void
	operator = (T datum) const {
		push(datum);
	}
};

/// A receiving end of a channel.
template <typename T>
class ChannelRx {
protected:
	friend struct Builder;

	ChannelSlot<T> *slot;
	ChannelRx(ChannelSlot<T> *slot) : slot(slot) {}

public:
	/// Check whether the channel contains data.
	inline bool
	valid () const {
		return slot->rx_valid;
	}

	inline
	operator bool () const {
		return valid();
	}

	/// Pop a value from the channel.
	inline T
	pop () const {
		slot->rx_ready = true;
		return slot->rx_datum;
	}

	inline
	operator T () const {
		return pop();
	}
};

/// A communication channel pair.
///
/// Wraps around the sender and receiver resources allocated in the simulation
/// core that may be used to transport data each clock cycle.
template <typename T>
struct Channel {
	/// The sending part of the channel.
	ChannelTx<T> tx;
	/// the receiving part of the channel.
	ChannelRx<T> rx;
};

/// A helper class to prepare the simulation.
///
/// The builder class is used as a helper to assemble the simulation hierarchy
/// and associated resources. It wraps around a simulation core and allocates
/// resources as more modules and channels are added.
class Builder {
	/// The simulation the builder constructs.
	Simulation *sim;

public:
	Builder(Simulation *sim) : sim(sim) {}

	/// Create a communication channel pair.
	template <typename T> Channel<T>
	make_channel () {
		std::unique_ptr<ChannelSlot<T>> ptr(new ChannelSlot<T>());
		Channel<T> chan {
			.tx = ChannelTx<T>(ptr.get()),
			.rx = ChannelRx<T>(ptr.get())
		};
		sim->channels.push_back(std::move(ptr));
		return chan;
	}

	/// Add a component to the simulation.
	///
	/// The simulation allocates memory for the component and assumes ownership.
	/// Returns a pointer to the final component, which is usually not needed.
	template <typename T> inline T *
	add_component(T component) {
		std::unique_ptr<Component<T>> ptr(new Component<T>(component));
		auto p = &ptr->wrapped;
		sim->components.push_back(std::move(ptr));
		return p;
	}
};
