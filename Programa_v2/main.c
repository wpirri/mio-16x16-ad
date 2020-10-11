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

/* *****************************************************************************
 * MIO-16x16-AD.
 * Interface de domotica remota via serial RS-232
 * *****************************************************************************
 * PIC16F877A/887
 * Clock: Xtal 3,6864 MHz
 * div interna /4 /256 (normal) = 278us
 * prescaler asignado a WDT
 * divisores timer(3,6864 Mhz) / 4 / 256 / 36 / 100
 *                                          \_ 10ms
 *                                                \_ 1 seg
 * ciclo de instruccion  1,08us
 * int timer             278us
 * *****************************************************************************
 * Pines
 *
 *   PORT     BIT     PIN    IN/OUT     DESCRIPCION
 * ---------------------------------------------------------------------------
 *    A        0        2       AD      Temp. on board
 *    A        1        3       AD      A/D Ext. 1
 *    A        2        4       AD      A/D Ext. 2
 *    A        3        5       AD      A/D Ext. 3
 *    A        4        6       I       Dig Input 1
 *    A        5        7       O       i2C SPI ss
 *    A        6        14
 *    A        7        13
 *    B        0        33      I       INT
 *    B        1        34      O       Display EN
 *    B        2        35      O       Display RS
 *    B        3        36      O       Display RW
 *    B        4        37      O       Disable entrada 1
 *    B        5        38      O       Enable salida 2
 *    B        6        39      O       Enable salida 1
 *    B        7        40      O       Disable entrada 2
 *    C        0        15      O/I     PWM CK  - Wiegand D0
 *    C        1        16      O/I     PWM 2   - Wiegand D1
 *    C        2        17      O       PWM 1   - Wiegand Led
 *    C        3        18      O       I2C/SPI - SCK/SCL
 *    C        4        23      I/O     I2C/SPI - SDI/SDA
 *    C        5        24      O       I2C/SPI - SDO
 *    C        6        25      O       Serial Tx
 *    C        7        26      I       Serial Rx
 *    D        0        19      I/O     Data Bus
 *    D        1        20      I/O     Data Bus
 *    D        2        21      I/O     Data Bus
 *    D        3        22      I/O     Data Bus
 *    D        4        27      I/O     Data Bus
 *    D        5        28      I/O     Data Bus
 *    D        6        29      I/O     Data Bus
 *    D        7        30      I/O     Data Bus
 *    E        0        8       O       Status Led
 *    E        1        9       O       Buzzer
 *    E        2        10      O       Disable Keyboard
 * 
 * 
 * ************************************************************************** */
#pragma config CPD = OFF
#pragma config CP = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF
#pragma config PWRTE = ON       /* Power-on reset activado */
#pragma config WDTE = ON        /* WDT encendido */
#pragma config FOSC = XT
/* ************************************************************************** */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <xc.h>

#include "config.h"

#include "adcsens.h"
#include "iee.h"
#include "ioports.h"
#include "lcd.h"
#include "minipow.h"
#include "perif.h"
#include "serial.h"
#include "wiegand.h"

/* *************************************************************************** *
 * Globales
 */
unsigned char TimerDiv1;
unsigned char TimerDiv2;
bit TimerFlag1;
bit TimerFlag2;

bit OnLineFlag;

unsigned char RetryMsg;
unsigned char PauseMsg;
unsigned char LastCntRcv;
unsigned char Blink;

unsigned char InitTimer;
unsigned char SaveConfigTimer;
unsigned char SaveStatusTimer;
unsigned char RebootTimer;

serial_msg SerialMsg;
serial_msg LastMsgSend;

#define FAST_BLINK      (Blink & 0b00001000)
#define SLOW_BLINK      (Blink & 0b00100000)

void LoadConfig( void )
{
    /* Posiciones 0 a 127 para configuracion */
    if(iEEgetc(0) != 0x5A)
    {
        /* Cargo el default */
        IOPORTCONF.hw_address = DEFAULT_ADDRESS;
        IOPORTCONF.io[0].mascara_seguidor = 0x00;
        IOPORTCONF.io[0].mascara_notif = 0xff;
        IOPORTCONF.io[1].mascara_seguidor = 0x00;
        IOPORTCONF.io[1].mascara_notif = 0xff;
        IOPORTCONF.io[2].mascara_notif = 0xff;
        IOPORTCONF.io[3].mascara_notif = 0xff;
        IOPORTCONF.analog[0].maxh = 0x03;
        IOPORTCONF.analog[0].maxl = 0xFF;
        IOPORTCONF.analog[0].minh = 0x00;
        IOPORTCONF.analog[0].minl = 0x00;
        IOPORTCONF.analog[1].maxh = 0x03;
        IOPORTCONF.analog[1].maxl = 0xFF;
        IOPORTCONF.analog[1].minh = 0x00;
        IOPORTCONF.analog[1].minl = 0x00;
        IOPORTCONF.analog[2].maxh = 0x03;
        IOPORTCONF.analog[2].maxl = 0xFF;
        IOPORTCONF.analog[2].minh = 0x00;
        IOPORTCONF.analog[2].minl = 0x00;
        IOPORTCONF.analog[3].maxh = 0x03;
        IOPORTCONF.analog[3].maxl = 0xFF;
        IOPORTCONF.analog[3].minh = 0x00;
        IOPORTCONF.analog[3].minl = 0x00;
        IOPORTCONF.mascara_seguidor_an = 0x00;
        IOPORTCONF.mascara_notif_an = 0x00;
        IOPORTCONF.baud_rate = DEFAULT_BAUDRATE;
        SaveConfigTimer = 1;
    }
    else
    {
        /* Leo la configuracion de la EE */
        IOPORTCONF.hw_address = iEEgetc(1);
        IOPORTCONF.io[0].mascara_seguidor = iEEgetc(2);
        IOPORTCONF.io[0].mascara_notif = iEEgetc(3);
        IOPORTCONF.io[1].mascara_seguidor = iEEgetc(4);
        IOPORTCONF.io[1].mascara_notif = iEEgetc(5);
        IOPORTCONF.io[2].mascara_notif = iEEgetc(6);
        IOPORTCONF.io[3].mascara_notif = iEEgetc(7);
        IOPORTCONF.analog[0].maxh = iEEgetc(8);
        IOPORTCONF.analog[0].maxl = iEEgetc(9);
        IOPORTCONF.analog[0].minh = iEEgetc(10);
        IOPORTCONF.analog[0].minl = iEEgetc(11);
        IOPORTCONF.analog[1].maxh = iEEgetc(12);
        IOPORTCONF.analog[1].maxl = iEEgetc(13);
        IOPORTCONF.analog[1].minh = iEEgetc(14);
        IOPORTCONF.analog[1].minl = iEEgetc(15);
        IOPORTCONF.analog[2].maxh = iEEgetc(16);
        IOPORTCONF.analog[2].maxl = iEEgetc(17);
        IOPORTCONF.analog[2].minh = iEEgetc(18);
        IOPORTCONF.analog[2].minl = iEEgetc(19);
        IOPORTCONF.analog[3].maxh = iEEgetc(20);
        IOPORTCONF.analog[3].maxl = iEEgetc(21);
        IOPORTCONF.analog[3].minh = iEEgetc(22);
        IOPORTCONF.analog[3].minl = iEEgetc(23);
        IOPORTCONF.mascara_seguidor_an = iEEgetc(24);
        IOPORTCONF.mascara_notif_an = iEEgetc(25);
        IOPORTCONF.baud_rate = iEEgetc(26);
    }
}

