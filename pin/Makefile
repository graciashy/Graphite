# this gives us default build rules and dependency handling
SIM_ROOT ?= $(CURDIR)/..

LD_LIBS += -pthread -lcarbon_sim

CLEAN=$(findstring clean,$(MAKECMDGOALS))

ifeq ($(CLEAN),)
include $(SIM_ROOT)/common/Makefile.common
endif

# Use these files for auto targets
.SUFFIXES:  .o .c .h .cc

# Debug information in binary
DBG = # -ggdb

# Add other CXX Flags
CXXFLAGS += -g -c \
            -Wall -Werror -Wno-unknown-pragmas $(DBG) -O2 -MMD

# Use the pin flags for building
include $(SIM_ROOT)/Makefile.config

TARGET = ia32e

PIN_KIT = $(PIN_HOME)
include $(PIN_HOME)/source/tools/makefile.gnu.config

CXXFLAGS += $(PIN_CXXFLAGS)
CFLAGS += $(CXXFLAGS)

SOURCES = $(shell ls $(SIM_ROOT)/pin/*.cc $(SIM_ROOT)/pin/lite/*.cc)

OBJECTS = $(SOURCES:%.cc=%.o)

## build rules
TARGET = $(SIM_ROOT)/lib/pin_sim.so

all: $(TARGET)

$(SIM_ROOT)/lib/libcarbon_sim.a:
	$(MAKE) -C $(SIM_ROOT)/common

$(TARGET): $(PIN_LIBNAMES) $(SIM_ROOT)/lib/libcarbon_sim.a
$(TARGET): $(OBJECTS)
	@echo $(TARGET)
	$(CXX) $(PIN_LDFLAGS) $(LD_FLAGS) -o $@ $(OBJECTS) $(PIN_LIBS) $(LD_LIBS) $(DBG)

ifneq ($(CLEAN),)
clean:
	-rm -f $(TARGET) $(OBJECTS)
endif
