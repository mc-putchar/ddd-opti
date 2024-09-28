NAME := InputOptiMock

SRCDIR := src
INCDIR := include
BINDIR := build
LIBDIR := lib
IMGUI_DIR := $(LIBDIR)/imgui
GLFW_DIR := $(LIBDIR)/glfw

SRCS := main InputOptiMock

IMGUI_URL = https://github.com/ocornut/imgui/archive/refs/heads/master.zip
GLFW_URL = https://github.com/glfw/glfw/archive/refs/heads/master.zip

CXX := c++
CXXFLAGS := -Wall -Wextra -std=c++11 -I$(IMGUI_DIR) -I$(GLFW_DIR)/include
CPPFLAGS := -I$(INCDIR)
LDFLAGS = -L$(GLFW_DIR)/lib -lglfw3
# LDFLAGS = -L$(GLFW_DIR)/lib -lglfw3 -lGL -lGLEW -limgui÷

.PHONY: all clean fclean re help

all: $(NAME)  # Compile all targets

$(NAME): $(IMGUI_DIR) $(GLFW_DIR) $(BINDIR)/main.o $(BINDIR)/InputOptiMock.o  # Ensure libraries are built first
	$(CXX) $(CPPFLAGS) $(BINDIR)/main.o $(BINDIR)/InputOptiMock.o -o $(NAME) $(LDFLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.cpp | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

# Rule to download and set up ImGui
$(IMGUI_DIR):
	mkdir -p $(IMGUI_DIR)
	curl -L $(IMGUI_URL) -o imgui.zip
	unzip imgui.zip -d $(LIBDIR)
	mv $(LIBDIR)/imgui-master/* $(IMGUI_DIR)/
	rm -rf imgui.zip $(LIBDIR)/imgui-master

# Rule to download and set up GLFW
$(GLFW_DIR):
	mkdir -p $(GLFW_DIR)
	curl -L $(GLFW_URL) -o glfw.zip
	unzip glfw.zip -d $(LIBDIR)
	mv $(LIBDIR)/glfw-master/* $(GLFW_DIR)/
	rm -rf glfw.zip $(LIBDIR)/glfw-master
	mkdir -p $(GLFW_DIR)/lib
	cd $(GLFW_DIR) && cmake . && make && cp src/*.a lib/  # Ensure you copy the correct library files

# Ensure libraries are downloaded and built before building the main program
$(BINDIR)/main.o: $(IMGUI_DIR) $(GLFW_DIR)
$(BINDIR)/InputOptiMock.o: $(IMGUI_DIR) $(GLFW_DIR)

clean:  # Remove compiled binary object files
	rm -fr $(BINDIR)

fclean: clean  # Remove all compiled binaries and libraries
	rm -fr $(LIBDIR) $(NAME)

re: fclean all  # Re-compile project

help:  # Print help on Makefile
	@awk 'BEGIN { \
	FS = ":.*#"; printf "Usage:\n\t$(COLOUR_CYNB)make $(COLOUR_MAGB)<target> \
	$(COLOUR_END)\n\nTargets:\n"; } \
	/^[a-zA-Z_0-9-]+:.*?#/ { \
	printf "$(COLOUR_MAGB)%-16s$(COLOUR_CYN)%s$(COLOUR_END)\n", $$1, $$2 } ' \
	Makefile