void SaveConfig( void )
{
    iEEputc(0, 0x5A);
    iEEputc(1, IOPORTCONF.hw_address);
    iEEputc(2, IOPORTCONF.io[0].mascara_seguidor);
    iEEputc(3, IOPORTCONF.io[0].mascara_notif);
    iEEputc(4, IOPORTCONF.io[1].mascara_seguidor);
    iEEputc(5, IOPORTCONF.io[1].mascara_notif);
    iEEputc(6, IOPORTCONF.io[2].mascara_notif);
    iEEputc(7, IOPORTCONF.io[3].mascara_notif);
    iEEputc(8, IOPORTCONF.analog[0].maxh);
    iEEputc(9, IOPORTCONF.analog[0].maxl);
    iEEputc(10, IOPORTCONF.analog[0].minh);
    iEEputc(11, IOPORTCONF.analog[0].minl);
    iEEputc(12, IOPORTCONF.analog[1].maxh);
    iEEputc(13, IOPORTCONF.analog[1].maxl);
    iEEputc(14, IOPORTCONF.analog[1].minh);
    iEEputc(15, IOPORTCONF.analog[1].minl);
    iEEputc(16, IOPORTCONF.analog[2].maxh);
    iEEputc(17, IOPORTCONF.analog[2].maxl);
    iEEputc(18, IOPORTCONF.analog[2].minh);
    iEEputc(19, IOPORTCONF.analog[2].minl);
    iEEputc(20, IOPORTCONF.analog[3].maxh);
    iEEputc(21, IOPORTCONF.analog[3].maxl);
    iEEputc(22, IOPORTCONF.analog[3].minh);
    iEEputc(23, IOPORTCONF.analog[3].minl);
    iEEputc(24, IOPORTCONF.mascara_seguidor_an);
    iEEputc(25, IOPORTCONF.mascara_notif_an);
    iEEputc(26, IOPORTCONF.baud_rate);
}

void LoadStatus( void )
{
    /* Posiciones 128 a 255 para estados de ports */
    if(iEEgetc(128) != 0xA5)
    {
        IOPORT.io[0].status = 0;
        IOPORT.io[1].status = 0;
        IOPORT.io[2].status = 0;
        IOPORT.io[3].status = 0;
        IOPORT.analog[0].alarm = 0;
        IOPORT.analog[1].alarm = 0;
        IOPORT.analog[2].alarm = 0;
        IOPORT.analog[3].alarm = 0;
        IOPORT.pulse[0] = 0;
        IOPORT.pulse[1] = 0;
        IOPORT.pulse[2] = 0;
        IOPORT.pulse[3] = 0;
        IOPORT.pulse[4] = 0;
        IOPORT.pulse[5] = 0;
        IOPORT.pulse[6] = 0;
        IOPORT.pulse[7] = 0;
        IOPORT.pulse[8] = 0;
        IOPORT.pulse[9] = 0;
        IOPORT.pulse[0] = 0;
        IOPORT.pulse[1] = 0;
        IOPORT.pulse[2] = 0;
        IOPORT.pulse[3] = 0;
        IOPORT.pulse[4] = 0;
        IOPORT.pulse[5] = 0;
    }
    else
    {
        IOPORT.io[0].status = iEEgetc(129);
        IOPORT.io[1].status = iEEgetc(130);
        IOPORT.io[2].status = iEEgetc(131);
        IOPORT.io[3].status = iEEgetc(132);
        IOPORT.analog[0].alarm = iEEgetc(133);
        IOPORT.analog[1].alarm = iEEgetc(134);
        IOPORT.analog[2].alarm = iEEgetc(135);
        IOPORT.analog[3].alarm = iEEgetc(136);
        IOPORT.pulse[0] = iEEgetc(137);
        IOPORT.pulse[1] = iEEgetc(138);
        IOPORT.pulse[2] = iEEgetc(139);
        IOPORT.pulse[3] = iEEgetc(140);
        IOPORT.pulse[4] = iEEgetc(141);
        IOPORT.pulse[5] = iEEgetc(142);
        IOPORT.pulse[6] = iEEgetc(143);
        IOPORT.pulse[7] = iEEgetc(144);
        IOPORT.pulse[8] = iEEgetc(145);
        IOPORT.pulse[9] = iEEgetc(146);
        IOPORT.pulse[0] = iEEgetc(147);
        IOPORT.pulse[1] = iEEgetc(148);
        IOPORT.pulse[2] = iEEgetc(149);
        IOPORT.pulse[3] = iEEgetc(150);
        IOPORT.pulse[4] = iEEgetc(151);
        IOPORT.pulse[5] = iEEgetc(152);
    }
    /* Para que notifique las entradas al ponerse el línea después de un corte  */
    IOPORT.io[0].last_status = ~IOPORT.io[0].status;
    IOPORT.io[1].last_status = ~IOPORT.io[1].status;
    IOPORT.analog[0].last_alarm = ~IOPORT.analog[0].alarm;
    IOPORT.analog[1].last_alarm = ~IOPORT.analog[1].alarm;
    IOPORT.analog[2].last_alarm = ~IOPORT.analog[2].alarm;
    IOPORT.analog[3].last_alarm = ~IOPORT.analog[3].alarm;
    /* Para que actualice el estado a la salida */
    IOPORT.io[2].last_status = ~IOPORT.io[2].status;
    IOPORT.io[3].last_status = ~IOPORT.io[3].status;
}

