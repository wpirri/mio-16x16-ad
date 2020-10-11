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

#include "adcsens.h"

#ifdef INCLUDE_ADC

#include "ioports.h"

#define ADC_ALARM_DELAY 30

unsigned char ADCSTM;
unsigned char ADCInitTimer;

unsigned char ADCAlarmDelayCount[4];

void ADCInit( void )
{
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
    ADCON0 = 0x00;
    /*
        bit 7 ADFM: A/D Conversion Result Format Select bit
            1 = Right justified
            0 = Left justified
        bit 6 Unimplemented: Read as ‘0’
        bit 5 VCFG1: Voltage Reference bit
            1 = V REF - pin
            0 = V SS
        bit 4 VCFG0: Voltage Reference bit
            1 = V REF + pin
            0 = V DD
        bit 3-0 Unimplemented: Read as ‘0’
     */
    ADCON1 = 0x80;
    ANS0 = 1;
    ANS1 = 1;
    ANS2 = 1;
    ANS3 = 1;
    
    ADCS0 = 1;
    ADCS1 = 1;
    
    CHS0 = 0;
    CHS1 = 0;
    CHS2 = 0;
    
    ADON = 1;
    
    ADCSTM = 0;
    ADCInitTimer = 30;
    ADCAlarmDelayCount[0] = ADC_ALARM_DELAY / 2;
    ADCAlarmDelayCount[1] = ADC_ALARM_DELAY / 2;
    ADCAlarmDelayCount[2] = ADC_ALARM_DELAY / 2;
    ADCAlarmDelayCount[3] = ADC_ALARM_DELAY / 2;
}

void ADCTask( void )
{
    switch(ADCSTM)
    {
        case 0:             /* Selecciono AN0 */
            CHS0 = 0;
            CHS1 = 0;
            CHS2 = 0;
            CHS3 = 0;
            __delay_us(10);
            GO_nDONE = 1;   /* Inicio la conversión */
            ADCSTM++;
            break;
        case 1:             /* Espero que finalice */
            if( !GO_nDONE) ADCSTM++;
            break;
        case 2:             /* Leo resultado de AN0 */
            IOPORT.analog[0].statush = ADRESH;
            IOPORT.analog[0].statusl = ADRESL;
            ADCSTM++;
            break;
        /* ---------------------------------------- */
        case 3:             /* Selecciono AN1 */
            CHS0 = 1;
            CHS1 = 0;
            CHS2 = 0;
            CHS3 = 0;
            __delay_us(10);
            GO_nDONE = 1;   /* Inicio la conversión */
            ADCSTM++;
            break;
        case 4:             /* Espero que finalice */
            if( !GO_nDONE) ADCSTM++;
            break;
        case 5:             /* Leo resultado de AN1 */
            IOPORT.analog[1].statush = ADRESH;
            IOPORT.analog[1].statusl = ADRESL;
            ADCSTM++;
            break;
        /* ---------------------------------------- */
        case 6:             /* Selecciono AN2 */
            CHS0 = 0;
            CHS1 = 1;
            CHS2 = 0;
            CHS3 = 0;
            __delay_us(10);
            GO_nDONE = 1;   /* Inicio la conversión */
            ADCSTM++;
            break;
        case 7:             /* Espero que finalice */
            if( !GO_nDONE) ADCSTM++;
            break;
        case 8:             /* Leo resultado de AN2 */
            IOPORT.analog[2].statush = ADRESH;
            IOPORT.analog[2].statusl = ADRESL;
            ADCSTM++;
            break;
        /* ---------------------------------------- */
        case 9:             /* Selecciono AN3 */
            CHS0 = 1;
            CHS1 = 1;
            CHS2 = 0;
            CHS3 = 0;
            __delay_us(10);
            GO_nDONE = 1;   /* Inicio la conversión */
            ADCSTM++;
            break;
        case 10:             /* Espero que finalice */
            if( !GO_nDONE) ADCSTM++;
            break;
        case 11:             /* Leo resultado de AN3 */
            IOPORT.analog[3].statush = ADRESH;
            IOPORT.analog[3].statusl = ADRESL;
            ADCSTM++;
            break;
        /* ---------------------------------------- */
        default:
            ADCSTM = 0;
            break;
    }
}

