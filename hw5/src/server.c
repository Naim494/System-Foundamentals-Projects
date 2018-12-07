#include "server.h"
#include <stdlib.h>
#include "csapp.h"
#include "protocol.h"
#include "debug.h"
#include "store.h"
#include "time.h"

CLIENT_REGISTRY *client_registry;

XACTO_PACKET *getReplyPkt(int status)
{
    XACTO_PACKET *reply = Malloc(sizeof(XACTO_PACKET));
    reply -> type = XACTO_REPLY_PKT;
    reply -> status = status;
    int sec = time(NULL);
    reply -> timestamp_sec = sec;
    reply -> timestamp_nsec = sec * 1000000000;
    reply -> size = 0;
    reply -> null = 0;

    return reply;
}

XACTO_PACKET *getDataPkt(int status)
{
    XACTO_PACKET *data = Malloc(sizeof(XACTO_PACKET));
    data -> type = XACTO_DATA_PKT;
    data -> status = status;
    int sec = time(NULL);
    data -> timestamp_sec = sec;
    data -> timestamp_nsec = sec * 1000000000;

    return data;
}


  // Thread function for the thread that handles client requests.
  // *
  // * @param  Pointer to a variable that holds the file descriptor for
  // * the client connection.  This pointer must be freed once the file
  // * descriptor has been retrieved.

void *xacto_client_service(void *arg)
{
    int connfdp = *(int*)arg;
    debug("[%d] Starting client service", connfdp);
    free(arg);

    Pthread_detach(pthread_self());

    creg_register(client_registry, connfdp);

    TRANSACTION *trans = trans_create();

    XACTO_PACKET *pkt = Malloc(sizeof(XACTO_PACKET));
    void **datap = Malloc(sizeof(void*));

    char *key = Malloc(sizeof(char));
    char *value = Malloc(sizeof(char));

    while(1)
    {
        /* RECEIVE REQUEST PACKET*/
        if(proto_recv_packet(connfdp, pkt, datap) == 0)
        {
            /* PUT REQ */
            if(pkt -> type == XACTO_PUT_PKT)
            {
                debug("[%d] PUT packet received", connfdp);

                /* RECEIVE KEY DATA PKT */
                if(proto_recv_packet(connfdp, pkt, datap) == 0)
                {
                    key = (char*)(*datap);
                    debug("[%d] Received key, size %d", connfdp, pkt -> size);
                    debug("key -> %s", key);
                    int key_sz = pkt -> size;

                    /* RECEIVE VALUE DATA PKT */
                    if(proto_recv_packet(connfdp, pkt, datap) == 0)
                    {
                        value = (char*)*datap;
                        debug("[%d] Received value, size %d", connfdp, pkt -> size);

                        /* CREATE BLOB & KEY FOR KEY VAL */
                        BLOB* key_blob = blob_create(key, key_sz);
                        KEY* key_key = key_create(key_blob);

                        /* CREATE BLOB FOR VALUE VAL */
                        BLOB* value_blob = blob_create(value, pkt -> size);

                        /* PUT KEY-VALUE PAIR IN STORE */
                        TRANS_STATUS status = store_put(trans, key_key, value_blob);

                        /* SEND REPLY PKT */
                        XACTO_PACKET *reply = getReplyPkt(status);
                        proto_send_packet(connfdp, reply, (void*)NULL);

                        free(reply);

                        store_show();
                        trans_show_all();
                    }
                }
            }
            /* GET REQ */
            else if(pkt -> type == XACTO_GET_PKT)
            {
                debug("[%d] GET packet received", connfdp);

                /* RECEIVE KEY DATA PKT */
                if(proto_recv_packet(connfdp, pkt, datap) == 0)
                {
                    key = (char*)*datap;
                    debug("[%d] Received key, size %d", connfdp, (int)pkt -> size);
                    debug("key -> %s , fd -> %d", key, connfdp);

                    /* CREATE BLOB & KEY FOR KEY VAL */
                    BLOB* key_blob = blob_create(key, (int)pkt -> size);
                    KEY* key_key = key_create(key_blob);

                    /* GET VALUE FROM STORE */
                    BLOB **valuep = Malloc(sizeof(BLOB*));
                    TRANS_STATUS status = store_get(trans, key_key, valuep);

                    /* SEND REPLY PKT */
                    XACTO_PACKET *reply = getReplyPkt(status);
                    proto_send_packet(connfdp, reply, NULL);

                    XACTO_PACKET *data_pkt = getDataPkt(status);

                    if(*valuep == NULL)
                    {
                        debug("No value found for key");
                        data_pkt -> null = 1;
                        proto_send_packet(connfdp, data_pkt, (void*)NULL);
                    }
                    else
                    {
                        debug("Found value for key");
                        data_pkt -> size = (*valuep) -> size;
                        void *payload = (void*)(*valuep) -> content;
                        proto_send_packet(connfdp, data_pkt, payload);
                        blob_unref(*valuep, "obtained from store get");
                    }

                    free(valuep);
                    free(reply);
                    store_show();
                    trans_show_all();
                }
            }
            /* COMMIT REQ */
            else if(pkt -> type == XACTO_COMMIT_PKT)
            {
                debug("[%d] COMMIT packet received", connfdp);

                /* COMMIT TRANSACTION */
                TRANS_STATUS status = trans_commit(trans);

                /* SEND REPLY PKT */
                XACTO_PACKET *reply = getReplyPkt(status);
                proto_send_packet(connfdp, reply, (void*)NULL);

                free(reply);

                store_show();
                trans_show_all();

                break;
            }
        }
        else
        {
            if(errno == EIO)
                break;
        }
    }

    free(key);
    free(value);
    free(pkt);
    free(datap);

    return (void *)NULL;
}