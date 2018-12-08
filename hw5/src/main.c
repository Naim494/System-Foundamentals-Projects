#include "debug.h"
#include "client_registry.h"
#include "transaction.h"
#include "store.h"
#include "string.h"
#include "signal.h"
#include "csapp.h"
#include "server.h"
#include <sys/types.h>
#include <sys/socket.h>


static void terminate(int status);

CLIENT_REGISTRY *client_registry;


//signal handler
void sighupHandler(int sig){
    terminate(EXIT_SUCCESS);
}




int main(int argc, char* argv[]){

    // Option processing should be performed here.
    // Option '-p <port>' is required in order to specify the port number
    // on which the server should listen.

    // Perform required initializations of the client_registry,
    // transaction manager, and object store.
    int listenfd = 0;

    if (argc == 3)
    {
        if(!strcmp(argv[1], "-p"))
        {
            listenfd = Open_listenfd(argv[2]);
        }
        else
        {
            fprintf(stderr, "usage: %s <port>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    client_registry = creg_init();
    trans_init();
    store_init();

    // TODO: Set up the server socket and enter a loop to accept connections
    // on this socket.  For each connection, a thread should be started to
    // run function xacto_client_service().  In addition, you should install
    // a SIGHUP handler, so that receipt of SIGHUP will perform a clean
    // shutdown of the server.

    Signal(SIGHUP, sighupHandler);

    int *connfdp;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;


    while(1){
        clientlen = sizeof(struct sockaddr_storage);
        connfdp = Malloc(sizeof(int));
        *connfdp = Accept(listenfd, (SA *) &clientaddr, &clientlen);
        Pthread_create(&tid, NULL, xacto_client_service, connfdp);
    }

    // fprintf(stderr, "You have to finish implementing main() "
    //     "before the Xacto server will function.\n");

    // terminate(EXIT_FAILURE);
}


/*
 * Function called to cleanly shut down the server.
 */
void terminate(int status) {
    // Shutdown all client connections.
    // This will trigger the eventual termination of service threads.
    creg_shutdown_all(client_registry);

    debug("Waiting for service threads to terminate...");
    creg_wait_for_empty(client_registry);
    debug("All service threads terminated.");

    // Finalize modules.
    creg_fini(client_registry);
    trans_fini();
    store_fini();

    debug("Xacto server terminating");
    exit(status);
}
