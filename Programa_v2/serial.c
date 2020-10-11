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
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include <xc.h>

#include "serial.h"

#include "ioports.h"
#include "perif.h"

unsigned char SerRxBuffer[sizeof(serial_msg)];
unsigned char SerRxBufferIndex;
unsigned char SerTimerCount;
unsigned char SerMsgCount;

void SerPutCh( char ch );

/*
	Inicializa el Port serie
		interrupcion de RX
		lineas de control para el modem GSM
		cola de comandos al modem
*/
void SerInit( void )
{
    /* Inicializacion del puerto Serie */

    /* Configuro el Baud Rate */
    BRGH = 1;
    BRG16 = 0;
    SPBRGH = 0x00;
    SPBRG = IOPORTCONF.baud_rate;   /* 0x17 = 9600 bps */

    /* Levanto Tx y Rx */
    TXEN = 1;
    SYNC = 0;
    CREN = 1;
    SPEN = 1;
    
    SerTimerCount = 0;
    SerRxBufferIndex = 0;
    SerMsgCount = 0;

    PEIE = 1;       /* Enable Perif Int */
    RCIE = 1;       /* Enable RX Int */
    
    /* Reseteo la recepcion */
    CREN = 0;
    CREN = 1;
}

void SerPutCh( unsigned char ch )
{
    /* Mando un Byte */
    while( !TRMT);
    TXREG = ch;
}

void SerSendQ(serial_msg* msg)
{
    char* p = &msg->xTyp;
    int i;

    msg->xTyp = 'Q';
    msg->xDst = 0;
    msg->xSrc = IOPORTCONF.hw_address;
    msg->xCnt = SerMsgCount;
    msg->xSum = 0;
    for(i = 0; i < 15; i++)
    {
        msg->xSum += *(p+i);
    }
    for(i = 0; i < 16; i++)
    {
        SerPutCh(*(p+i));
    }
}

void SerSendR(serial_msg* msg)
{
    char* p = &msg->xTyp;
    int i;

    if(msg->xTyp == 'Q')msg->xTyp = 'R';
    msg->xDst = 0;
    msg->xSrc = IOPORTCONF.hw_address;
    msg->xSum = 0;
    for(i = 0; i < 15; i++)
    {
        msg->xSum += *(p+i);
    }
    for(i = 0; i < 16; i++)
    {
        SerPutCh(*(p+i));
    }
}

unsigned char SerReceive( serial_msg* msg )
{
  unsigned char i;
	char* p;

	if(SerRxBufferIndex == 16)
	{
		p = &msg->xTyp;
		for(i = 0; i < 16; i++)
		{
			*p = SerRxBuffer[i];
			p++;
		}
		SerRxBufferIndex = 0;
		return 16;
	}
	return 0;
}

void SerRecv( unsigned char ch )
{
    unsigned char sum;
    unsigned char i;
    
    if(SerRxBufferIndex == 0)
    {
        /* Inicio de mensaje */
        if(ch == 'Q' || ch == 'R' || ch == 'E')
        {
            SerRxBuffer[0] = ch;
            SerRxBufferIndex++;
            SerTimerCount = 5;
        }
    }
    else if(SerRxBufferIndex < 15)
    {
        /* Cuerpo de mensaje */
        SerRxBuffer[SerRxBufferIndex] = ch;
        SerRxBufferIndex++;
        SerTimerCount = 5;
    }
    else if(SerRxBufferIndex == 15)
    {
        /* Fin de mensaje, control de chksum, etc. */
        SerTimerCount = 0;
        SerRxBuffer[SerRxBufferIndex] = ch;
        SerRxBufferIndex++;
        sum = 0;
        for(i = 0; i < 15; i++)
        {
            sum += SerRxBuffer[i];
        }
        if(sum != SerRxBuffer[15])
        {
            /* descarto */
            SerRxBufferIndex = 0;
            return;
        }
        /* Control de source address */
        if(SerRxBuffer[2] != 0x00)
        {
            SerRxBufferIndex = 0;
            return;
        }

        /* Control de dest address */
        if(SerRxBuffer[1] != IOPORTCONF.hw_address &&
           SerRxBuffer[1] != 0xFF )
        {
            SerRxBufferIndex = 0;
            return;
        }

    }
    
}

void SerTask( void )
{
    if(OERR)
    {
        CREN = 0;
        CREN = 1;
        //Beep(5);
    }
}

void SerTimer( void )
{
    if(SerTimerCount)
    {
        SerTimerCount--;
        if( !SerTimerCount)
        {
            SerRxBufferIndex = 0;
        }
    }
}

void SerAck( void )
{
    SerMsgCount++;
}

void SerTimeOut( void )
{
    SerMsgCount++;
}
