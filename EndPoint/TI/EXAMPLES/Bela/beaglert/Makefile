# BeagleRT
# Low-latency, real-time audio and sensor processing on BeagleBone Black
# (c) 2015 Andrew McPherson, Victor Zappi, Giulio Moro
# Centre for Digital Music, Queen Mary University of London

# This Makefile is intended for use on the BeagleBone Black itself
# and not for cross-compiling

RM := rm -rf
STATIC_LIBS := ./libprussdrv.a ./libNE10.a
LIBS := -lrt -lnative -lxenomai -lsndfile 
# refresh library cache and check if libpd is there
TEST_LIBPD := $(shell ldconfig; ldconfig -p | grep "libpd\.so") 
ifeq ($(strip $(TEST_LIBPD)), )
else
# if libpd is there, link it in
  LIBS += -lpd -lpthread_rt
endif

CPP_FLAGS := -O3 -march=armv7-a -mtune=cortex-a8 -mfloat-abi=hard -mfpu=neon -ftree-vectorize 
C_FLAGS := $(CPP_FLAGS)

ifndef COMPILER
# check whether clang is installed
#    TEST_COMPILER := $(shell which clang)
#  ifneq ($(strip $(TEST_COMPILER)), )
#    # if it is installed, use it
#    COMPILER := clang
#  else
    COMPILER := gcc
#  endif
endif

ifeq ($(COMPILER), clang)
  CC=clang
  CXX=clang++
  CPP_FLAGS +=
  C_FLAGS +=
else
  CC=gcc
  CXX=g++
  CPP_FLAGS += --fast-math
  C_FLAGS += --fast-math
endif

INCLUDES := -I./source -I./include -I/usr/include/ne10 -I/usr/xenomai/include -I/usr/arm-linux-gnueabihf/include/xenomai/include -I/usr/arm-linux-gnueabihf/include/ne10

