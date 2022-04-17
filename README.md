# File System Watcher

Very simple C implementation of the file system watcher class from C#

## Quick Start

Just copy filesystemwatcher.h into your project and include it like this:

``` c++
#define FS_WATCHER_IMPLEMENTATION
#include "filesystemwatcher.h"
```

This header file is a stb-style header-only library. For more information on this style see
[stb_howto.txt](https://github.com/nothings/stb/blob/master/docs/stb_howto.txt)

## Example

``` c++
#define FS_WATCHER_IMPLEMENTATION
#include "filesystemwatcher.h"


void event_on_edited(char *path)
{
    printf("Path [%s] edited\n", path);
}

void event_on_deleted(char *path)
{
    printf("Path [%s] deleted\n", path);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "File to watch must be provided\n");
        return -1;
    }

    fs_watcher_t *watcher = fs_watcher_init(argv[1]);

    if (!watcher)
        return -1;

    watcher->on_edited  = &event_on_edited;
    watcher->on_deleted = &event_on_deleted;

    fs_watcher_start(watcher);

    char c;
    while ((c = getchar()) != 'q')
    {
        printf("char c is [%c]\n", c);
    }

    fs_watcher_close(watcher);


    return 0;
}
```


