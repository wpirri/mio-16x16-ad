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
#include "lcd.h"
#ifdef INCLUDE_LCD

#include <stdio.h>
#include <stdlib.h>

#include <xc.h>

#define LINE1_ROOT  0x00
#define LINE2_ROOT  0x40
#define LINE3_ROOT  0x14
#define LINE4_ROOT  0x54

bit LCD_Ok;

void LCDGoto(unsigned char line, unsigned char col);
unsigned char LCDSend( unsigned char data, unsigned char cmd );

void LCDInit( void )
{
    LCD_Ok = 0;

    /* Bus de datos como entrada */
#ifdef LCD_BUS4BITS_HIGH
    LCD_TRIS_DATA |= 0xF0;
#endif
#ifdef LCD_BUS4BITS_LOW
    LCD_TRIS_DATA |= 0x0F;
#endif
#ifdef LCD_BUS8BITS
    LCD_TRIS_DATA = 0xFF;
#endif    
    LCD_EN_TRIS = 0;
    LCD_RW_TRIS = 0;
    LCD_RS_TRIS = 0;
    
    /* Inicializo el display */
    __delay_ms(50);             /* Power On delay */
    LCD_EN = 0;
    LCD_RS = 0;
    LCD_RW = 0;
    /* Bus de datos como salida */
#ifdef LCD_BUS4BITS_HIGH
    LCD_TRIS_DATA &= 0x0F;
    LCD_PORT_DATA &= 0x0F;
    LCD_PORT_DATA |= 0x30;
#endif
#ifdef LCD_BUS4BITS_LOW
    LCD_TRIS_DATA &= 0xF0;
    LCD_PORT_DATA &= 0xF0;
    LCD_PORT_DATA |= 0x03;
#endif
#ifdef LCD_BUS8BITS
    LCD_TRIS_DATA = 0x00;
    LCD_PORT_DATA = 0x38;
#endif
    LCD_EN = 1;
    NOP(); NOP(); NOP();
    LCD_EN = 0;
    __delay_ms(5);
    LCD_EN = 1;
    NOP(); NOP(); NOP();
    LCD_EN = 0;
    __delay_ms(5);
    LCD_EN = 1;
    NOP(); NOP(); NOP();
    LCD_EN = 0;
#ifdef LCD_BUS4BITS_HIGH
    LCD_TRIS_DATA &= 0x0F;
    LCD_PORT_DATA &= 0x0F;
    LCD_PORT_DATA |= 0x20;
#endif
#ifdef LCD_BUS4BITS_LOW
    LCD_TRIS_DATA &= 0xF0;
    LCD_PORT_DATA &= 0xF0;
    LCD_PORT_DATA |= 0x02;
#endif
    __delay_ms(5);
    LCD_EN = 1;
    NOP(); NOP(); NOP();
    LCD_EN = 0;

    __delay_ms(5);
    /* Ahora ya funciona el BUSY */
#ifdef LCD_BUS4BITS_HIGH
    if(LCDSend( 0b00101100, 1 )) return;    /* (1/0) 8/4 bit, 2/1 lines, 11/8 dots */
#endif
#ifdef LCD_BUS4BITS_LOW
    if(LCDSend( 0b00101100, 1 )) return;    /* (1/0) 8/4 bit, 2/1 lines, 11/8 dots */
#endif
#ifdef LCD_BUS8BITS
    if(LCDSend( 0b00111100, 1 )) return;    /* (1/0) 8/4 bit, 2/1 lines, 11/8 dots */
#endif    
    if(LCDSend( 0b00000001, 1 )) return;    /* Clear, cursor al inicio */
    if(LCDSend( 0b00000110, 1 )) return;    /* Cursor shift */
    if(LCDSend( 0b00001100, 1 )) return;    /* LCD On, cursor Off,Blink Off */


    if(LCDSend( 0b10000000, 1 )) return;    /* Direccion de escritura */

    LCD_Ok = 1;
}

unsigned char LCDOk( void )
{
    return (LCD_Ok)?1:0;
}

void LCDClear( void )
{
    if( !LCD_Ok) return;
    if(LCDSend( 0b00000001, 1 )) LCD_Ok = 0;
}

void LCDMsg(unsigned char linea, unsigned char columna, const char* msg)
{
    if( !LCD_Ok) return;
    
    LCDGoto(linea, columna);
    while(*msg && columna <= LCD_COLS)
    {
        if(LCDSend((unsigned char)(*msg), 0))
        {
            LCD_Ok = 0;
            break;
        }
        msg++;
        columna++;
    }
}

