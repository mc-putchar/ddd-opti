NAME := d3

DC := docker compose
SETUP := setup.bat
OVERRIDE_FILE := -f compose.yaml -f compose.dev.yaml

.PHONY: all up down start stop ps logs logs-front logs-back clean fclean re setup

all: up
# NOTE to run vite in dev: make DEV=1
up: 
	git submodule update --init --recursive
	$(DC) $(if $(DEV),$(OVERRIDE_FILE)) up --build -d 
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
setup:
	$(info Running serial connection setup script requires administrator privileges)
	@cmd.exe /c "$(SETUP)"
