#ifndef FS_WATCHER_
#define FS_WATCHER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>

typedef struct {
    pthread_t thread;
    char *path;
    char should_close;
    uint32_t sleep_timer;

    struct timespec file_ts;

    void (*on_edited)(char*);
    void (*on_deleted)(char*);
} fs_watcher_t;

fs_watcher_t *fs_watcher_init(const char *path);
void *watch_path(void *fsw);
int fs_watcher_start(fs_watcher_t *fsw);
void fs_watcher_close(fs_watcher_t *fsw);

#endif // FS_WATCHER_

#ifdef FS_WATCHER_IMPLEMENTATION

fs_watcher_t *fs_watcher_init(const char *path)
{
    /* Get the status of the path before we allocate memory for a watcher */
    /* This allows us to return any errors if before any allocations      */
    struct stat file_stat = {0};
    int ret_stat = stat(path, &file_stat);

    if (ret_stat != 0)
    {
        fprintf(stderr, "stat() failed with error [%s]\n", strerror(errno));
        return NULL;
    }

    fs_watcher_t *fsw = calloc(1, sizeof(fs_watcher_t));
    fsw->path = calloc(strlen(path) + 1, sizeof(char));
    fsw->path = strcpy(fsw->path, path);

    fsw->should_close = 0;
    fsw->sleep_timer = 1000;

    fsw->file_ts = file_stat.st_ctim;

    fsw->on_edited  = NULL;
    fsw->on_deleted = NULL;

    return fsw;
}

void *watch_path(void *fsw)
{
    fs_watcher_t *watcher = (fs_watcher_t*)fsw;
    struct stat file_stat = {0};

    while (!watcher->should_close)
    {
        int ret_stat = stat(watcher->path, &file_stat);

        if (ret_stat != 0)
        {
            if (ret_stat == -1)
            {
                watcher->on_deleted(watcher->path);
            }
            else
            {
                fprintf(stderr, "stat() failed with error [%s]\n", strerror(errno));
            }
        }
        else
        {
            if (file_stat.st_ctim.tv_sec != watcher->file_ts.tv_sec)
            {
                watcher->on_edited(watcher->path);
                watcher->file_ts = file_stat.st_ctim;
            }
        }

        usleep(1000 * watcher->sleep_timer);
    }

    return NULL;
}

int fs_watcher_start(fs_watcher_t *fsw)
{
    return pthread_create(&fsw->thread, NULL, watch_path, fsw);
}

void fs_watcher_close(fs_watcher_t *fsw)
{
    fsw->should_close = 1;

    printf("waiting\n");
    pthread_join(fsw->thread, NULL);
    printf("joined\n");

    free(fsw->path);
    free(fsw);
}


#endif // FS_WATCHER_IMPLEMENTATION
