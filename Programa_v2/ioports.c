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

#include <xc.h>

#define _IN_IOPORTS_C_
#include "ioports.h"
#undef _IN_IOPORTS_C_

void IOUpdateOut( void )
{
    /* Salida 1 - B4=1 B5=0 -B6=1- B7=1 */
    if(IOPORT.io[2].status != IOPORT.io[2].last_status)
    {
        IOPORT.io[2].notif = IOPORT.io[2].status ^ IOPORT.io[2].last_status;

        TRISD = 0x00;
        PORTD = IOPORT.io[2].status;
        NOP();
        NOP();
        NOP();
        PORTB |= 0b01000000;
        NOP();
        NOP();
        NOP();
        PORTB &= 0b10111111;
        NOP();
        NOP();
        NOP();
        TRISD = 0xff;
        IOPORT.io[2].last_status = IOPORT.io[2].status;
    }
    /* Salida 2 - B4=1 -B5=1- B6=0 B7=1 */
    if(IOPORT.io[3].status != IOPORT.io[3].last_status)
    {
        IOPORT.io[3].notif = IOPORT.io[3].status ^ IOPORT.io[3].last_status;

        TRISD = 0x00;
        PORTD = IOPORT.io[3].status;
        NOP();
        NOP();
        NOP();
        PORTB |= 0b00100000;
        NOP();
        NOP();
        NOP();
        PORTB &= 0b11011111;
        NOP();
        NOP();
        NOP();
        TRISD = 0xff;
        IOPORT.io[3].last_status = IOPORT.io[3].status;
    }
}

