CC = gcc
CFLAGS = -lssl -lcrypto

SERVER_SRCS = server.c

SERVER_OBJS = $(SERVER_SRCS:.c=.o)

SERVER_BIN = server_exec

all: $(SERVER_BIN)

$(SERVER_BIN): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o

fclean: clean
	rm -f $(SERVER_BIN)

re: fclean all

.PHONY: all clean fclean re