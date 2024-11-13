NAME := d3

DC := docker compose
CONF := setup.conf

.PHONY: all up down start stop ps logs clean

all: up
setup: $(CONF)
up: $(CONF)
	$(DC) up --build -d
down start stop ps logs:
	$(DC) $@
clean: down
	docker system prune
fclean: clean
	$(RM) $(CONF)
$(CONF):
	touch $(CONF)
