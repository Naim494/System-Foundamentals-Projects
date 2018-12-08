// #include "client_registry.h"
// #include <semaphore.h>
// #include <stdlib.h>
// #include <sys/socket.h>
// #include "debug.h"
// #include "csapp.h"


// #define MAX_FDS 1024

// struct client_registry
// {
//     int *fd_buf;
//     int max_fds;
//     int front;
//     int rear;
//     int count;
//     sem_t mutex;
//     sem_t slots;
//     sem_t fds;
//     sem_t empty;
// };

// /*
//  * Initialize a new client registry.
//  *
//  * @return  the newly initialized client registry.
//  */
// CLIENT_REGISTRY *creg_init()
// {
//     debug("Initialize client registry (NAIM)");
//     CLIENT_REGISTRY *cr = Malloc(sizeof(CLIENT_REGISTRY));
//     cr -> fd_buf = Calloc(MAX_FDS, sizeof(int));
//     cr -> max_fds = MAX_FDS;
//     //cr -> front = cr -> rear = 0;
//     sem_init(&(cr -> mutex), 0, 1);
//     sem_init(&(cr -> slots), 0, MAX_FDS);
//     sem_init(&(cr ->fds), 0, 0);
//     sem_init(&(cr -> empty), 0, 1);
//     cr -> count = 0;

//     return cr;
// }

// /*
//  * Finalize a client registry.
//  *
//  * @param cr  The client registry to be finalized, which must not
//  * be referenced again.
//  */
// void creg_fini(CLIENT_REGISTRY *cr)
// {
//     Free(cr -> fd_buf);
//     Free(cr);
// }

// /*
//  * Register a client file descriptor.
//  *
//  * @param cr  The client registry.
//  * @param fd  The file descriptor to be registered.
//  */
// void creg_register(CLIENT_REGISTRY *cr, int fd)
// {
//     debug("Register file descriptor to CR");
//     P(&(cr -> slots));
//     P(&(cr -> mutex));
//     cr -> fd_buf[cr -> count] = fd;
//     V(&(cr -> mutex));
//     V(&(cr -> fds));

//     ++cr -> count;

//      // AFTER FIRST FD IS ADDED, DEC THE EMPTY SEPHAMORE
//     if(cr -> count == 1)
//         P(&(cr -> empty));
// }

// /*
//  * Unregister a client file descriptor, alerting anybody waiting
//  * for the registered set to become empty.
//  *
//  * @param cr  The client registry.
//  * @param fd  The file descriptor to be unregistered.
//  */
// void creg_unregister(CLIENT_REGISTRY *cr, int fd)
// {
//     debug("Unregister file descriptor from CR");
//     P(&(cr -> fds));
//     P(&(cr -> mutex));

//     for(int i = 0; i < MAX_FDS; i++)
//     {
//         if(cr -> fd_buf[i] == fd)
//             cr -> fd_buf[i] = -1;
//     }

//     V(&(cr -> mutex));
//     V(&(cr -> slots));

//     --cr -> count;

//     /* AFTER LAST FD IS REMOVED, INC EMPTY SEMAPHORE */
//     if(cr -> count == 0)
//         V(&(cr -> empty));
// }

// /*
//  * A thread calling this function will block in the call until
//  * the number of registered clients has reached zero, at which
//  * point the function will return.
//  *
//  * @param cr  The client registry.
//  */
// void creg_wait_for_empty(CLIENT_REGISTRY *cr)
// {
//     debug("Waiting for empty CR");
//     P(&(cr -> empty));
// }

// /*
//  * Shut down all the currently registered client file descriptors.
//  *
//  * @param cr  The client registry.
//  */
// void creg_shutdown_all(CLIENT_REGISTRY *cr)
// {
//     debug("Shutting down all client fds");
//     int i;

//     for(i = 0; i < cr -> count; i++) {
//         shutdown(cr -> fd_buf[i], SHUT_RDWR);
//     }

//     sem_init(&(cr -> slots), 0, MAX_FDS);
//     sem_init(&(cr ->fds), 0, 0);
//     sem_init(&(cr -> empty), 0, 1);
//     cr -> count = 0;
// }



