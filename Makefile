NAME := d3

DC := docker compose
CONF := setup.conf
OVERRIDE_FILE := -f compose.yaml -f compose.dev.yaml

.PHONY: all up down start stop ps logs clean

all: up
up: 
	git submodule update --init --recursive
	$(DC) $(if $(DEV),$(OVERRIDE_FILE)) up --build -d 
	# NOTE to run vite in dev: make DEV=1
down start stop ps logs:
	$(DC) $@
logs-front:
	$(DC) logs --tail=100 -f frontend
logs-back:
	$(DC) logs --tail=100 -f backend
clean: down
	docker system prune
	docker builder prune -a
fclean: clean
re: down up
