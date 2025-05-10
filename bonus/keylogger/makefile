CC = gcc
CFLAGS = -lssl -lcrypto

CLIENT_SRCS = key.c gnl.c gnl_utils.c
SERVER_SRCS = server.c gnl.c gnl_utils.c

CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)
SERVER_OBJS = $(SERVER_SRCS:.c=.o)

CLIENT_BIN = client_exec
SERVER_BIN = server_exec

all: $(CLIENT_BIN) $(SERVER_BIN)

$(CLIENT_BIN): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(SERVER_BIN): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o

fclean: clean
	rm -f $(CLIENT_BIN) $(SERVER_BIN)

re: fclean all

.PHONY: all clean fclean re
