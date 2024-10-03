CLIENT := ddd-client
CLIENTKEY := ddd-clientkey
CONTROLLER := ddd-controller

SRCDIR := src
INCDIR := include
BINDIR := build
LIBUIOHOOCK_LIB_PATH = ./libuiohook/dist/lib
LIBUIOHOOCK_INCLUDE_PATH = ./libuiohook/dist/include

CONTROLLER_SRCS = controller DroneState
CLIENT_SRCS := client DroneState
CLIENTKEY_SRCS := clientKeyHook DroneState

CXX := c++
CXXFLAGS := -Wall -Wextra -std=c++11 -I$(LIBUIOHOOCK_INCLUDE_PATH)
CPPFLAGS := -I$(INCDIR)
LDFLAGS += -L$(LIBUIOHOOCK_LIB_PATH) -luiohook

COLOUR_END := \033[0m
COLOUR_GREEN := \033[0;32m
COLOUR_RED := \033[0;31m
COLOUR_MAG := \033[0;35m
COLOUR_MAGB := \033[1;35m
COLOUR_CYN := \033[0;36m
COLOUR_CYNB := \033[1;36m

.PHONY: all LIB client controller run clean fclean re help

all:  LIB $(CONTROLLER) $(CLIENT) $(CLIENTKEY)	# Compile all targets

LIB:
	git clone https://github.com/kwhat/libuiohook || (cd libuiohook && git pull)
	cd libuiohook && mkdir -p build && cd build && \
	cmake -S .. -D BUILD_SHARED_LIBS=ON -D BUILD_DEMO=ON -DCMAKE_INSTALL_PREFIX=../dist && \
	cmake --build . --parallel 2 --target install
client: $(CLIENT)	# Compile client
clientkey: $(CLIENTKEY)	# Compile client
controller: $(CONTROLLER)	# Compile controller

run: client controller
	./$(CONTROLLER) &
	./$(CLIENT)

$(CONTROLLER): $(CONTROLLER_SRCS:%=$(BINDIR)/%.o)
	$(CXX) $(CPPFLAGS) $(CONTROLLER_SRCS:%=$(BINDIR)/%.o) -o $(CONTROLLER) $(LDFLAGS)

$(CLIENT): $(CLIENT_SRCS:%=$(BINDIR)/%.o)
	$(CXX) $(CPPFLAGS) $(CLIENT_SRCS:%=$(BINDIR)/%.o) -o $(CLIENT) $(LDFLAGS)

$(CLIENTKEY): $(CLIENTKEY_SRCS:%=$(BINDIR)/%.o)
	$(CXX) $(CPPFLAGS) $(CLIENTKEY_SRCS:%=$(BINDIR)/%.o) -o $(CLIENTKEY) $(LDFLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.cpp | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

clean:	# Remove compiled binary object files
	rm -fr $(BINDIR)

fclean: clean	# Remove all compiled binaries
	rm -f $(CONTROLLER)
	rm -f $(CLIENT)
	rm -rf libuiohook

re: fclean all	# Re-compile project

help:			# Print help on Makefile
	@awk 'BEGIN { \
	FS = ":.*#"; printf "Usage:\n\t$(COLOUR_CYNB)make $(COLOUR_MAGB)<target> \
	$(COLOUR_END)\n\nTargets:\n"; } \
	/^[a-zA-Z_0-9-]+:.*?#/ { \
	printf "$(COLOUR_MAGB)%-16s$(COLOUR_CYN)%s$(COLOUR_END)\n", $$1, $$2 } ' \
	Makefile
