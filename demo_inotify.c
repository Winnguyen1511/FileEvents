#include <sys/inotify.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <pthread.h>

#define BUF_LEN 10 * (sizeof(struct inotify_event) + 1024 + 1)

static void             /* Display information from inotify_event structure */
displayInotifyEvent(struct inotify_event *i)
{
    printf("    wd =%2d; ", i->wd);
    if (i->cookie > 0)
        printf("cookie =%4d; ", i->cookie);

    printf("mask = ");
    if (i->mask & IN_ACCESS)        printf("IN_ACCESS ");
    if (i->mask & IN_ATTRIB)        printf("IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE)   printf("IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE)        printf("IN_CREATE ");
    if (i->mask & IN_DELETE)        printf("IN_DELETE ");
    if (i->mask & IN_DELETE_SELF)   printf("IN_DELETE_SELF ");
    if (i->mask & IN_IGNORED)       printf("IN_IGNORED ");
    if (i->mask & IN_ISDIR)         printf("IN_ISDIR ");
    if (i->mask & IN_MODIFY)        printf("IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
    if (i->mask & IN_OPEN)          printf("IN_OPEN ");
    if (i->mask & IN_Q_OVERFLOW)    printf("IN_Q_OVERFLOW ");
    if (i->mask & IN_UNMOUNT)       printf("IN_UNMOUNT ");
    printf("\n");

    if (i->len > 0)
        printf("        name = %s\n", i->name);
}





int
main(int argc, char *argv[])
{
    int inotifyFd, wd, j, fd;
        char buf[BUF_LEN];// __attribute__ ((aligned(8)));
    char readBuf[2] ={0};
    ssize_t numRead;

    char *p;
    struct inotify_event *event;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        //usageErr("%s pathname...\n", argv[0]);
        return -1;

    inotifyFd = inotify_init();                 /* Create inotify instance */
    if (inotifyFd == -1)
        //errExit("inotify_init");
        return -1;
    /* For each command-line argument, add a watch for all events */

    //for (j = 1; j < argc; j++) {
        wd = inotify_add_watch(inotifyFd, argv[1], IN_OPEN);
        fd = open(argv[1], O_CREAT, O_RDONLY);
        if (wd == -1)
            //errExit("inotify_add_watch");
            return -1;
        printf("Watching %s using wd %d\n", argv[1], wd);
    //}
    int i = 0;
    int previous, now;
    numRead = read(inotifyFd, buf, BUF_LEN);
    lseek(fd, 0, SEEK_SET);
    read(fd, readBuf, 1);
        //sem_post(sem);
        if (numRead == 0)
            //fatal("read() from inotify fd returned 0!");
            return -1;
        if (numRead == -1)
            //errExit("read");
            return -1;
        //printf("Read %ld bytes from inotify fd\n", (long) numRead);

        /* Process all of the events in buffer returned by read() */

        
        for (p = buf; p < buf + numRead; ) {
            event = (struct inotify_event *) p;
            printf("%d: ", i++);
            displayInotifyEvent(event);
            printf("\n");
            p += sizeof(struct inotify_event) + event->len;
        }
    previous = atoi(readBuf);
    now = previous;
    printf("Read buf=%s\n", readBuf);
    printf("prev=%d, now=%d\n", previous, now);
    lseek(fd, 0, SEEK_SET);
    printf("\n");
    
    for (;;) {      
                                    /* Read events forever */
        //sem_wait(sem);
        numRead = read(inotifyFd, buf, BUF_LEN);
        //sem_post(sem);
        if (numRead == 0)
            //fatal("read() from inotify fd returned 0!");
            return -1;
        if (numRead == -1)
            //errExit("read");
            return -1;
        read(fd, readBuf, 1);
        lseek(fd, 0, SEEK_SET);
        printf("Read buf=%s\n", readBuf);
        //printf("Read %ld bytes from inotify fd\n", (long) numRead);

        /* Process all of the events in buffer returned by read() */

        now = atoi(readBuf);
        printf("prev=%d, now=%d\n", previous, now);
        if(previous != now)
        {
            
            previous = now;
        }
        for (p = buf; p < buf + numRead; ) {
                event = (struct inotify_event *) p;
                printf("%d: ", i++);
                displayInotifyEvent(event);
                printf("\n");
                p += sizeof(struct inotify_event) + event->len;
            }
           printf("\n");

    }

    //exit(EXIT_SUCCESS);
    return 0;
}