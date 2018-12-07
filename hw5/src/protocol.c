#include "protocol.h"
#include <arpa/inet.h>
#include <unistd.h>
#include "csapp.h"
#include "debug.h"

int proto_send_packet(int fd, XACTO_PACKET *pkt, void *data)
{
    debug("*********************INSIDE proto_send_packet****************************");
    int bytes_read;
    int size_cpy = pkt->size;
    pkt->size = htonl(pkt->size);
    pkt->timestamp_sec = htonl(pkt->timestamp_sec);
    pkt->timestamp_nsec = htonl(pkt->timestamp_nsec);

    if((bytes_read = rio_writen(fd, pkt, sizeof(XACTO_PACKET))) >= 0)
    {
        if(bytes_read == 0)
            return 0;

        if(pkt -> type == XACTO_DATA_PKT)
        {
            debug("********************SENDING DATA PQCKET****************************");
            if(size_cpy != 0)
            {
                if((bytes_read = rio_writen(fd, data, size_cpy)) < 0)
                    return -1;
            }
        }
        else if(pkt -> type == XACTO_PUT_PKT)
            debug("*********************SENDING PUT PKT TO SERVER****************************");
        else if(pkt -> type == XACTO_GET_PKT)
            debug("*********************SENDING GET PKT TO SERVER****************************");
        else if(pkt -> type == XACTO_COMMIT_PKT)
            debug("*********************SENDING COMMIT PKT TO SERVER****************************");
        else if(pkt -> type == XACTO_REPLY_PKT)
            debug("*********************SENDING REPLY PKT TO CLNT****************************");
    }
    else return -1;

    return 0;
}

int proto_recv_packet(int fd, XACTO_PACKET *pkt, void **datap){

    debug("*********************INSIDE proto_recv_packet****************************");
    int bytes_read;
    if((bytes_read = rio_readn(fd, pkt, sizeof(XACTO_PACKET))) >= 0)
    {
        if(bytes_read == 0)
        {
            errno = 5;
            return -1;
        }

        pkt->size = ntohl(pkt->size);
        pkt->timestamp_sec = ntohl(pkt->timestamp_sec);
        pkt->timestamp_nsec = ntohl(pkt->timestamp_nsec);

        if(pkt -> type == XACTO_DATA_PKT)
        {
            debug("*********************RECEIVED DATA PKT****************************");
            if(pkt->size != 0){
                debug("*********************RECEIVED DATA PKT PAYLOAD****************************");
                *datap = malloc(pkt->size);
                if((bytes_read = rio_readn(fd, *datap, pkt->size)) < 0)
                    return -1;
                // else if (bytes_read == 0)
                // {
                //     errno = 5;
                //     return -1;
                // }
            }
            else debug("*********************DATA PKT HAS NO PAYLOAD****************************");
        }
        else if(pkt -> type == XACTO_PUT_PKT)
            debug("*********************RECEIVED PUT PKT FRM CLNT****************************");
        else if(pkt -> type == XACTO_GET_PKT)
            debug("*********************RECEIVED GET PKT FRM CLNT****************************");
        else if(pkt -> type == XACTO_COMMIT_PKT)
            debug("*********************RECEIVED COMMIT PKT FRM CLNT****************************");
        else if(pkt -> type == XACTO_REPLY_PKT)
            debug("*********************RECEIVED REPLY PKT FRM SEVER****************************");
    }
    else return -1;

    return 0;
}