ASM_SRCS := $(wildcard source/*.S)
ASM_OBJS := $(addprefix build/source/,$(notdir $(ASM_SRCS:.S=.o)))
ASM_DEPS := $(addprefix build/source/,$(notdir $(ASM_SRCS:.S=.d)))

C_SRCS := $(wildcard source/*.c)
C_OBJS := $(addprefix build/source/,$(notdir $(C_SRCS:.c=.o)))
C_DEPS := $(addprefix build/source/,$(notdir $(C_SRCS:.c=.d)))

CPP_SRCS := $(wildcard source/*.cpp)
CPP_OBJS := $(addprefix build/source/,$(notdir $(CPP_SRCS:.cpp=.o)))
CPP_DEPS := $(addprefix build/source/,$(notdir $(CPP_SRCS:.cpp=.d)))

# Core BeagleRT sources
CORE_CPP_SRCS = $(filter-out core/default_main.cpp, $(wildcard core/*.cpp))
CORE_OBJS := $(addprefix build/core/,$(notdir $(CORE_CPP_SRCS:.cpp=.o)))
CORE_CPP_DEPS := $(addprefix build/core/,$(notdir $(CORE_CPP_SRCS:.cpp=.d)))

# Objects for a system-supplied default main() file, if the user
# only wants to provide the render functions.
DEFAULT_MAIN_CPP_SRCS := ./core/default_main.cpp
DEFAULT_MAIN_OBJS := ./build/core/default_main.o
DEFAULT_MAIN_CPP_DEPS := ./build/core/default_main.d

# all = build BeagleRT 
all: SYNTAX_FLAG :=
all: BeagleRT

# debug = buildBeagleRT debug
debug: CPP_FLAGS=-g
debug: C_FLAGS=-g
debug: all

# syntax = check syntax
syntax: SYNTAX_FLAG := -fsyntax-only
syntax: BeagleRT

# Rule for BeagleRT core C++ files
build/core/%.o: ./core/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: C++ Compiler'
	$(CXX) $(SYNTAX_FLAG) $(INCLUDES) $(CPP_FLAGS) -Wall -c -fmessage-length=0 -U_FORTIFY_SOURCE -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# Rule for user-supplied C++ files
build/source/%.o: ./source/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: C++ Compiler'
	$(CXX) $(SYNTAX_FLAG) $(INCLUDES) $(CPP_FLAGS) -Wall -c -fmessage-length=0 -U_FORTIFY_SOURCE -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# Rule for user-supplied C files
build/source/%.o: ./source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: C Compiler'
	$(CC) $(SYNTAX_FLAG) $(INCLUDES) $(C_FLAGS) -Wall -c -fmessage-length=0 -U_FORTIFY_SOURCE -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -std=c99 
	@echo 'Finished building: $<'
	@echo ' '

# Rule for user-supplied assembly files
build/source/%.o: ./source/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler'
	as  -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# This is a nasty kludge: we want to be able to optionally link in a default
# main file if the user hasn't supplied one. We check for the presence of the main()
# function, and conditionally call one of two recursive make targets depending on whether
# we want to link in the default main file or not. The kludge is the mess of a shell script
# line below. Surely there's a better way to do this?
BeagleRT: $(CORE_OBJS) $(ASM_OBJS) $(C_OBJS) $(CPP_OBJS) $(STATIC_LIBS)
	$(eval NEXT_TARGET := $(shell bash -c 'if [ `nm build/source/*.o | grep -w T | grep -w main | wc -l` == '0' ]; then echo "BeagleRT_with_main"; else echo "BeagleRT_without_main"; fi'))
	$(MAKE) $(NEXT_TARGET)
	@echo 'Finished building target: $@'
	@echo ' '
#	$(MAKE) --no-print-directory post-build	

# Rule for building BeagleRT including the default main file (no user-supplied main())
BeagleRT_with_main: $(CORE_OBJS) $(DEFAULT_MAIN_OBJS) $(ASM_OBJS) $(C_OBJS) $(CPP_OBJS) $(STATIC_LIBS)
	@echo 'Building target: $@'
	@echo 'Invoking: C++ Linker'
	$(CXX) $(SYNTAX_FLAG) -L/usr/xenomai/lib -L/usr/arm-linux-gnueabihf/lib -L/usr/arm-linux-gnueabihf/lib/xenomai -L/usr/lib/arm-linux-gnueabihf -pthread -Wpointer-arith -o "BeagleRT" $(CORE_OBJS) $(DEFAULT_MAIN_OBJS) $(ASM_OBJS) $(C_OBJS) $(CPP_OBJS) $(STATIC_LIBS) $(LIBS)

# Rule for building BeagleRT without the default main file (user-supplied main())
BeagleRT_without_main: $(CORE_OBJS) $(ASM_OBJS) $(C_OBJS) $(CPP_OBJS) $(STATIC_LIBS) 
	@echo 'Building target: $@'
	@echo 'Invoking: C++ Linker'
	$(CXX) $(SYNTAX_FLAG) -L/usr/xenomai/lib -L/usr/arm-linux-gnueabihf/lib -L/usr/arm-linux-gnueabihf/lib/xenomai -L/usr/lib/arm-linux-gnueabihf -pthread -Wpointer-arith -o "BeagleRT" $(CORE_OBJS) $(ASM_OBJS) $(C_OBJS) $(CPP_OBJS) $(STATIC_LIBS) $(LIBS)

# Other Targets:

# Remove the temporary user-supplied source, plus the objects built from them
sourceclean:
	-$(RM) source/* build/source/* BeagleRT
	-@echo ' '	

# Remove all the built objects, including the core BeagleRT objects
distclean:
	-$(RM) build/source/* $(CORE_OBJS) $(CORE_CPP_DEPS) $(DEFAULT_MAIN_OBJS) $(DEFAULT_MAIN_CPP_DEPS) BeagleRT
	-@echo ' '

# Remove only the user-generated objects
clean:
	-$(RM) build/source/* BeagleRT
	-@echo ' '

post-build:
# Nothing to do here (for now)

.PHONY: all clean distclean sourceclean dependents debug
.SECONDARY: post-build
