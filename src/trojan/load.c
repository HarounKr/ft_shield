#include "trojan.h"

// https://stackoverflow.com/questions/31526876/casting-when-using-dlsym
// https://stackoverflow.com/questions/34840510/how-to-replace-pthread-create-during-linkage

static create_type create = NULL;
static detach_type detach = NULL;

void load_create() {
    void *handle = dlopen("libpthread.so.0", RTLD_LAZY); // grep: /lib/x86_64-linux-gnu/libpthread.so.0: binary file matches
    char *err = dlerror();
    if (err) {
        printf("%s\n", err);
    }
    create = (create_type)dlsym(handle, "pthread_create");
    err = dlerror();
    if (err) {
        printf("%s\n", err);
    }
    dlclose(handle);
}

void load_detach() {
    void *handle = dlopen("libpthread.so.0", RTLD_LAZY);
    char *err = dlerror();
    if (err) {
        printf("%s\n", err);
    }
    create = (create_type)dlsym(handle, "pthread_detach");
    err = dlerror();
    if (err) {
        printf("%s\n", err);
    }
    dlclose(handle);
}

int p_create(CREATE_ARGS) {
    if (create == NULL) {
        load_create();
    }
    int ret = create(thread, attr, start_routine, arg);
    return ret;
}

int p_detach(pthread_t id) {
    if (detach == NULL) {
        load_detach();
    }
    int ret = detach(id);

    return ret;
}