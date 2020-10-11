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

#ifndef ADCSENS_H
#define	ADCSENS_H

#include "config.h"

#ifdef INCLUDE_ADC

void ADCInit( void );
void ADCTask( void );
void ADCTimer( void );
unsigned char ADCAlarm( const char channel );
void ADCAlarmReset( const char channel,  unsigned char reset);

#endif /* INCLUDE_ADC */

#endif	/* ADCSENS_H */

