/* ************************************************************************** *
 *
 *  Filename:           wiegand.c
 *  Date:               6 de Octubre de 2014
 *  File Version:       1.0
 *  Assembled using:    XC8
 *
 *  Author:             Walter Pirri
 *                      (walter***AT***pirri***DOT***com***DOT***ar)
 *  Company:            WGP
 *
 * ************************************************************************* *
 * Utiliza los bits 1 y 2 del port B para
 *
 *************************************************************************** */
#define _IN_WIEGAND_C_
#include "wiegand.h"
#undef _IN_WIEGAND_C_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <xc.h>

unsigned char gWGND_TimerCount;
unsigned char gWGND_BitCount;

void WGND_Init( void )
{
    gWGND_DataReady = 0;
    gWGND_TimerCount = 0;
    gWGND_BitCount = 0;
    gWGND_Data.udata.ldata = 0l;

    /* Seteo las entradas que se van a usar */
//    ANSELB &= 0b11111001;
    TRISC |= 0b00000011;
    TRISC &= 0b11111011;
//    WPUB |= 0b00000110;

    /* Seteo las interrupciones de esas entradas */
//    IOCBN |= 0b00000110;
    PORTC &= 0b11111011;
}

/*  WGND_Timer
 *      Se lo llama 10 veces por segundo
 */
void WGND_Timer( void )
{
    if(gWGND_TimerCount)
    {
        gWGND_TimerCount--;
        if( !gWGND_TimerCount)
        {
            /* Reseteo los datos que pueda haber */
            gWGND_BitCount = 0;
            gWGND_Data.udata.ldata = 0l;
            gWGND_DataReady = 0;
        }
    }
}

void WGND_Ack( void )
{
    /* Reseteo todo para una nueva recepción */
    gWGND_BitCount = 0;
    gWGND_Data.udata.ldata = 0l;
    gWGND_DataReady = 0;
    PORTC &= 0b11111011;
}

/*  WGND_IntData
 *      Se la llama desde la interrupcion por cambios en PORT
 */
void WGND_IntData( unsigned char d0, unsigned char d1 )
{
    if(gWGND_DataReady) return;
    if(d0 == d1) return;

    gWGND_Data.udata.ldata >>= 1;
    
    if(!d1) gWGND_Data.udata.ldata |= 0x80000000;
    else    gWGND_Data.udata.ldata &= 0x7FFFFFFF;

    gWGND_BitCount++;

    if(gWGND_BitCount == 26)
    {
        /* desplaso los bits para alinearlos a 32 */
        gWGND_Data.udata.ldata >>= 6;
        /* los 6 bits mas significativos no van */
        gWGND_Data.udata.ldata &= 0x3FFFFFFF;

        gWGND_TimerCount = 0;
        gWGND_DataReady = 1;
        PORTC |= 0b00000100;
    }
    else
    {
        gWGND_TimerCount = 5;
    }
}
