#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/select.h>
#include <errno.h>
#include <string.h>
#include "gnl.h"
#include <stdbool.h>
// utiliser la bibliotheque libxkbcommon
const char *keymap[] = {
    [KEY_A] = "A", [KEY_B] = "B", [KEY_C] = "C", [KEY_D] = "D",
    [KEY_E] = "E", [KEY_F] = "F", [KEY_G] = "G", [KEY_H] = "H",
    [KEY_I] = "I", [KEY_J] = "J", [KEY_K] = "K", [KEY_L] = "L",
    [KEY_M] = "M", [KEY_N] = "N", [KEY_O] = "O", [KEY_P] = "P",
    [KEY_Q] = "Q", [KEY_R] = "R", [KEY_S] = "S", [KEY_T] = "T",
    [KEY_U] = "U", [KEY_V] = "V", [KEY_W] = "W", [KEY_X] = "X",
    [KEY_Y] = "Y", [KEY_Z] = "Z",
    [KEY_SPACE] = "SPACE",
    [KEY_ENTER] = "ENTER"
};

void read_event(char *event)
{
    char *device = malloc(sizeof(char *) * (strlen(event) + strlen("/dev/input/")) + 1);;
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
                    if (key) {
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


    //          PARTIE 1 : Recuperer l'info sur quel event fichier ce trouve le clavier 
    // ouvrir le fichier /proc/bus/input/devices
    // chercher dans ce dernier le fichier event correspondant a mon clavier.
    // Recuperner N: Name= et verifier si le mots KEYBOARD est dans le nom
    // Si Oui Allez dans H: Handlers et recuperer le numero d'event. 
    // Si non passez a la suite.
    //          PARTIE 2 : Lire le contenu de event
    // Ouvrir le fichier /dev/input/event
    // lire le contenu de ce dernier 
    // passer ce contenu a la fonction
    //          PARTIE 3 : Traduire ce contenu et le stocker dans un fichier en local
    read_event(parse_device_name());
    return 0;
}
