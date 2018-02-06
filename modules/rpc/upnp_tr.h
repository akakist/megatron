/*
 * This file Copyright (C) Mnemosyne LLC
 *
 * This file is licensed by the GPL version 2. Works owned by the
 * Transmission project are granted a special exemption to clause 2(b)
 * so that the bulk of its code can remain under the MIT license.
 * This exemption does not extend to derived works not owned by
 * the Transmission project.
 *
 * $Id: upnp.h 11709 2011-01-19 13:48:47Z jordan $
 */


#ifndef TR_UPNP_H
#define TR_UPNP_H 1

/**
 * @addtogroup port_forwarding Port Forwarding
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

//typedef struct tr_upnp;
extern char tr_lanaddr[];
extern int tr_port;

struct tr_upnp;
struct tr_upnp * tr_upnpInit( void );

void      tr_upnpClose(struct tr_upnp * );

int       tr_upnpPulse(      struct   tr_upnp *,
                             int port,
                             int isEnabled,
                             int doPortCheck );
#ifdef __cplusplus
}
#endif
/* @} */
#endif
