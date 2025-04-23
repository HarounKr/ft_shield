NAME = client
CC = gcc
CFLAGS = -Wall -Wextra -Werror
SANITIZE_FLAGS = -fsanitize=address -g
LDFLAGS = -lssl -lcrypto -lpthread

SRCS_DIR = .
OBJS_DIR = objs

SRCS = $(wildcard $(SRCS_DIR)/*.c)
OBJS = $(patsubst $(SRCS_DIR)/%.c, $(OBJS_DIR)/%.o, $(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Compilation de $(NAME) terminée."

sanitize: CFLAGS += $(SANITIZE_FLAGS)
sanitize: re
	@echo "Compilation avec AddressSanitizer terminée."

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c | $(OBJS_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compilation de $< terminée."

$(OBJS_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(OBJS_DIR)
	@echo "Nettoyage des fichiers objets terminé."

fclean: clean
	@rm -f $(NAME)
	@echo "Nettoyage complet terminé."

re: fclean all

install_openssl:
	@echo "Installation de la librairie OpenSSL..."
	@sudo apt-get update && sudo apt-get install -y libssl-dev
	@echo "OpenSSL installé avec succès."

.PHONY: all clean fclean re sanitize install_openssl
