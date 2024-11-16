NAME := d3

DC := docker compose
SETUP := setup.bat

.PHONY: all up down start stop ps logs logs-front logs-back clean fclean re setup-serial

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
setup-serial:
	$(info Running serial connection setup script requires administrator privileges)
	@cmd.exe /c "$(SETUP)"
