# Define the targets
CONTROLLER := ddd-backend

# Define directories
SRCDIR := src
INCDIR := include
BINDIR := build

# Source files for each target
SRCS = main DroneState Path WsServer
ifeq ($(OS),Windows_NT)
	SRCS += WinSerialHandler
	CONTROLLER := $(CONTROLLER).exe
	LDLIBS := -DCROW_ENABLE_COMPRESSION -lz -lwsock32 -lws2_32 -lpthread
	MKDIR := mkdir
	RM := del /s /q
else
	SRCS += SerialHandler
	LDLIBS :=
	MKDIR := mkdir -p
	RM := rm -fr
endif

# Compiler settings
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++14 -g
CPPFLAGS := -I$(INCDIR) -I/asio/include

# Color definitions for output
COLOUR_END := \033[0m
COLOUR_GREEN := \033[0;32m
COLOUR_RED := \033[0;31m
COLOUR_MAG := \033[0;35m
COLOUR_MAGB := \033[1;35m
COLOUR_CYN := \033[0;36m
COLOUR_CYNB := \033[1;36m

.PHONY: all controller run clean fclean re help

all: $(CONTROLLER)	# Compile all targets

run: all	# Run targets
	@./$(CONTROLLER)

$(CONTROLLER): $(SRCS:%=$(BINDIR)/%.o)	# Link and create executables
	$(CXX) $(CPPFLAGS) $(SRCS:%=$(BINDIR)/%.o) -o $@ $(LDLIBS)

$(BINDIR)/%.o: $(SRCDIR)/%.cpp | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(BINDIR):
	$(MKDIR) $(BINDIR)

clean:	# Clean up
	$(RM) $(BINDIR)

fclean: clean	# Full clean up
	$(RM) $(CONTROLLER)

re: fclean all

help:	# Print help on Makefile
	@awk 'BEGIN { \
	FS = ":.*#"; printf "Usage:\n\t$(COLOUR_CYNB)make $(COLOUR_MAGB)<target> \
	$(COLOUR_END)\n\nTargets:\n"; } \
	/^[a-zA-Z_0-9-]+:.*?#/ { \
	printf "$(COLOUR_MAGB)%-16s$(COLOUR_CYN)%s$(COLOUR_END)\n", $$1, $$2 } ' \
	Makefile