void ADCTimer( void )
{
    if(ADCInitTimer) ADCInitTimer--;
    if(ADCInitTimer) return;
    
    /* Comparo los valores de ADC con los maximos y minimos */
    
    /* AN0 */
    if( ((IOPORT.analog[0].statush*256)+IOPORT.analog[0].statusl) >
        ((IOPORTCONF.analog[0].maxh*256)+IOPORTCONF.analog[0].maxl) )
    {
        if(ADCAlarmDelayCount[0] < ADC_ALARM_DELAY)
        {
            ADCAlarmDelayCount[0]++;
            if(ADCAlarmDelayCount[0] == ADC_ALARM_DELAY)
            {
                IOPORT.analog[0].alarm = 0xff;
                /* Seguidor AN -> OUT2 */
                if(IOPORTCONF.mascara_seguidor_an & 0b00000001)
                {
                    IOPORT.io[3].status |= 0b00000001;
                }
            }
        }
    }
    else if( ((IOPORT.analog[0].statush*256)+IOPORT.analog[0].statusl) <
             ((IOPORTCONF.analog[0].minh*256)+IOPORTCONF.analog[0].minl) )
    {
        if(ADCAlarmDelayCount[0] > 0)
        {
            ADCAlarmDelayCount[0]--;
            if(ADCAlarmDelayCount[0] == 0)
            {
                IOPORT.analog[0].alarm = 0x00;
                /* Seguidor AN -> OUT2 */
                if(IOPORTCONF.mascara_seguidor_an & 0b00000001)
                {
                    IOPORT.io[3].status &= 0b11111110;
                }
            }
        }
    }
    else ADCAlarmDelayCount[0] = ADC_ALARM_DELAY / 2;
    
    /* AN1 */
    if( ((IOPORT.analog[1].statush*256)+IOPORT.analog[1].statusl) >
        ((IOPORTCONF.analog[1].maxh*256)+IOPORTCONF.analog[1].maxl) )
    {
        if(ADCAlarmDelayCount[1] < ADC_ALARM_DELAY)
        {
            ADCAlarmDelayCount[1]++;
            if(ADCAlarmDelayCount[1] == ADC_ALARM_DELAY)
            {
                IOPORT.analog[1].alarm = 0xff;
                /* Seguidor AN -> OUT2 */
                if(IOPORTCONF.mascara_seguidor_an & 0b00000010)
                {
                    IOPORT.io[3].status |= 0b00000010;
                }
            }
        }
    }
    else if( ((IOPORT.analog[1].statush*256)+IOPORT.analog[1].statusl) <
             ((IOPORTCONF.analog[1].minh*256)+IOPORTCONF.analog[1].minl) )
    {
        if(ADCAlarmDelayCount[1] > 0)
        {
            ADCAlarmDelayCount[1]--;
            if(ADCAlarmDelayCount[1] == 0)
            {
                IOPORT.analog[1].alarm = 0x00;
                /* Seguidor AN -> OUT2 */
                if(IOPORTCONF.mascara_seguidor_an & 0b00000010)
                {
                    IOPORT.io[3].status &= 0b11111101;
                }
            }
        }
    }
    else ADCAlarmDelayCount[1] = ADC_ALARM_DELAY / 2;
    
    /* AN2 */
    if( ((IOPORT.analog[2].statush*256)+IOPORT.analog[2].statusl) >
        ((IOPORTCONF.analog[2].maxh*256)+IOPORTCONF.analog[2].maxl) )
    {
        if(ADCAlarmDelayCount[2] < ADC_ALARM_DELAY)
        {
            ADCAlarmDelayCount[2]++;
            if(ADCAlarmDelayCount[2] == ADC_ALARM_DELAY)
            {
                IOPORT.analog[2].alarm = 0xff;
                /* Seguidor AN -> OUT2 */
                if(IOPORTCONF.mascara_seguidor_an & 0b00000100)
                {
                    IOPORT.io[3].status |= 0b00000100;
                }
            }
        }
    }
    else if( ((IOPORT.analog[2].statush*256)+IOPORT.analog[2].statusl) <
             ((IOPORTCONF.analog[2].minh*256)+IOPORTCONF.analog[2].minl) )
    {
        if(ADCAlarmDelayCount[2] > 0)
        {
            ADCAlarmDelayCount[2]--;
            if(ADCAlarmDelayCount[2] == 0)
            {
                IOPORT.analog[2].alarm = 0x00;
                /* Seguidor AN -> OUT2 */
                if(IOPORTCONF.mascara_seguidor_an & 0b00000100)
                {
                    IOPORT.io[3].status &= 0b11111011;
                }
            }
        }
    }
    else ADCAlarmDelayCount[2] = ADC_ALARM_DELAY / 2;
    
    /* AN3 */
    if( ((IOPORT.analog[3].statush*256)+IOPORT.analog[3].statusl) >
        ((IOPORTCONF.analog[3].maxh*256)+IOPORTCONF.analog[3].maxl) )
    {
        if(ADCAlarmDelayCount[3] < ADC_ALARM_DELAY)
        {
            ADCAlarmDelayCount[3]++;
            if(ADCAlarmDelayCount[3] == ADC_ALARM_DELAY)
            {
                IOPORT.analog[3].alarm = 0xff;
                /* Seguidor AN -> OUT2 */
                if(IOPORTCONF.mascara_seguidor_an & 0b00001000)
                {
                    IOPORT.io[3].status |= 0b00001000;
                }
            }
        }
    }
    else if( ((IOPORT.analog[3].statush*256)+IOPORT.analog[3].statusl) <
             ((IOPORTCONF.analog[3].minh*256)+IOPORTCONF.analog[3].minl) )
    {
        if(ADCAlarmDelayCount[3] > 0)
        {
            ADCAlarmDelayCount[3]--;
            if(ADCAlarmDelayCount[3] == 0)
            {
                IOPORT.analog[3].alarm = 0x00;
                /* Seguidor AN -> OUT2 */
                if(IOPORTCONF.mascara_seguidor_an & 0b00001000)
                {
                    IOPORT.io[3].status &= 0b11110111;
                }
            }
        }
    }
    else ADCAlarmDelayCount[3] = ADC_ALARM_DELAY / 2;
    
}