#ifdef LCD_BUS4BITS_HIGH
unsigned char LCDSend( unsigned char data, unsigned char cmd )
{
    unsigned char aux_byte;
    
    /* Me fijo si el display a listo para procesar */
    LCD_TRIS_DATA |= 0xF0;               /* Bus de datos como entrada */
    LCD_RW = 1;
    LCD_RS = 0;
    NOP(); NOP(); NOP();
    LCD_EN = 1;
    NOP(); NOP(); NOP();
    /* Bloque Wait -->  Si DB7 a encendido el display a ocupado */
    for(aux_byte = 255; aux_byte && (LCD_PORT_DATA & 0x80); aux_byte-- )
    {
        __delay_us(1);
    }
    if(LCD_PORT_DATA & 0x80)
    {
        /* Error en display */
        LCD_EN = 0;
        LCD_RW = 0;
        return 1;
    }
    /* <-- Bloque Wait */
    LCD_EN = 0;
    NOP(); NOP(); NOP();
    /* Le mando el dato / comando */
    LCD_RW = 0;
    if( !cmd) LCD_RS = 1;
    LCD_TRIS_DATA &= 0x0F;           /* Bus de datos como salida */

    aux_byte = LCD_PORT_DATA;
    aux_byte &= 0x0F;
    aux_byte |= (data & 0xF0);  /* Envío primero los 4 bits superiores */
    LCD_PORT_DATA = aux_byte;
    NOP(); NOP(); NOP();
    LCD_EN = 1;
    NOP(); NOP(); NOP();
    LCD_EN = 0;
    NOP(); NOP(); NOP();

    aux_byte = LCD_PORT_DATA;
    aux_byte &= 0x0F;
    aux_byte |= ((data << 4) & 0xF0); /* Envío los 4 bits inferiores */
    LCD_PORT_DATA = aux_byte;
    NOP(); NOP(); NOP();
    LCD_EN = 1;
    NOP(); NOP(); NOP();
    LCD_EN = 0;
    NOP(); NOP(); NOP();

    LCD_RW = 1;
    LCD_TRIS_DATA |= 0xF0;               /* Bus de datos como entrada */
    return 0;
}
#endif

#ifdef LCD_BUS4BITS_LOW
unsigned char LCDSend( unsigned char data, unsigned char cmd )
{
    unsigned char aux_byte;
    
    /* Me fijo si el display a listo para procesar */
    LCD_TRIS_DATA |= 0x0F;               /* Bus de datos como entrada */
    LCD_RW = 1;
    LCD_RS = 0;
    NOP(); NOP(); NOP();
    LCD_EN = 1;
    NOP(); NOP(); NOP();
    /* Bloque Wait -->  Si DB7 a encendido el display a ocupado */
    for(aux_byte = 255; aux_byte && (LCD_PORT_DATA & 0x08); aux_byte-- )
    {
        __delay_us(1);
    }
    if(LCD_PORT_DATA & 0x08)
    {
        /* Error en display */
        LCD_EN = 0;
        LCD_RW = 0;
        return 1;
    }
    /* <-- Bloque Wait */
    LCD_EN = 0;
    NOP(); NOP(); NOP();
    /* Le mando el dato / comando */
    LCD_RW = 0;
    if( !cmd) LCD_RS = 1;
    LCD_TRIS_DATA &= 0xF0;           /* Bus de datos como salida */

    aux_byte = LCD_PORT_DATA;
    aux_byte &= 0xF0;
    aux_byte |= ((data >> 4) & 0x0F); /* Envío los 4 bits superiores */
    LCD_PORT_DATA = aux_byte;
    NOP(); NOP(); NOP();
    LCD_EN = 1;
    NOP(); NOP(); NOP();
    LCD_EN = 0;
    NOP(); NOP(); NOP();

    aux_byte = LCD_PORT_DATA;
    aux_byte &= 0xF0;
    aux_byte |= (data & 0x0F);  /* Envío primero los 4 bits inferiores */
    LCD_PORT_DATA = aux_byte;
    NOP(); NOP(); NOP();
    LCD_EN = 1;
    NOP(); NOP(); NOP();
    LCD_EN = 0;
    NOP(); NOP(); NOP();

    LCD_RW = 1;
    LCD_TRIS_DATA |= 0x0F;               /* Bus de datos como entrada */
    return 0;
}
#endif

#ifdef LCD_BUS8BITS
unsigned char LCDSend( unsigned char data, unsigned char cmd )
{
    unsigned char aux_byte;
    
    /* Me fijo si el display a listo para procesar */
    LCD_TRIS_DATA = 0xFF;               /* Bus de datos como entrada */
    LCD_RW = 1;
    LCD_RS = 0;
    NOP(); NOP(); NOP();
    LCD_EN = 1;
    NOP(); NOP(); NOP();
    /* Bloque Wait -->  Si DB7 a encendido el display a ocupado */
    for(aux_byte = 255; aux_byte && (LCD_PORT_DATA & 0x80); aux_byte-- )
    {
        __delay_us(1);
    }
    if(LCD_PORT_DATA & 0x80)
    {
        /* Error en display */
        LCD_EN = 0;
        LCD_RW = 0;
        return 1;
    }
    /* <-- Bloque Wait */
    LCD_EN = 0;
    NOP(); NOP(); NOP();
    /* Le mando el dato / comando */
    LCD_RW = 0;
    if( !cmd) LCD_RS = 1;
    LCD_TRIS_DATA = 0x00;           /* Bus de datos como salida */
    LCD_PORT_DATA = data;
    NOP(); NOP(); NOP();
    LCD_EN = 1;
    NOP(); NOP(); NOP();
    LCD_EN = 0;
    NOP(); NOP(); NOP();
    LCD_RW = 1;
    LCD_TRIS_DATA = 0xff;               /* Bus de datos como entrada */
    return 0;
}
#endif

void LCDGoto(unsigned char line, unsigned char col)
{
    if(line < 1 || line > LCD_LINES) return;
    if(col < 1 || col > LCD_COLS) return;
    switch(line)
    {
        case 1:
            LCDSend(0x80 + LINE1_ROOT + col - 1, 1);
            break;
        case 2:
            LCDSend(0x80 + LINE2_ROOT + col - 1, 1);
            break;
        case 3:
            LCDSend(0x80 + LINE3_ROOT + col - 1, 1);
            break;
        case 4:
            LCDSend(0x80 + LINE4_ROOT + col - 1, 1);
            break;
    }
}

#endif /* INCLUDE_LCD */
