# Define the targets
CONTROLLER := ddd-backend

# Define directories
SRCDIR := src
INCDIR := include
BINDIR := build

# Source files for each target

CONTROLLER_SRCS = main DroneState Path SerialHandler WsServer

# Compiler settings
CXX := c++
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

# Compile all targets
all: $(CONTROLLER)

# Compile individual targets
controller: $(CONTROLLER)


# Run targets
run: client controller
	./$(CONTROLLER) &
	./$(CLIENT)

# Link and create executables
$(CONTROLLER): $(CONTROLLER_SRCS:%=$(BINDIR)/%.o)
	$(CXX) $(CPPFLAGS) $(CONTROLLER_SRCS:%=$(BINDIR)/%.o) -o $(CONTROLLER)

# Compile object files
$(BINDIR)/%.o: $(SRCDIR)/%.cpp | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

# Clean up
clean: 
	rm -fr $(BINDIR)

fclean: clean
	rm -f $(CONTROLLER)

re: fclean all

# Print help on Makefile
help:
	@awk 'BEGIN { \
	FS = ":.*#"; printf "Usage:\n\t$(COLOUR_CYNB)make $(COLOUR_MAGB)<target> \
	$(COLOUR_END)\n\nTargets:\n"; } \
	/^[a-zA-Z_0-9-]+:.*?#/ { \
	printf "$(COLOUR_MAGB)%-16s$(COLOUR_CYN)%s$(COLOUR_END)\n", $$1, $$2 } ' \
	Makefile
