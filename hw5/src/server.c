// #include "server.h"
// #include <stdlib.h>
// #include "csapp.h"
// #include "transaction.h"
// #include "protocol.h"


//   // Thread function for the thread that handles client requests.
//   // *
//   // * @param  Pointer to a variable that holds the file descriptor for
//   // * the client connection.  This pointer must be freed once the file
//   // * descriptor has been retrieved.

// void *xacto_client_service(void *arg)
// {
//     int *connfdp = (int*)arg;
//     free(arg);

//     Pthread_detach(pthread_self());

//     creg_register(client_registry, *connfdp);

//     TRANSACTION *trans = trans_create();

//     XACTO_PACKET *pkt = Malloc(sizeof(XACTO_PACKET));
//     void **datap = NULL;

//     while(1)
//     {
//         if(proto_recv_packet(*connfdp, pkt, datap) == 0)
//         {
//             if(pkt -> type == XACTO_PUT_PKT)
//             {

//             }
//             else if(pkt -> type == XACTO_GET_PKT)
//             {

//             }
//             else if(pkt -> type == XACTO_COMMIT_PKT)
//             {

//             }
//         }
//     }



//     return (void *)NULL;
// }