void SaveStatus( void )
{
    iEEputc(128, 0xA5); 
    iEEputc(129, IOPORT.io[0].status);
    iEEputc(130, IOPORT.io[1].status);
    iEEputc(131, IOPORT.io[2].status);
    iEEputc(132, IOPORT.io[3].status);
    iEEputc(133, IOPORT.analog[0].alarm);
    iEEputc(134, IOPORT.analog[1].alarm);
    iEEputc(135, IOPORT.analog[2].alarm);
    iEEputc(136, IOPORT.analog[3].alarm);
    iEEputc(137, IOPORT.pulse[0]);
    iEEputc(138, IOPORT.pulse[1]);
    iEEputc(139, IOPORT.pulse[2]);
    iEEputc(140, IOPORT.pulse[3]);
    iEEputc(141, IOPORT.pulse[4]);
    iEEputc(142, IOPORT.pulse[5]);
    iEEputc(143, IOPORT.pulse[6]);
    iEEputc(144, IOPORT.pulse[7]);
    iEEputc(145, IOPORT.pulse[8]);
    iEEputc(146, IOPORT.pulse[9]);
    iEEputc(147, IOPORT.pulse[10]);
    iEEputc(148, IOPORT.pulse[11]);
    iEEputc(149, IOPORT.pulse[12]);
    iEEputc(150, IOPORT.pulse[13]);
    iEEputc(151, IOPORT.pulse[14]);
    iEEputc(152, IOPORT.pulse[15]);
}

#ifdef INCLUDE_INFO_SCREEN
void PrintDiaSem(unsigned char dia, char* str)
{
    switch(dia)
    {
        case 1:
            str[0] = 'L';
            str[1] = 'u';
            str[2] = 'n';
            break;
        case 2:
            str[0] = 'M';
            str[1] = 'a';
            str[2] = 'r';
            break;
        case 3:
            str[0] = 'M';
            str[1] = 'i';
            str[2] = 'e';
            break;
        case 4:
            str[0] = 'J';
            str[1] = 'u';
            str[2] = 'e';
            break;
        case 5:
            str[0] = 'V';
            str[1] = 'i';
            str[2] = 'e';
            break;
        case 6:
            str[0] = 'S';
            str[1] = 'a';
            str[2] = 'b';
            break;
        case 7:
            str[0] = 'D';
            str[1] = 'o';
            str[2] = 'm';
            break;
    }
}

void PrintMes(unsigned char mes, char* str)
{
    switch(mes)
    {
        case 1:
            str[0] = 'E';
            str[1] = 'n';
            str[2] = 'e';
            break;
        case 2:
            str[0] = 'F';
            str[1] = 'e';
            str[2] = 'b';
            break;
        case 3:
            str[0] = 'M';
            str[1] = 'a';
            str[2] = 'r';
            break;
        case 4:
            str[0] = 'A';
            str[1] = 'b';
            str[2] = 'r';
            break;
        case 5:
            str[0] = 'M';
            str[1] = 'a';
            str[2] = 'y';
            break;
        case 6:
            str[0] = 'J';
            str[1] = 'u';
            str[2] = 'n';
            break;
        case 7:
            str[0] = 'J';
            str[1] = 'u';
            str[2] = 'l';
            break;
        case 8:
            str[0] = 'A';
            str[1] = 'g';
            str[2] = 'o';
            break;
        case 9:
            str[0] = 'S';
            str[1] = 'e';
            str[2] = 'p';
            break;
        case 10:
            str[0] = 'O';
            str[1] = 'c';
            str[2] = 't';
            break;
        case 11:
            str[0] = 'N';
            str[1] = 'o';
            str[2] = 'v';
            break;
        case 12:
            str[0] = 'D';
            str[1] = 'i';
            str[2] = 'c';
            break;
    }
}

void PrintInfoScreen( void )
{
    char str[19];

    /* " Mie 14-Sep   14:04 " */

    PrintDiaSem(gFechaHora.fecha.dia + 1, str);
    str[3] = ' ';
    if(gFechaHora.fecha.mes < 10)
    {
        str[4] = '0';
        utoa(&str[5], gFechaHora.fecha.mes, 10);
    }
    else
    {
        utoa(&str[4], gFechaHora.fecha.mes, 10);
    }
    str[6] = '-';
    PrintMes(gFechaHora.fecha.mes + 1, &str[7]);
    str[10] = ' ';
    str[11] = ' ';
    str[12] = ' ';
    if(gFechaHora.hora.horas < 10)
    {
        str[13] = '0';
        utoa(&str[14], gFechaHora.hora.horas, 10);
    }
    else
    {
        utoa(&str[13], gFechaHora.hora.horas, 10);
    }
    str[15] = ':';
    if(gFechaHora.hora.minutos < 10)
    {
        str[16] = '0';
        utoa(&str[17], gFechaHora.hora.minutos, 10);
    }
    else
    {
        utoa(&str[16], gFechaHora.hora.minutos, 10);
    }
    str[18] = '\0';
    LCDMsg(1, 2, str);
}
#endif /* INCLUDE_INFO_SCREEN */

