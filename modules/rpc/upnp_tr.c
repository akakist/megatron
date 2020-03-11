/*
 * This file Copyright (C) Mnemosyne LLC
 *
 * This file is licensed by the GPL version 2. Works owned by the
 * Transmission project are granted a special exemption to clause 2(b)
 * so that the bulk of its code can remain under the MIT license.
 * This exemption does not extend to derived works not owned by
 * the Transmission project.
 *
 * $Id: upnp.c 12204 2011-03-22 15:19:54Z jordan $
 */
#ifndef WITHOUT_UPNP

#include <assert.h>
#include <errno.h>
#ifndef __MACH__
#include <malloc.h>
#endif
#include <string.h>
#include <stdio.h>

#include "miniupnp/miniupnpc/miniupnpc.h"
#include "miniupnp/miniupnpc/upnpcommands.h"

#include <stdlib.h>

#include "upnp_tr.h"

//static const char *
//getKey( void ) { return _( "Port Forwarding (UPnP)" ); }

typedef enum
{
    TR_UPNP_IDLE,
    TR_UPNP_ERR,
    TR_UPNP_DISCOVER,
    TR_UPNP_MAP,
    TR_UPNP_UNMAP
}
tr_upnp_state;

typedef enum
{
    TR_PORT_ERROR,
    TR_PORT_UNMAPPED,
    TR_PORT_UNMAPPING,
    TR_PORT_MAPPING,
    TR_PORT_MAPPED
}
tr_port_forwarding;

#ifndef __cplusplus
typedef unsigned char bool;
//#define NULL 0
#define true 1
#define false 0
#endif

struct tr_upnp
{
    bool               hasDiscovered;
    struct UPNPUrls    urls;
    struct IGDdatas    data;
    int                port;
    char               lanaddr[16];
    unsigned int       isMapped;
    tr_upnp_state      state;
};

/**
***
**/
char tr_lanaddr[16];
int tr_port;

struct tr_upnp*
tr_upnpInit( void )
{
    struct tr_upnp * ret = (struct tr_upnp * )malloc(sizeof(struct tr_upnp));

    ret->state = TR_UPNP_DISCOVER;
    ret->port = -1;
    return ret;
}

void
tr_upnpClose(struct  tr_upnp * handle )
{
    assert( !handle->isMapped );
    assert( ( handle->state == TR_UPNP_IDLE )
            || ( handle->state == TR_UPNP_ERR )
            || ( handle->state == TR_UPNP_DISCOVER ) );

    if( handle->hasDiscovered )
        FreeUPNPUrls( &handle->urls );
    free( handle );
}


/**
***
**/

enum
{
    UPNP_IGD_NONE = 0,
    UPNP_IGD_VALID_CONNECTED = 1,
    UPNP_IGD_VALID_NOT_CONNECTED = 2,
    UPNP_IGD_INVALID = 3
};



