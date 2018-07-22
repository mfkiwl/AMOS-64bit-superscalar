TARGET_EXEC ?= amos_tb

BUILD_DIR ?= ./build
SRC_DIRS ?= ./csrc
INC_DIRS ?= ./include ./riscv-isa-sim/softfloat
RTL_DIR ?= ./rtl

VERILATOR ?= verilator

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.cc -or -name *.s)
HDRS := $(shell find $(SRC_DIRS) $(INC_DIRS) -name *.hpp -or -name *.h)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d) $(VERILATOR_ROOT)/include $(INC_DIRS)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -pthread -Wall -std=c++11
LDFLAGS ?=

all: verilate $(BUILD_DIR)/simlib-test $(BUILD_DIR)/amos
	cd $(BUILD_DIR) && make -j -f Vamos_fifo.mk Vamos_fifo

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s $(HDRS)
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c $(HDRS)
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp $(HDRS)
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cc.o: %.cc $(HDRS)
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

verilate:
	$(VERILATOR) --Mdir $(BUILD_DIR) $(RTL_DIR)/amos_fifo.sv --cc --exe csrc/verilog_tb.cpp

$(BUILD_DIR)/simlib-test: $(BUILD_DIR)/csrc/simlib-test.cpp.o
	$(CXX) $(LDFLAGS) -o $@ $<

$(BUILD_DIR)/amos: $(BUILD_DIR)/csrc/amos.cpp.o $(BUILD_DIR)/csrc/devices.cpp.o  \
				   $(BUILD_DIR)/csrc/frontend.cc.o $(BUILD_DIR)/csrc/disasm.cc.o \
				   $(BUILD_DIR)/csrc/regnames.cc.o
	$(CXX) $(LDFLAGS) -lfesvr -o $@ $^

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