#ifdef INCLUDE_ADC
void LCDInfoADC( unsigned char adc)
{
    char str[11];
    unsigned int ui;
    unsigned char i;

    str[0] = 'A';
    str[1] = 'D';
    str[2] = '0'+ adc;
    str[3] = ':';
    ui = (IOPORT.analog[adc-1].statush * 256) + IOPORT.analog[adc-1].statusl;
    if(ui > 999)
    {
        i = 4;
    }
    else if(ui > 99)
    {
        str[4] = ' ';
        i = 5;
    }
    else if(ui > 9)
    {
        str[4] = ' ';
        str[5] = ' ';
        i = 6;
    }
    else
    {
        str[4] = ' ';
        str[5] = ' ';
        str[6] = ' ';
        i = 7;
    }
    utoa(&str[i], ui, 10);
    switch(adc)
    {
        case 1:
            LCDMsg(3, 1, str);
            break;
        case 2:
            LCDMsg(3, 10, str);
            break;
        case 3:
            LCDMsg(4, 1, str);
            break;
        case 4:
            LCDMsg(4, 10, str);
            break;
    }
}
#endif /*INCLUDE_ADC*/

/* *************************************************************************** *
 * Funciones llamadas dentro de interrupcion
   *** Si se usa % dentro de interrupci�n da overflow el stack ***
 */



/* *************************************************************************** *
 * Interrupciones
 */
void Timer0Int( void )
{
    T0IF = 0;
    if( !T0IE) return;
    
    if( !TimerDiv1) TimerDiv1 = 36;
    if( !TimerDiv2) TimerDiv2 = 100;
    
    TimerDiv1--;
    if( !TimerDiv1)
    {
        TimerFlag1 = 1;
        TimerDiv2--;
        if( !TimerDiv2)
        {
            /* 1s */
            TimerFlag2 = 1;
#ifdef INCLUDE_LCD
            /* Mantengo el reloj */
            gFechaHora.hora.segundos++;
            if(gFechaHora.hora.segundos > 59)
            {
                gFechaHora.hora.segundos = 0;
                gFechaHora.hora.minutos++;
                if(gFechaHora.hora.minutos > 59)
                {
                    gFechaHora.hora.minutos = 0;
                    gFechaHora.hora.horas++;
                    if(gFechaHora.hora.horas > 23)
                    {
                        gFechaHora.hora.horas = 0;
                        gFechaHora.fecha.dia++;
                        if(gFechaHora.fecha.dia > 6)
                        {
                            gFechaHora.fecha.dia = 0;
                        }
                        gFechaHora.fecha.fecha++;
                        if( (gFechaHora.fecha.fecha > 28 && gFechaHora.fecha.mes == 1 /*&& !(gFechaHora.fecha.anio%4)*/ ) ||
                            (gFechaHora.fecha.fecha > 29 && gFechaHora.fecha.mes == 1) ||
                            (gFechaHora.fecha.fecha > 30 && ( gFechaHora.fecha.mes == 3 || gFechaHora.fecha.mes == 5 || gFechaHora.fecha.mes == 8 || gFechaHora.fecha.mes == 10) ) ||
                            (gFechaHora.fecha.fecha > 31 && gFechaHora.fecha.mes == 1) )
                        {
                            gFechaHora.fecha.fecha = 1;
                            gFechaHora.fecha.mes++;
                            if(gFechaHora.fecha.mes > 11)
                            {
                                gFechaHora.fecha.anio = 0;
                                if(gFechaHora.fecha.anio > 99)
                                {
                                    gFechaHora.fecha.anio = 0;
                                }
                            }
                        }
                    }
                }
            }
            /* Fin Reloj */
#endif /* INCLUDE_LCD */
        }
    }
}

//void Timer1Int( void )
//{
//    TMR1IF = 0;
//    if( !TMR1IE) return;
//
//    
//}

//void ExterInt( void )
//{
//    INTF = 0;
//    if( !INTE) return;
//    
//    
//}

void SerialInt(void)
{
    unsigned char ch;
    
    RCIF = 0;
    ch = RCREG;
    if( !RCIE) return;
    
    SerRecv(ch);
}

/* *************************************************************************** *
 * Punto de interrupcion
 */
void interrupt isr(void)
{
//    GIE = 0;  << NO SE DEBE HACER >>
    if(T0IF)    { Timer0Int(); }
//    if(TMR1IF)  { Timer1Int(); }
//    if(INTF)    { ExterInt();  }
    if(RCIF)    { SerialInt();  }
//    GIE = 1;  << NO SE DEBE HACER >>
}

/* *************************************************************************** *
 * MAIN
 */
