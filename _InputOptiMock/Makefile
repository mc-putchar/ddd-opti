
NAME := InputOptiMock

SRCDIR := src
INCDIR := include
BINDIR := build

SRCS := main InputOptiMock

CXX := c++
CXXFLAGS := -Wall -Wextra -std=c++11
CPPFLAGS := -I$(INCDIR)

COLOUR_END := \033[0m
COLOUR_GREEN := \033[0;32m
COLOUR_RED := \033[0;31m
COLOUR_MAG := \033[0;35m
COLOUR_MAGB := \033[1;35m
COLOUR_CYN := \033[0;36m
COLOUR_CYNB := \033[1;36m

.PHONY: all clean fclean re help

all: $(NAME)	# Compile all targets

$(NAME): $(SRCS:%=$(BINDIR)/%.o)
	$(CXX) $(CPPFLAGS) $(SRCS:%=$(BINDIR)/%.o) -o $(NAME) $(LDFLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.cpp | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

clean:	# Remove compiled binary object files
	rm -fr $(BINDIR)

fclean: clean	# Remove all compiled binaries
	rm -f $(NAME)

re: fclean all	# Re-compile project

help:			# Print help on Makefile
	@awk 'BEGIN { \
	FS = ":.*#"; printf "Usage:\n\t$(COLOUR_CYNB)make $(COLOUR_MAGB)<target> \
	$(COLOUR_END)\n\nTargets:\n"; } \
	/^[a-zA-Z_0-9-]+:.*?#/ { \
	printf "$(COLOUR_MAGB)%-16s$(COLOUR_CYN)%s$(COLOUR_END)\n", $$1, $$2 } ' \
	Makefile
