NAME := d3

DC := docker compose
CONF := setup.conf

.PHONY: all up down start stop ps logs clean

all: up
up:
	$(DC) up --build -d
down start stop ps logs:
	$(DC) $@
logs-front:
	$(DC) logs --tail=100 -f frontend
logs-back:
	$(DC) logs --tail=100 -f backend
clean: down
	docker system prune
fclean: clean
re: down up