int
tr_upnpPulse( struct tr_upnp * handle,
              int       port,
              int       isEnabled,
              int       doPortCheck )
{
    int ret;

    if( isEnabled && ( handle->state == TR_UPNP_DISCOVER ) )
    {
        struct UPNPDev * devlist;
        errno = 0;
        /*
         * old
         * LIBSPEC struct UPNPDev *
upnpDiscover(int delay, const char * multicastif,
             const char * minissdpdsock, int sameport,
             int ipv6,
             int * error);


* new
         *
upnpDiscover(int delay, const char * multicastif,
             const char * minissdpdsock, int localport,
             int ipv6, unsigned char ttl,
             int * error);
*/
        devlist = upnpDiscover( 2000, NULL, NULL, 0,0,NULL);
        if( devlist == NULL )
        {
            printf(
                "upnpDiscover failed (errno %d - %s)\n", errno,
                strerror( errno ) );
        }
        errno = 0;
        if( UPNP_GetValidIGD( devlist, &handle->urls, &handle->data,
                              handle->lanaddr, sizeof( handle->lanaddr ) ) == UPNP_IGD_VALID_CONNECTED )
        {
            printf("Found Internet Gateway Device \"%s\"\n",handle->urls.controlURL );
            printf("Local Address is \"%s\"\n" , handle->lanaddr );
            handle->state = TR_UPNP_IDLE;
            handle->hasDiscovered = 1;
        }
        else
        {
            handle->state = TR_UPNP_ERR;
            printf("UPNP_GetValidIGD failed (errno %d - %s)",errno,strerror( errno ) );
            printf("If your router supports UPnP, please make sure UPnP is enabled!" );
        }
        freeUPNPDevlist( devlist );
    }

    if( handle->state == TR_UPNP_IDLE )
    {
        if( handle->isMapped && ( !isEnabled || ( handle->port != port ) ) )
            handle->state = TR_UPNP_UNMAP;
    }


    if( isEnabled && handle->isMapped && doPortCheck )
    {
        char portStr[8];
        char intPort[8];
        char intClient[16];


        snprintf( portStr, sizeof( portStr ), "%d", handle->port );
        /*
        UPNP_GetSpecificPortMappingEntry(const char * controlURL,
                                 const char * servicetype,
                                 const char * extPort,
                                 const char * proto,
                                 char * intClient,
                                 char * intPort,
                                 char * desc,
                                 char * enabled,
                                 char * leaseDuration);
        */
        char enabled[100];
        char leasedur[100];

        /*
         * old:
         * LIBSPEC int
UPNP_GetSpecificPortMappingEntry(const char * controlURL,
                                 const char * servicetype,
                                 const char * extPort,
                                 const char * proto,
                                 char * intClient,
                                 char * intPort,
                                 char * desc,
                                 char * enabled,
                                 char * leaseDuration);
         * new:
MINIUPNP_LIBSPEC int
UPNP_GetSpecificPortMappingEntry(const char * controlURL,
                                 const char * servicetype,
                                 const char * extPort,
                                 const char * proto,
                                 const char * remoteHost,
                                 char * intClient,
                                 char * intPort,
                                 char * desc,
                                 char * enabled,
                                 char * leaseDuration);

*/
        if( UPNP_GetSpecificPortMappingEntry( handle->urls.controlURL, handle->data.first.servicetype,
                                              portStr, "TCP", intClient, intPort,"PROJECT_PREFIX" ,enabled,leasedur ) != UPNPCOMMAND_SUCCESS  ||
                UPNP_GetSpecificPortMappingEntry( handle->urls.controlURL, handle->data.first.servicetype,
                        portStr, "UDP", intClient, intPort,"PROJECT_PREFIX" ,enabled,leasedur ) != UPNPCOMMAND_SUCCESS )
        {
            printf( "Port %d isn't forwarded\n" , handle->port );
            handle->isMapped = false;
        }
    }



    if( handle->state == TR_UPNP_UNMAP )
    {
        char portStr[16];
        snprintf( portStr, sizeof( portStr ), "%d", handle->port );
        UPNP_DeletePortMapping( handle->urls.controlURL,
                                handle->data.first.servicetype,
                                portStr, "TCP", NULL );
        UPNP_DeletePortMapping( handle->urls.controlURL,
                                handle->data.first.servicetype,
                                portStr, "UDP", NULL );
        printf( "Stopping port forwarding through \"%s\", service \"%s\"\n" ,handle->urls.controlURL, handle->data.first.servicetype );
        handle->isMapped = 0;
        handle->state = TR_UPNP_IDLE;
        handle->port = -1;
    }

    if( handle->state == TR_UPNP_IDLE )
    {
        if( isEnabled && !handle->isMapped )
            handle->state = TR_UPNP_MAP;
    }

    if( handle->state == TR_UPNP_MAP )
    {
        int  err_tcp = -1;
        int  err_udp = -1;
        errno = 0;

        if( !handle->urls.controlURL /*|| !handle->data.first.servicetype */)
            handle->isMapped = 0;
        else
        {
            char portStr[16];
            char desc[64];
            const int prev_errno = errno;
            snprintf( portStr, sizeof( portStr ), "%d", port );
            snprintf( desc, sizeof( desc ), "%s at %d", "PROJECT_PREFIX" "-app", port );

            errno = 0;
            /*
            UPNP_AddPortMapping(const char * controlURL, const char * servicetype,
                    const char * extPort,
                    const char * inPort,
                    const char * inClient,
                    const char * desc,
                    const char * proto,
                    const char * remoteHost,
                    const char * leaseDuration);

            */
            err_tcp = UPNP_AddPortMapping( handle->urls.controlURL,
                                           handle->data.first.servicetype,
                                           portStr, portStr, handle->lanaddr,
                                           desc, "TCP", NULL,NULL );
            if( err_tcp )
                printf( "TCP Port forwarding failed with error %d (errno %d - %s)\n",err_tcp, errno, strerror( errno ) );

            errno = 0;
            err_udp = UPNP_AddPortMapping( handle->urls.controlURL,
                                           handle->data.first.servicetype,
                                           portStr, portStr, handle->lanaddr,
                                           desc, "UDP", NULL,NULL );
            if( err_udp )
                printf( "UDP Port forwarding failed with error %d (errno %d - %s)\n",err_udp, errno, strerror( errno ) );

            errno = prev_errno;
            handle->isMapped = !err_tcp | !err_udp;
        }
        //printf(  "Port forwarding through \"%s\", service \"%s\". (local address: %s:%d)\n" ,handle->urls.controlURL, handle->data.first.servicetype,handle->lanaddr, port );
        strcpy(tr_lanaddr,handle->lanaddr);
        //printf("tr_lanaddr %s\n",tr_lanaddr);
        tr_port=port;
        if( handle->isMapped )
        {
            printf( "Port forwarding successful!\n");
            handle->port = port;
            handle->state = TR_UPNP_IDLE;
        }
        else
        {
            printf( "If your router supports UPnP, please make sure UPnP is enabled!\n" );
            handle->port = -1;
            handle->state = TR_UPNP_ERR;
        }
    }

    switch( handle->state )
    {
    case TR_UPNP_DISCOVER:
        ret = TR_PORT_UNMAPPED;
        break;

    case TR_UPNP_MAP:
        ret = TR_PORT_MAPPING;
        break;

    case TR_UPNP_UNMAP:
        ret = TR_PORT_UNMAPPING;
        break;

    case TR_UPNP_IDLE:
        ret = handle->isMapped ? TR_PORT_MAPPED
              : TR_PORT_UNMAPPED;
        break;

    default:
        ret = TR_PORT_ERROR;
        break;
    }

    return ret;
}

#endif