void IOUpdateIn( void )
{
    /* Selecciono entrada 1 - -B4=0- B5=0 B6=0 B7=1 */
    PORTB &= 0b11101111;
    NOP();
    NOP();
    NOP();
    /* Entrada 1,0 */
    if(PORTD & 0b00000001)
    {
        if(IOPORT.status_count[0] < 255)
        {
            IOPORT.status_count[0]++;
            if(IOPORT.status_count[0] == 255)
            {
                IOPORT.io[0].status |= 0b00000001;
            }
        }
    }
    else
    {
        if(IOPORT.status_count[0] > 0)
        {
            IOPORT.status_count[0]--;
            if(IOPORT.status_count[0] == 0)
            {
                IOPORT.io[0].status &= 0b11111110;
            }
        }
    }
    /* Entrada 1,1 */
    if(PORTD & 0b00000010)
    {
        if(IOPORT.status_count[1] < 255)
        {
            IOPORT.status_count[1]++;
            if(IOPORT.status_count[1] == 255)
            {
                IOPORT.io[0].status |= 0b00000010;
            }
        }
    }
    else
    {
        if(IOPORT.status_count[1] > 0)
        {
            IOPORT.status_count[1]--;
            if(IOPORT.status_count[1] == 0)
            {
                IOPORT.io[0].status &= 0b11111101;
            }
        }
    }
    /* Entrada 1,2 */
    if(PORTD & 0b00000100)
    {
        if(IOPORT.status_count[2] < 255)
        {
            IOPORT.status_count[2]++;
            if(IOPORT.status_count[2] == 255)
            {
                IOPORT.io[0].status |= 0b00000100;
            }
        }
    }
    else
    {
        if(IOPORT.status_count[2] > 0)
        {
            IOPORT.status_count[2]--;
            if(IOPORT.status_count[2] == 0)
            {
                IOPORT.io[0].status &= 0b11111011;
            }
        }
    }
    /* Entrada 1,3 */
    if(PORTD & 0b00001000)
    {
        if(IOPORT.status_count[3] < 255)
        {
            IOPORT.status_count[3]++;
            if(IOPORT.status_count[3] == 255)
            {
                IOPORT.io[0].status |= 0b00001000;
            }
        }
    }
    else
    {
        if(IOPORT.status_count[3] > 0)
        {
            IOPORT.status_count[3]--;
            if(IOPORT.status_count[3] == 0)
            {
                IOPORT.io[0].status &= 0b11110111;
            }
        }
    }
    /* Entrada 1,4 */
    if(PORTD & 0b00010000)
    {
        if(IOPORT.status_count[4] < 255)
        {
            IOPORT.status_count[4]++;
            if(IOPORT.status_count[4] == 255)
            {
                IOPORT.io[0].status |= 0b00010000;
            }
        }
    }
    else
    {
        if(IOPORT.status_count[4] > 0)
        {
            IOPORT.status_count[4]--;
            if(IOPORT.status_count[4] == 0)
            {
                IOPORT.io[0].status &= 0b11101111;
            }
        }
    }
    /* Entrada 1,5 */
    if(PORTD & 0b00100000)
    {
        if(IOPORT.status_count[5] < 255)
        {
            IOPORT.status_count[5]++;
            if(IOPORT.status_count[5] == 255)
            {
                IOPORT.io[0].status |= 0b00100000;
            }
        }
    }
    else
    {
        if(IOPORT.status_count[5] > 0)
        {
            IOPORT.status_count[5]--;
            if(IOPORT.status_count[5] == 0)
            {
                IOPORT.io[0].status &= 0b11011111;
            }
        }
    }
    /* Entrada 1,6 */
    if(PORTD & 0b01000000)
    {
        if(IOPORT.status_count[6] < 255)
        {
            IOPORT.status_count[6]++;
            if(IOPORT.status_count[6] == 255)
            {
                IOPORT.io[0].status |= 0b01000000;
            }
        }
    }
    else
    {
        if(IOPORT.status_count[6] > 0)
        {
            IOPORT.status_count[6]--;
            if(IOPORT.status_count[6] == 0)
            {
                IOPORT.io[0].status &= 0b10111111;
            }
        }
    }
    /* Entrada 1,7 */
    if(PORTD & 0b10000000)
    {
        if(IOPORT.status_count[7] < 255)
        {
            IOPORT.status_count[7]++;
            if(IOPORT.status_count[7] == 255)
            {
                IOPORT.io[0].status |= 0b10000000;
            }
        }
    }
    else
    {
        if(IOPORT.status_count[7] > 0)
        {
            IOPORT.status_count[7]--;
            if(IOPORT.status_count[7] == 0)
            {
                IOPORT.io[0].status &= 0b01111111;
            }
        }
    }
    PORTB |= 0b00010000;
    /* Fin entrada 1 */

    /* Entrada 2 - B4=1 B5=0 B6=0 -B7=0- */
    PORTB &= 0b01111111;
    NOP();
    NOP();
    NOP();
    /* Entrada 2,0 */
    if(PORTD & 0b00000001)
    {
        if(IOPORT.status_count[8] < 255)
        {
            IOPORT.status_count[8]++;
            if(IOPORT.status_count[8] == 255)
            {
                IOPORT.io[1].status |= 0b00000001;
            }
        }
    }
    else
    {
        if(IOPORT.status_count[8] > 0)
        {
            IOPORT.status_count[8]--;
            if(IOPORT.status_count[8] == 0)
            {
                IOPORT.io[1].status &= 0b11111110;
            }
        }
    }
    /* Entrada 2,1 */
    if(PORTD & 0b00000010)
    {
        if(IOPORT.status_count[9] < 255)
        {
            IOPORT.status_count[9]++;
            if(IOPORT.status_count[9] == 255)
            {
                IOPORT.io[1].status |= 0b00000010;
            }
        }
    }
    else
    {
        if(IOPORT.status_count[9] > 0)
        {
            IOPORT.status_count[9]--;
            if(IOPORT.status_count[9] == 0)
            {
                IOPORT.io[1].status &= 0b11111101;
            }
        }
    }
    /* Entrada 2,2 */
    if(PORTD & 0b00000100)
    {
        if(IOPORT.status_count[10] < 255)
        {
            IOPORT.status_count[10]++;
            if(IOPORT.status_count[10] == 255)
            {
                IOPORT.io[1].status |= 0b00000100;
            }
        }
    }
    else
    {
        if(IOPORT.status_count[10] > 0)
        {
            IOPORT.status_count[10]--;
            if(IOPORT.status_count[10] == 0)
            {
                IOPORT.io[1].status &= 0b11111011;
            }
        }
    }
    /* Entrada 2,3 */
    if(PORTD & 0b00001000)
    {
        if(IOPORT.status_count[11] < 255)
        {
            IOPORT.status_count[11]++;
            if(IOPORT.status_count[11] == 255)
            {
                IOPORT.io[1].status |= 0b00001000;
            }
        }
    }
    else
    {
        if(IOPORT.status_count[11] > 0)
        {
            IOPORT.status_count[11]--;
            if(IOPORT.status_count[11] == 0)
            {
                IOPORT.io[1].status &= 0b11110111;
            }
        }
    }
    /* Entrada 2,4 */
    if(PORTD & 0b00010000)
    {
        if(IOPORT.status_count[12] < 255)
        {
            IOPORT.status_count[12]++;
            if(IOPORT.status_count[12] == 255)
            {
                IOPORT.io[1].status |= 0b00010000;
            }
        }
    }
    else
    {
        if(IOPORT.status_count[12] > 0)
        {
            IOPORT.status_count[12]--;
            if(IOPORT.status_count[12] == 0)
            {
                IOPORT.io[1].status &= 0b11101111;
            }
        }
    }
    /* Entrada 2,5 */
    if(PORTD & 0b00100000)
    {
        if(IOPORT.status_count[13] < 255)
        {
            IOPORT.status_count[13]++;
            if(IOPORT.status_count[13] == 255)
            {
                IOPORT.io[1].status |= 0b00100000;
            }
        }
    }
    else
    {
        if(IOPORT.status_count[13] > 0)
        {
            IOPORT.status_count[13]--;
            if(IOPORT.status_count[13] == 0)
            {
                IOPORT.io[1].status &= 0b11011111;
            }
        }
    }
    /* Entrada 2,6 */
    if(PORTD & 0b01000000)
    {
        if(IOPORT.status_count[14] < 255)
        {
            IOPORT.status_count[14]++;
            if(IOPORT.status_count[14] == 255)
            {
                IOPORT.io[1].status |= 0b01000000;
            }
        }
    }
    else
    {
        if(IOPORT.status_count[14] > 0)
        {
            IOPORT.status_count[14]--;
            if(IOPORT.status_count[14] == 0)
            {
                IOPORT.io[1].status &= 0b10111111;
            }
        }
    }
    /* Entrada 2,7 */
    if(PORTD & 0b10000000)
    {
        if(IOPORT.status_count[15] < 255)
        {
            IOPORT.status_count[15]++;
            if(IOPORT.status_count[15] == 255)
            {
                IOPORT.io[1].status |= 0b10000000;
            }
        }
    }
    else
    {
        if(IOPORT.status_count[15] > 0)
        {
            IOPORT.status_count[15]--;
            if(IOPORT.status_count[15] == 0)
            {
                IOPORT.io[1].status &= 0b01111111;
            }
        }
    }
    PORTB |= 0b10000000;
    /* Fin Entrada 2 */
}

