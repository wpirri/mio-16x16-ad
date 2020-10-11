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

#ifndef PERIF_H
#define	PERIF_H

#include "config.h"

typedef struct _FECHA_HORA
{
    struct
    {
        unsigned char anio;
        unsigned char mes;
        unsigned char dia;
        unsigned char fecha;
    } fecha;
    struct
    {
        unsigned char horas;
        unsigned char minutos;
        unsigned char segundos;
    } hora;
    
} FECHA_HORA;

#ifdef _IN_PERIF_C_
FECHA_HORA gFechaHora;
#else
extern FECHA_HORA gFechaHora;
#endif;

void PerifInit( void );
void PerifTask( void );
void PerifTimer10ms( void );
void PerifTimer1s( void );

void Beep(unsigned char count);

#endif	/* PERIF_H */

