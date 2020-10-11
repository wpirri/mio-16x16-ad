/***************************************************************************
    Copyright (C) 2017   Walter Pirri

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
#ifndef LCD_H
#define	LCD_H

#include "config.h"

#ifdef INCLUDE_LCD

/* Formato del display */
#define LCD_LINES   4
#define LCD_COLS    20

/* L�neas de control */
#define LCD_EN      RB1     /* EN = 0: Disable, EN = 1: Enable */
#define LCD_EN_TRIS TRISB1
#define LCD_RS      RB2     /* RS = 0: Comando, RS = 1: Dato */
#define LCD_RS_TRIS TRISB2
#define LCD_RW      RB3     /* RW = 0: Write, RW = 1: Read */
#define LCD_RW_TRIS TRISB3

/* Bus de datos */
#define LCD_PORT_DATA   PORTD
#define LCD_TRIS_DATA   TRISD
/* Seleccionar el yipo de bus descomentando una de las siguientes lineas */
//#define LCD_BUS8BITS
#define LCD_BUS4BITS_HIGH
//#define LCD_BUS4BITS_LOW

void LCDInit( void );
unsigned char LCDOk( void );
void LCDClear( void );
void LCDMsg(unsigned char linea, unsigned char columna, const char* msg);

#endif /* INCLUDE_LCD */

#endif	/* LCD_H */