void IOControlFinPulso( void )
{
    /* Salida 1,0 */
    if(IOPORT.pulse[0])
    {
        IOPORT.pulse[0]--;
        if( !IOPORT.pulse[0])
        {
            IOPORT.io[2].status &= 0b11111110;
        }
    }
    /* Salida 1,1 */
    if(IOPORT.pulse[1])
    {
        IOPORT.pulse[1]--;
        if( !IOPORT.pulse[1])
        {
            IOPORT.io[2].status &= 0b11111101;
        }
    }
    /* Salida 1,2 */
    if(IOPORT.pulse[2])
    {
        IOPORT.pulse[2]--;
        if( !IOPORT.pulse[2])
        {
            IOPORT.io[2].status &= 0b11111011;
        }
    }
    /* Salida 1,3 */
    if(IOPORT.pulse[3])
    {
        IOPORT.pulse[3]--;
        if( !IOPORT.pulse[3])
        {
            IOPORT.io[2].status &= 0b11110111;
        }
    }
    /* Salida 1,4 */
    if(IOPORT.pulse[4])
    {
        IOPORT.pulse[4]--;
        if( !IOPORT.pulse[4])
        {
            IOPORT.io[2].status &= 0b11101111;
        }
    }
    /* Salida 1,5 */
    if(IOPORT.pulse[5])
    {
        IOPORT.pulse[5]--;
        if( !IOPORT.pulse[5])
        {
            IOPORT.io[2].status &= 0b11011111;
        }
    }
    /* Salida 1,6 */
    if(IOPORT.pulse[6])
    {
        IOPORT.pulse[6]--;
        if( !IOPORT.pulse[6])
        {
            IOPORT.io[2].status &= 0b10111111;
        }
    }
    /* Salida 1,7 */
    if(IOPORT.pulse[7])
    {
        IOPORT.pulse[7]--;
        if( !IOPORT.pulse[7])
        {
            IOPORT.io[2].status &= 0b01111111;
        }
    }

    /* Salida 2,0 */
    if(IOPORT.pulse[8])
    {
        IOPORT.pulse[8]--;
        if( !IOPORT.pulse[8])
        {
            IOPORT.io[3].status &= 0b11111110;
        }
    }
    /* Salida 2,1 */
    if(IOPORT.pulse[9])
    {
        IOPORT.pulse[9]--;
        if( !IOPORT.pulse[9])
        {
            IOPORT.io[3].status &= 0b11111101;
        }
    }
    /* Salida 2,2 */
    if(IOPORT.pulse[10])
    {
        IOPORT.pulse[10]--;
        if( !IOPORT.pulse[10])
        {
            IOPORT.io[3].status &= 0b11111011;
        }
    }
    /* Salida 2,3 */
    if(IOPORT.pulse[11])
    {
        IOPORT.pulse[11]--;
        if( !IOPORT.pulse[11])
        {
            IOPORT.io[3].status &= 0b11110111;
        }
    }
    /* Salida 2,4 */
    if(IOPORT.pulse[12])
    {
        IOPORT.pulse[12]--;
        if( !IOPORT.pulse[12])
        {
            IOPORT.io[3].status &= 0b11101111;
        }
    }
    /* Salida 2,5 */
    if(IOPORT.pulse[13])
    {
        IOPORT.pulse[13]--;
        if( !IOPORT.pulse[13])
        {
            IOPORT.io[3].status &= 0b11011111;
        }
    }
    /* Salida 2,6 */
    if(IOPORT.pulse[14])
    {
        IOPORT.pulse[14]--;
        if( !IOPORT.pulse[14])
        {
            IOPORT.io[3].status &= 0b10111111;
        }
    }
    /* Salida 2,7 */
    if(IOPORT.pulse[15])
    {
        IOPORT.pulse[15]--;
        if( !IOPORT.pulse[15])
        {
            IOPORT.io[3].status &= 0b01111111;
        }
    }

}

