/* ************************************************************************** *
 *
 *  Filename:           touch.h
 *  Date:               23 de junio de 2014
 *  File Version:       1.0
 *  Assembled using:    XC8
 *
 *  Author:             Walter Pirri
 *                      (walter***AT***pirri***DOT***com***DOT***ar)
 *  Company:            WGP
 *
 *************************************************************************** */
#ifndef _WIEGAND_H_
#define	_WIEGAND_H_

#include "config.h"

typedef struct _wgnd_data
{
    union
    {
        struct
        {
            unsigned char d;
            unsigned char c;
            unsigned char b;
            unsigned char a;
        } bytes;
        unsigned long ldata;
    } udata;
} wgnd_data;


#ifdef _IN_WIEGAND_C_
volatile unsigned char gWGND_DataReady;
volatile wgnd_data gWGND_Data;
#else
extern volatile unsigned char gWGND_DataReady;
extern volatile wgnd_data gWGND_Data;
#endif /* _IN_WIEGAND_C_ */

void WGND_Init( void );
void WGND_Timer( void );
void WGND_Ack( void );
void WGND_IntData( unsigned char d0, unsigned char d1 );


#endif /* _WIEGAND_H_ */