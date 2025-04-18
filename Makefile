NAME = ft_shield
CC = gcc
CFLAGS = -Wall -Wextra -Werror
SANITIZE_FLAGS = -fsanitize=address -g

SRCS_DIR = .
OBJS_DIR = objs

SRCS = $(wildcard $(SRCS_DIR)/*.c)
OBJS = $(patsubst $(SRCS_DIR)/%.c, $(OBJS_DIR)/%.o, $(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $@ $^
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

.PHONY: all clean fclean re sanitize