void main(void)
{
    unsigned char i;
/*
    bit 7 RBPU: PORTB Pull-up Enable bit
    *   1 = PORTB pull-ups are disabled
        0 = PORTB pull-ups are enabled by individual PORT latch values
    bit 6 INTEDG: Interrupt Edge Select bit
        1 = Interrupt on rising edge of INT pin
    *   0 = Interrupt on falling edge of INT pin
    bit 5 T0CS: Timer0 Clock Source Select bit
        1 = Transition on T0CKI pin
    *   0 = Internal instruction cycle clock (F OSC /4)
    bit 4 T0SE: Timer0 Source Edge Select bit
        1 = Increment on high-to-low transition on T0CKI pin
    *   0 = Increment on low-to-high transition on T0CKI pin
    bit 3 PSA: Prescaler Assignment bit
        1 = Prescaler is assigned to the WDT
    *   0 = Prescaler is assigned to the Timer0 module
    bit 2-0 PS<2:0>: Prescaler Rate Select bits
        1:256
*/
    OPTION_REG = 0b10001111;
    /* Ports I/O */
    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    PORTE = 0x00;
    TRISA = 0b11101111;
    TRISB = 0b00000001;
    TRISC = 0b10000000;
    TRISD = 0b11111111;
    TRISE = 0b00000000;
    /* Interrupci�n del TIMER0 */
    INTCON = 0x00;
    T0IE = 1;       /* Interrupcion del timer 0 */
    INTE = 1;       /* Ext Int */
    PEIE = 1;       /* Interrupcion de Perifericos */

    /* Configuracion Analogica */
    ANSEL  = 0b00000000;
    ANSELH = 0b00000000;
    /*
    bit 7-6 ADCS<1:0>: A/D Conversion Clock Select bits
        00 = F OSC /2
        01 = F OSC /8
        10 = F OSC /32
        11 = F RC (clock derived from a dedicated internal oscillator = 500 kHz max)
    bit 5-2 CHS<3:0>: Analog Channel Select bits
        0000 = AN0
        0001 = AN1
        0010 = AN2
        0011 = AN3
        0100 = AN4
        0101 = AN5
        0110 = AN6
        0111 = AN7
        1000 = AN8
        1001 = AN9
        1010 = AN10
        1011 = AN11
        1100 = AN12
        1101 = AN13
        1110 = CV REF
        1111 = Fixed Ref (0.6V fixed voltage reference)
    bit 1 GO/DONE: A/D Conversion Status bit
        1 = A/D conversion cycle in progress. Setting this bit starts an A/D conversion cycle.
        This bit is automatically cleared by hardware when the A/D conversion has completed.
        0 = A/D conversion completed/not in progress
    bit 0 ADON: ADC Enable bit
        1 = ADC is enabled
        0 = ADC is disabled and consumes no operating current
*/
    ADCON0 = 0b00000000;
/*
    bit 7 ADFM: A/D Conversion Result Format Select bit
        1 = Right justified
        0 = Left justified
    bit 6 Unimplemented: Read as ?0?
    bit 5 VCFG1: Voltage Reference bit
        1 = V REF - pin
        0 = V SS
    bit 4 VCFG0: Voltage Reference bit
        1 = V REF + pin
        0 = V DD
    bit 3-0 Unimplemented: Read as ?0?
*/
    ADCON1 = 0b00000000;
    
    CCP1CON = 0b00000000;
    CCP2CON = 0b00000000;
    WPUB = 0b00000000;
    IOCB = 0b00000000;
    CM1CON0 = 0b00000000;
    CM2CON0 = 0b00000000;
    CM2CON1 = 0b00000000;

    TimerDiv1 = 0;
    TimerDiv2 = 0;
    TimerFlag1 = 0;
    TimerFlag2 = 0;
    InitTimer = 5;
    SaveConfigTimer = 0;
    SaveStatusTimer = 0;
    RetryMsg = 0;
    PauseMsg = 30;
    LastCntRcv = 0xFF;
    OnLineFlag = 0;
    Blink = 0;
    RebootTimer = 0;
#ifdef INCLUDE_LCD
    gFechaHora.fecha.anio = 0;
    gFechaHora.fecha.mes = 0;
    gFechaHora.fecha.dia = 0;
    gFechaHora.fecha.fecha = 1;
    gFechaHora.hora.horas = 0;
    gFechaHora.hora.minutos = 0;
    gFechaHora.hora.segundos = 0;
#endif /* INCLUDE_LCD */

    LoadConfig();

    IOInit();
    PerifInit();
    SerInit();
#ifdef INCLUDE_LCD
    LCDInit();
#endif
#ifdef INCLUDE_ADC
    ADCInit();
#endif
#ifdef INCLUDE_WIEGAND
    WGND_Init();
#endif
    
    Beep(1);
    PORTE |= 0b00000001;    /* Led de status -> on */
    
    GIE = 1;
     
    /* Loop del programa */
    while(1)
    {
        CLRWDT();
        
        IOTask();
        PerifTask();
        SerTask();
#ifdef INCLUDE_ADC
        ADCTask();
#endif
#ifdef INCLUDE_WIEGAND
        WGND_IntData( RC0, RC1 );
#endif
        /* *****************************************************************
         * Timer de 10 milisegundos fuera de interrupción
         */
        if(TimerFlag1)
        {
            TimerFlag1 = 0;

            Blink++;
            SerTimer();
            PerifTimer10ms();
#ifdef INCLUDE_WIEGAND
            WGND_Timer();
#endif
        }

        /* *****************************************************************
         * Timer de 1 segundo fuera de interrupción
         */
        if(TimerFlag2)
        {
            TimerFlag2 = 0;
            /* Retardo de arranque */
            if(InitTimer)
            {
                InitTimer--;
                if(InitTimer == 2)
                {
                    Beep(2);
                    LoadStatus();
                }
            }
            /* Temporizador para grabar configuración */
            if(SaveConfigTimer)
            {
                SaveConfigTimer--;
                if( !SaveConfigTimer)
                {
                    SaveConfig();
                }
            }
            /* Temporizador para grabar cabios de estado */
            if(SaveStatusTimer)
            {
                SaveStatusTimer--;
                if( !SaveStatusTimer)
                {
                    SaveStatus();
                }
            }
            /* Temporizador para el reinicio */
            if(RebootTimer)
            {
                /* Si tengo pendientes grabación de configuración o de estado
                 * espero a que se hagan
                 */
                if( !SaveConfigTimer && !SaveStatusTimer)
                {
                    RebootTimer--;
                    if( !RebootTimer) while(1) { NOP(); }
                }
            }
            /* Mantengo el contador que evita saturación del canal serial */
            if(PauseMsg) PauseMsg--;
            /* Llamo a los timers de los demás módulos */
            IOTimer();
            PerifTimer1s();
#ifdef INCLUDE_ADC
            ADCTimer();
#endif
            
#ifdef INCLUDE_LCD
#ifdef DEBUG_ADC
            LCDInfoADC(1);
            LCDInfoADC(2);
            LCDInfoADC(3);
            LCDInfoADC(4);
#else
#ifdef INCLUDE_INFO_SCREEN
            PrintInfoScreen();
#endif /* INCLUDE_INFO_SCREEN */
#endif /* DEBUG_ADC */
#endif /* INCLUDE_LCD */
        }

        /* Si no pasó el tiempo de arranque corto acá */
        if(InitTimer) continue;

        /* Frecuencia de parpadeo del led de status */
        if(OnLineFlag)
        {
            if(FAST_BLINK)
                PORTE |= 0b00000001;
            else
                PORTE &= 0b11111110;
        }
        else
        {
            if(SLOW_BLINK)
                PORTE |= 0b00000001;
            else
                PORTE &= 0b11111110;
        }

        /* *****************************************************************
         * * Recepción de mensajes serial
         */
        if(SerReceive(&SerialMsg))
        {
            if(SerialMsg.xTyp == 'Q')
            {
                /* Proceso un requerimiento */
                switch(SerialMsg.xFcn)
                {
                    case 'P':   /* Ping */
                        /* Si sale con respuesta standard no hace falta hacer nada */
                        break;
                    case '3':   /* Set Bit */
                        if(LastCntRcv != SerialMsg.xCnt)
                        {
                            if(SerialMsg.xData[2] == '1') /* Encender */
                                IOPORT.io[SerialMsg.xData[0]-'1'+2].status |= minipow2(SerialMsg.xData[1]-'0');
                            else                    /* Apagar */
                                IOPORT.io[SerialMsg.xData[0]-'1'+2].status &= (minipow2(SerialMsg.xData[1]-'0') ^ 0xFF);
                            /* Se modifico una salida, en un minuto actualizo el estado */
                            SaveStatusTimer = 60;
                        }
                        break;
                    case '4':   /* IR Code */

                        break;
                    case '5':   /* Pulse */
                        if(LastCntRcv != SerialMsg.xCnt)
                        {
                            /* Me abro por salida */
                            switch(SerialMsg.xData[0])
                            {
                                case '1':   /* Salida 1 */
                                    IOPORT.io[2].status |= minipow2(SerialMsg.xData[1]-'0');
                                    IOPORT.pulse[SerialMsg.xData[1]-'0'] = SerialMsg.xData[2];
                                    break;
                                case '2':   /* Salida 2 */
                                    IOPORT.io[3].status |= minipow2(SerialMsg.xData[1]-'0');
                                    IOPORT.pulse[SerialMsg.xData[1]-'0'+8] = SerialMsg.xData[2];
                                    break;
                            }
                        }
                        break;
                    case '6':   /* Get Input */
                        SerialMsg.xData[1] = IOPORT.io[SerialMsg.xData[0]-'1'].status;
                        SerialMsg.xData[2] = IOPORTCONF.io[SerialMsg.xData[0]-'1'].mascara_notif;
                        SerialMsg.xData[3] = IOPORTCONF.io[SerialMsg.xData[0]-'1'].mascara_seguidor;
                        break;
                    case '7':   /* Get Out */
                        SerialMsg.xData[1] = IOPORT.io[SerialMsg.xData[0]-'1'+2].status;
                        SerialMsg.xData[2] = IOPORTCONF.io[SerialMsg.xData[0]-'1'+2].mascara_notif;
                        SerialMsg.xData[3] = IOPORTCONF.io[SerialMsg.xData[0]-'1'].mascara_seguidor;
                        SerialMsg.xData[4] = IOPORTCONF.mascara_seguidor_an;
                        break;
                    case '8':   /* Get Analog */
                        SerialMsg.xData[1] = IOPORT.analog[SerialMsg.xData[0]-'1'].statush;
                        SerialMsg.xData[2] = IOPORT.analog[SerialMsg.xData[0]-'1'].statusl;
                        SerialMsg.xData[3] = IOPORTCONF.analog[SerialMsg.xData[0]-'1'].maxh;
                        SerialMsg.xData[4] = IOPORTCONF.analog[SerialMsg.xData[0]-'1'].maxl;
                        SerialMsg.xData[5] = IOPORTCONF.analog[SerialMsg.xData[0]-'1'].minh;
                        SerialMsg.xData[6] = IOPORTCONF.analog[SerialMsg.xData[0]-'1'].minl;
                        SerialMsg.xData[7] = IOPORT.analog[SerialMsg.xData[0]-'1'].alarm;
                        SerialMsg.xData[8] = IOPORTCONF.mascara_notif_an;
                        SerialMsg.xData[9] = IOPORTCONF.mascara_seguidor_an;
                        break;
                    case 'I':   /* Invert */
                        if(LastCntRcv != SerialMsg.xCnt)
                        {
                            IOPORT.io[SerialMsg.xData[0]-'1'+2].status ^= minipow2(SerialMsg.xData[1]-'0');
                            /* Se modifico una salida, en un minuto actualizo el estado */
                            SaveStatusTimer = 60;
                        }
                        break;
                    case 'B':   /* Baud Rate */
                        if(SerialMsg.xData[0] == 0xBF ||
                           SerialMsg.xData[0] == 0x5F ||
                           SerialMsg.xData[0] == 0x17 ||
                           SerialMsg.xData[0] == 0x0B ||
                           SerialMsg.xData[0] == 0x03 ||
                           SerialMsg.xData[0] == 0x01  )
                        {
                            IOPORTCONF.baud_rate = SerialMsg.xData[0];
                            SaveConfigTimer = 1;
                            SaveStatusTimer = 1;
                            RebootTimer = 5;
                            Beep(20);
                        }
                        break;
                    case 'F':   /* Seguidores */
                        if(LastCntRcv != SerialMsg.xCnt)
                        {
                            /* Me abro por entrada */
                            switch(SerialMsg.xData[0])
                            {
                                case '1':   /* Entrada 1 */
                                    IOPORTCONF.io[0].mascara_seguidor = SerialMsg.xData[1];
                                    break;
                                case '2':   /* Entrada 2 */
                                    IOPORTCONF.io[1].mascara_seguidor = SerialMsg.xData[1];
                                    break;
                                case 'A':
                                    IOPORTCONF.mascara_seguidor_an = SerialMsg.xData[1];
                                    break;
                            }
                            SaveConfigTimer = 10;
                        }
                        break;
                    case 'O':   /* Mascaras de notificacion */
                        if(LastCntRcv != SerialMsg.xCnt)
                        {
                            /* Me abro por port */
                            switch(SerialMsg.xData[0])
                            {
                                case '1':   /* Entrada 1 */
                                    IOPORTCONF.io[0].mascara_notif = SerialMsg.xData[1];
                                    break;
                                case '2':   /* Entrada 2 */
                                    IOPORTCONF.io[1].mascara_notif = SerialMsg.xData[1];
                                    break;
                                case '3':   /* Salida 1 */
                                    IOPORTCONF.io[2].mascara_notif = SerialMsg.xData[1];
                                    break;
                                case '4':   /* Salida 2 */
                                    IOPORTCONF.io[3].mascara_notif = SerialMsg.xData[1];
                                    break;
                                case 'A':
                                    IOPORTCONF.mascara_notif_an = SerialMsg.xData[1];
                                    break;
                            }
                            SaveConfigTimer = 10;
                        }
                        break;
                    case 'A':   /* Alarma A/D */
                        if(LastCntRcv != SerialMsg.xCnt)
                        {
                            IOPORTCONF.analog[SerialMsg.xData[0]-'1'].maxh = SerialMsg.xData[1];
                            IOPORTCONF.analog[SerialMsg.xData[0]-'1'].maxl = SerialMsg.xData[1];
                            IOPORTCONF.analog[SerialMsg.xData[0]-'1'].minh = 0;
                            IOPORTCONF.analog[SerialMsg.xData[0]-'1'].minl = SerialMsg.xData[1];
                            SaveConfigTimer = 10;
                        }
                        break;
                    case 'C':   /* Beep */
                        Beep(SerialMsg.xData[0] - '0');
                        break;
#ifdef INCLUDE_LCD
                    case 'D':   /* MSG-1L */
                        SerialMsg.xSum = 0;
                        LCDMsg(1, 1, &SerialMsg.xData[0]);
                        break;
                    case 'E':   /* MSG-1R */
                        SerialMsg.xSum = 0;
                        LCDMsg(1, 20, &SerialMsg.xData[0]);
                        break;
                    case 'G':   /* MSG-2L */
                        SerialMsg.xSum = 0;
                        LCDMsg(2, 1, &SerialMsg.xData[0]);
                        break;
                    case 'H':   /* MSG-2R */
                        SerialMsg.xSum = 0;
                        LCDMsg(2, 20, &SerialMsg.xData[0]);
                        break;
                    case 'J':   /* MSG-3L */
                        SerialMsg.xSum = 0;
                        LCDMsg(3, 1, &SerialMsg.xData[0]);
                        break;
                    case 'K':   /* MSG-3R */
                        SerialMsg.xSum = 0;
                        LCDMsg(3, 20, &SerialMsg.xData[0]);
                        break;
                    case 'M':   /* MSG-4L */
                        SerialMsg.xSum = 0;
                        LCDMsg(4, 1, &SerialMsg.xData[0]);
                        break;
                    case 'N':   /* MSG-4R */
                        SerialMsg.xSum = 0;
                        LCDMsg(4, 20, &SerialMsg.xData[0]);
                        break;
                    case 'Q':   /* Display Config */

                        break;
#endif /* INCLUDE_LCD */
                    case 'L':   /* Set ADDR */
                        if(SerialMsg.xData[0] > 0 && SerialMsg.xData[0] < 254)
                        {
                            IOPORTCONF.hw_address = SerialMsg.xData[0];
                            SaveConfigTimer = 1;
                            SaveStatusTimer = 1;
                            RebootTimer = 5;
                            Beep(20);
                        }
                        break;
                    case 'R':   /* Reset */
                        SaveConfigTimer = 1;
                        SaveStatusTimer = 1;
                        RebootTimer = 5;
                        Beep(20);
                        break;
#ifdef INCLUDE_LCD
                    case 'T':   /* Set Time */
                        gFechaHora.fecha.anio = SerialMsg.xData[6];
                        gFechaHora.fecha.mes = SerialMsg.xData[5];
                        gFechaHora.fecha.dia = SerialMsg.xData[3];
                        gFechaHora.fecha.fecha = SerialMsg.xData[4];
                        gFechaHora.hora.horas = SerialMsg.xData[0];
                        gFechaHora.hora.minutos = SerialMsg.xData[1];
                        gFechaHora.hora.segundos = SerialMsg.xData[2];
                        break;
#endif /* INCLUDE_LCD */
                    default:
                        /* Mensaje desconocido, contesto error */
                        SerialMsg.xTyp = 'E'; 
                        break;
                }
                SerSendR(&SerialMsg);
                LastCntRcv = SerialMsg.xCnt;
            }
            else if(SerialMsg.xTyp == 'R')
            {
                /* Proceso una respuesta */
                if( SerialMsg.xCnt == LastMsgSend.xCnt &&
                    SerialMsg.xFcn == LastMsgSend.xFcn  )
                {
                    RetryMsg = 0;
                    PauseMsg = 0;
                    SerAck();
                    switch(SerialMsg.xFcn)
                    {
                        case '6':   /* Cambio de estado de entrada */
                            IOResetChanges(SerialMsg.xData[0], SerialMsg.xData[1]);
                            break;
                        case '7':   /* Cambio de estado de salida */
                            IOResetChanges(SerialMsg.xData[0]+2, SerialMsg.xData[1]);
                            break;
#ifdef INCLUDE_ADC
                        case '9':   /* Cambio de estado de entrada analogica */
                            ADCAlarmReset(SerialMsg.xData[0], SerialMsg.xData[1]);
                            break;
#endif
                        case 'P':   /* Ping */
                            if( !OnLineFlag)
                            {
                                OnLineFlag = 1;
                                Beep(5);
                                LCDMsg(3, 2, "Interface ON-Line");
                            }
                            break;
#ifdef INCLUDE_WIEGAND
                        case 'W':
                            WGND_Ack();
                            break;
#endif
                    }
                }

            }
        }

        /* *****************************************************************
         * Transmisión de mensajes de requerimientos serial.
         * Se hace solamente si la base contestó el PING (está OnLine)
         * Si no está OnLine solo se manda Ping
         */

        if( !PauseMsg)
        {
            if(OnLineFlag)
            {
                if(RetryMsg)
                {
                    /* Hay reintentos pendientes de envío */
                    RetryMsg--;
                    if( !RetryMsg)
                    {
                        /* Se llegó al límite de reintentos para un mensaje */
                        SerTimeOut();
                        OnLineFlag = 0;
                        LCDMsg(3, 2, "Interface OFF-Line");

                        switch(LastMsgSend.xFcn)
                        {
                            case '6':   /* Cambio de estado de entrada */
                                IOResetChanges(LastMsgSend.xData[0], LastMsgSend.xData[1]);
                                break;
                            case '7':   /* Cambio de estado de salida */
                                IOResetChanges(LastMsgSend.xData[0]+2, LastMsgSend.xData[1]);
                                break;
#ifdef INCLUDE_ADC
                            case '9':   /* Cambio de estado de entrada analogica */
                                ADCAlarmReset(LastMsgSend.xData[0], LastMsgSend.xData[1]);
                                break;
#endif
#ifdef INCLUDE_WIEGAND
                            case 'W':
                                WGND_Ack();
                                break;
#endif
                        }
                    }
                    else
                    {
                        /* Reenvio el ultimo mensaje */
                        SerSendQ(&LastMsgSend);
                        PauseMsg = 2;
                    }
                }
                else    /* RetryMsg */
                {
                    for(i = 0; i < 4; i++)
                    {
                        if(IOChanges('1' + i))
                        {
                            LastMsgSend.xFcn = (i < 2)?'6':'7';
                            LastMsgSend.xData[0] = (i & 0x01)?'2':'1';
                            LastMsgSend.xData[1] = IOPORT.io[i].status;
                            LastMsgSend.xData[2] = IOPORTCONF.io[i].mascara_notif;
                            LastMsgSend.xData[3] = IOPORTCONF.io[i].mascara_seguidor;
                            PauseMsg = 2;
                            RetryMsg = 10;
                            SerSendQ(&LastMsgSend);
                            break;
                        }
#ifdef INCLUDE_ADC
                        if(ADCAlarm('1' + i))
                        {
                            LastMsgSend.xFcn = '9';
                            LastMsgSend.xData[0] = '1' + i;
                            LastMsgSend.xData[1] = IOPORT.analog[i].alarm;
                            LastMsgSend.xData[2] = IOPORT.analog[i].statush;
                            LastMsgSend.xData[3] = IOPORT.analog[i].statusl;
                            LastMsgSend.xData[4] = IOPORTCONF.analog[i].maxh;
                            LastMsgSend.xData[5] = IOPORTCONF.analog[i].maxl;
                            LastMsgSend.xData[6] = IOPORTCONF.analog[i].minh;
                            LastMsgSend.xData[7] = IOPORTCONF.analog[i].minh;
                            LastMsgSend.xData[8] = IOPORTCONF.mascara_notif_an;
                            LastMsgSend.xData[9] = IOPORTCONF.mascara_seguidor_an;
                            PauseMsg = 2;
                            RetryMsg = 10;
                            SerSendQ(&LastMsgSend);
                            break;
                        }
#endif /* INCLUDE_ADC */
#ifdef INCLUDE_WIEGAND
                        if(gWGND_DataReady)
                        {
                            LastMsgSend.xFcn = 'W';
                            LastMsgSend.xData[0] = '0'; /* Key ID */
                            LastMsgSend.xData[1] = '1'; /* Key Part */
                            LastMsgSend.xData[2] = '1'; /* Key Parts */
                            LastMsgSend.xData[3] = 0x00; /* Filler */
                            LastMsgSend.xData[4] = gWGND_Data.udata.bytes.a;
                            LastMsgSend.xData[5] = gWGND_Data.udata.bytes.b;
                            LastMsgSend.xData[6] = gWGND_Data.udata.bytes.c;
                            LastMsgSend.xData[7] = gWGND_Data.udata.bytes.d;
                            LastMsgSend.xData[8] = 0x00;
                            LastMsgSend.xData[9] = 0x00;
                            PauseMsg = 2;
                            RetryMsg = 10;
                            SerSendQ(&LastMsgSend);
                        }
#endif
                    }
                }
            }
            else    /* OnLineFlag */
            {
                LastMsgSend.xFcn = 'P';
                LastMsgSend.xData[0] = 0x02;     /* MIO-16x16-A/D */
                LastMsgSend.xData[1] = 0x01;     /* HW V1.00 */
                LastMsgSend.xData[2] = 0x02;     /* SW V2.00 */
                LastMsgSend.xData[3] = 0x03;     /* UP PIC16F887 */
                LastMsgSend.xData[3] = 0x00;     /* No display */
                SerSendQ(&LastMsgSend);
                PauseMsg = 15;
                RetryMsg = 0;
                Beep(0);
            }
        }
        
        /* ***************************************************************** */
        
    }
}