unsigned char ADCAlarm( const char channel )
{
    unsigned char alarm = 0;
    
    switch(channel)
    {
        case '1':
            if((IOPORT.analog[0].alarm != IOPORT.analog[0].last_alarm) && (IOPORTCONF.mascara_notif_an & 0b00000001))
            {
                alarm = 1;
            }
            break;
        case '2':
            if((IOPORT.analog[1].alarm != IOPORT.analog[1].last_alarm) && (IOPORTCONF.mascara_notif_an & 0b00000010))
            {
                alarm = 1;
            }
            break;
        case '3':
            if((IOPORT.analog[2].alarm != IOPORT.analog[2].last_alarm) && (IOPORTCONF.mascara_notif_an & 0b00000100))
            {
                alarm = 1;
            }
            break;
        case '4':
            if((IOPORT.analog[3].alarm != IOPORT.analog[3].last_alarm) && (IOPORTCONF.mascara_notif_an & 0b00001000))
            {
                alarm = 1;
            }
            break;
    }
    
    return alarm;
}

void ADCAlarmReset( const char channel,  unsigned char reset)
{
    switch(channel)
    {
        case '1':
            IOPORT.analog[0].last_alarm = reset;
            break;
        case '2':
            IOPORT.analog[1].last_alarm = reset;
            break;
        case '3':
            IOPORT.analog[2].last_alarm = reset;
            break;
        case '4':
            IOPORT.analog[3].last_alarm = reset;
            break;
    }
}

#endif /* INCLUDE_ADC */