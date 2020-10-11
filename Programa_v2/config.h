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
#ifndef CONFIG_H
#define	CONFIG_H

/* Para funciones Delay de la librería __delay_ms y __delay_us */
#define _XTAL_FREQ 3686400

#define INCLUDE_LCD
#define INCLUDE_INFO_SCREEN
//#define INCLUDE_ADC
//#define DEBUG_ADC
//#define INCLUDE_WIEGAND

#define DEFAULT_BAUDRATE 0x17;        /* 9600 bps */
#define DEFAULT_ADDRESS  0xFE;

#endif	/* CONFIG_H */

