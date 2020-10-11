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
#define _IN_PERIF_C_
#include "perif.h"
#undef _IN_PERIF_C_

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <xc.h>


unsigned char PerifBeepCount;
unsigned char PerifBeepOn;
unsigned char PerifBeepOff;

void PerifInit( void )
{
    RE1 = 0;    /* Buzzer */
    PerifBeepCount = 0;
    PerifBeepOn = 0;
    PerifBeepOff = 0;
}

void PerifTask( void )
{
    
}

void PerifTimer10ms( void )
{
    if(PerifBeepOn)
    {
        PerifBeepOn--;
        if( !PerifBeepOn)
        {
            PORTE &= 0b11111101;
            PerifBeepOff = 30;
        }
    }
    else if(PerifBeepOff)
    {
        PerifBeepOff--;
        if( !PerifBeepOff)
        {
            PerifBeepCount--;
            if(PerifBeepCount)
            {
                PORTE |= 0b00000010;
                PerifBeepOn = 5;
            }
            
        }
        
    }
}

void PerifTimer1s( void )
{

}

void Beep(unsigned char count)
{
    PORTE |= 0b00000010;
    /* El primer beep es mas largo salvo que count sea cero */
    PerifBeepOn = (count)?30:5;
    PerifBeepOff = 0;
    PerifBeepCount = (count)?count:1;
}
