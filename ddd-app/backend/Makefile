# Define the targets
CLIENT := ddd-client
CLIENTKEY := ddd-clientkey
CONTROLLER := ddd-controller

# Define directories
SRCDIR := src
INCDIR := include
BINDIR := build

# Source files for each target
CLIENT_SRCS := client DroneState
CLIENTKEY_SRCS := clientKeyHook DroneState
CONTROLLER_SRCS = main DroneState Path SerialHandler

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
.PHONY: all client controller clientkey run clean fclean re help

# Compile all targets
# all: $(CONTROLLER) $(CLIENT) $(CLIENTKEY)
all: $(CONTROLLER)

# Compile individual targets
client: $(CLIENT)
clientkey: $(CLIENTKEY)
controller: $(CONTROLLER)


# Run targets
run: client controller
	./$(CONTROLLER) &
	./$(CLIENT)

# Link and create executables
$(CONTROLLER): $(CONTROLLER_SRCS:%=$(BINDIR)/%.o)
	$(CXX) $(CPPFLAGS) $(CONTROLLER_SRCS:%=$(BINDIR)/%.o) -o $(CONTROLLER)
	mkdir -p tmp

$(CLIENT): $(CLIENT_SRCS:%=$(BINDIR)/%.o)
	$(CXX) $(CPPFLAGS) $(CLIENT_SRCS:%=$(BINDIR)/%.o) -o $(CLIENT)

$(CLIENTKEY): $(CLIENTKEY_SRCS:%=$(BINDIR)/%.o)
	$(CXX) $(CPPFLAGS) $(CLIENTKEY_SRCS:%=$(BINDIR)/%.o) -o $(CLIENTKEY)

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
	rm -f $(CLIENT)
	rm -f $(CLIENTKEY)
	rm -fr tmp

re: fclean all

# Print help on Makefile
help:
	@awk 'BEGIN { \
	FS = ":.*#"; printf "Usage:\n\t$(COLOUR_CYNB)make $(COLOUR_MAGB)<target> \
	$(COLOUR_END)\n\nTargets:\n"; } \
	/^[a-zA-Z_0-9-]+:.*?#/ { \
	printf "$(COLOUR_MAGB)%-16s$(COLOUR_CYN)%s$(COLOUR_END)\n", $$1, $$2 } ' \
	Makefile