void IOInit( void )
{
    /* B4=1 B5=0 B6=0 B7=1 */
    
    PORTB |= 0b10010000;
    PORTB &= 0b10011111;
    TRISD = 0xff;

    IOPORT.io[0].status = 0x00;
    IOPORT.io[0].last_status = 0x00;
    IOPORT.io[1].status = 0x00;
    IOPORT.io[1].last_status = 0x00;
    IOPORT.io[2].status = 0x00;
    IOPORT.io[2].last_status = 0xff;
    IOPORT.io[2].notif = 0x00;
    IOPORT.io[3].status = 0x00;
    IOPORT.io[3].last_status = 0xff;
    IOPORT.io[3].notif = 0x00;
    IOPORT.status_count[0] = 0x7f;
    IOPORT.status_count[1] = 0x7f;
    IOPORT.status_count[2] = 0x7f;
    IOPORT.status_count[3] = 0x7f;
    IOPORT.status_count[4] = 0x7f;
    IOPORT.status_count[5] = 0x7f;
    IOPORT.status_count[6] = 0x7f;
    IOPORT.status_count[7] = 0x7f;
    IOPORT.status_count[8] = 0x7f;
    IOPORT.status_count[9] = 0x7f;
    IOPORT.status_count[10] = 0x7f;
    IOPORT.status_count[11] = 0x7f;
    IOPORT.status_count[12] = 0x7f;
    IOPORT.status_count[13] = 0x7f;
    IOPORT.status_count[14] = 0x7f;
    IOPORT.status_count[15] = 0x7f;
    IOPORT.pulse[0] = 0x00;
    IOPORT.pulse[1] = 0x00;
    IOPORT.pulse[2] = 0x00;
    IOPORT.pulse[3] = 0x00;
    IOPORT.pulse[4] = 0x00;
    IOPORT.pulse[5] = 0x00;
    IOPORT.pulse[6] = 0x00;
    IOPORT.pulse[7] = 0x00;
    IOPORT.pulse[8] = 0x00;
    IOPORT.pulse[9] = 0x00;
    IOPORT.pulse[10] = 0x00;
    IOPORT.pulse[11] = 0x00;
    IOPORT.pulse[12] = 0x00;
    IOPORT.pulse[13] = 0x00;
    IOPORT.pulse[14] = 0x00;
    IOPORT.pulse[15] = 0x00;
    
    IOUpdateOut();
}

void IOTask( void )
{
    IOUpdateOut();
    IOUpdateIn();
}

void IOTimer( void )
{
    IOControlFinPulso();
}

unsigned char IOChanges(unsigned char io)
{
    unsigned char rc = 0x00;
    
    switch(io)
    {
        case '1':
            rc = (IOPORT.io[0].status ^ IOPORT.io[0].last_status) & IOPORTCONF.io[0].mascara_notif;
            break;
        case '2':
            rc = (IOPORT.io[1].status ^ IOPORT.io[1].last_status) & IOPORTCONF.io[1].mascara_notif;
            break;
        case '3':
            rc = IOPORT.io[2].notif & IOPORTCONF.io[2].mascara_notif;
            break;
        case '4':
            rc = IOPORT.io[3].notif & IOPORTCONF.io[3].mascara_notif;
            break;
    
    }
    return rc;
}

void IOResetChanges(unsigned char io, unsigned char reset)
{
    switch(io)
    {
        case '1':
            IOPORT.io[0].last_status = reset;
            break;
        case '2':
            IOPORT.io[1].last_status = reset;
            break;
        case '3':
            IOPORT.io[2].notif = IOPORT.io[2].last_status ^ reset;
            break;
        case '4':
            IOPORT.io[3].notif = IOPORT.io[3].last_status ^ reset;
            break;
    
    }
}
