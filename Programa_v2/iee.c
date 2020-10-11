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
#include <string.h>

#include <xc.h>

#include "iee.h"

//This function Writes data to given address in internal EEPROM of PIC MCU
void iEEputc(unsigned char address, unsigned char data)
{
    unsigned char INTCON_SAVE;

    EEADR = address;
    EEDATA = data;

    EEPGD = 0; // 0 = Access data EEPROM memory
//    CFGS = 0; // 0 = Access Flash program or DATA EEPROM memory
    WREN = 1; // enable writes to internal EEPROM

    INTCON_SAVE = INTCON; // Save INTCON register contants
    INTCON = 0;             // Disable interrupts, Next two lines SHOULD run without interrupts

    EECON2 = 0x55;        // Required sequence for write to internal EEPROM
    EECON2 = 0xAA;        // Required sequence for write to internal EEPROM

    WR = 1;    // begin write to internal EEPROM
    INTCON = INTCON_SAVE; //Now we can safely enable interrupts if previously used

    NOP();

    while(EEIF == 0)//Wait till write operation complete
    {
        CLRWDT();
        NOP();
    }

    WREN = 0; // Disable writes to EEPROM on write complete (EEIF flag on set PIR2 )
    EEIF = 0;   //Clear EEPROM write complete flag. (must be cleared in software. So we do it here)

}

// This function reads data from address given in internal EEPROM of PIC
unsigned char iEEgetc(unsigned char address)
{
    EEADR = address;
//    EEPGD = 0; // 0 = Access data EEPROM memory
//    CFGS = 0;  // 0 = Access Flash program or DATA EEPROM memory
    EECON1bits.RD = 1;    // EEPROM Read
    return EEDATA;        // return data
}
