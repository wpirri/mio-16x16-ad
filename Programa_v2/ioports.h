/***************************************************************************
    Copyright (C) 2013   Walter Pirri

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ***************************************************************************/

#ifndef IOPORTS_H
#define	IOPORTS_H

#include "config.h"

typedef struct  _mio16x16_ad_ports
{
    struct
    {
        unsigned char status;
        unsigned char last_status;
        unsigned char notif;
    } io[4];    /* 0 y 1 Entradas - 2 y 3 Salidas */
    struct
    {
        unsigned char alarm;
        unsigned char last_alarm;
        unsigned char statush;
        unsigned char statusl;
    } analog[4];
    unsigned char pulse[16];
    unsigned char status_count[16];
} mio16x16_ad_ports;

#ifdef _IN_IOPORTS_C_
mio16x16_ad_ports IOPORT;
#else
extern mio16x16_ad_ports IOPORT;
#endif

typedef struct _mio16x16_ad_port_config
{
    unsigned char hw_address;
    struct
    {
        unsigned char mascara_seguidor;
        unsigned char mascara_notif;
    } io[4];
    struct
    {
        unsigned char maxh;
        unsigned char maxl;
        unsigned char minh;
        unsigned char minl;
    } analog[4];
    unsigned char mascara_seguidor_an;
    unsigned char mascara_notif_an;
    unsigned char baud_rate;
} mio16x16_ad_port_config;

#ifdef _IN_IOPORTS_C_
mio16x16_ad_port_config IOPORTCONF;
#else
extern mio16x16_ad_port_config IOPORTCONF;
#endif

void IOInit( void );
void IOTask( void );
void IOTimer( void );

unsigned char IOChanges(unsigned char io);
void IOResetChanges(unsigned char io, unsigned char reset);

#endif	/* IOPORTS_H */

