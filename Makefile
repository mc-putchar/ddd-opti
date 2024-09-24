
NAME := ddd-opti

SRCDIR := src
INCDIR := include
BINDIR := build

SRCS := main

CXX := c++
CXXFLAGS := -Wall -Wextra
CPPFLAGS := -I$(INCDIR)

.PHONY: all clean fclean re help

all: $(NAME)

$(NAME): $(SRCS:%=$(BINDIR)/%.o)
	$(CXX) $(CPPFLAGS) $(SRCS:%=$(BINDIR)/%.o) -o $(NAME) $(LDFLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.cpp | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -fr $(BINDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

