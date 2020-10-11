/***************************************************************************
    Copyright (C) 2018   Walter Pirri

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
#ifndef _MENU_H_
#define	_MENU_H_

#include <xc.h> // include processor files - each processor file is guarded.  

#include "lcd.h"

char main_menu[][10][LCD_COLS] = {
    {
        {"Menu 1A"},
        {"Menu 1B"},
        {"Menu 1C"},
        {"Menu 1D"},
        {"Menu 1E"},
        {"Menu 1F"},
        {"Menu 1G"},
        {0},
        {0},
        {0}
    },
    {
        {"Menu 2A"},
        {"Menu 2B"},
        {"Menu 2C"},
        {"Menu 2D"},
        {"Menu 2E"},
        {"Menu 2F"},
        {"Menu 2G"},
        {0},
        {0},
        {0}
    },
    {
        {"Menu 3A"},
        {"Menu 3B"},
        {"Menu 3C"},
        {"Menu 3D"},
        {"Menu 3E"},
        {"Menu 3F"},
        {"Menu 3G"},
        {0},
        {0},
        {0}
    },
    {
        {"Menu 4A"},
        {"Menu 4B"},
        {"Menu 4C"},
        {"Menu 4D"},
        {"Menu 4E"},
        {"Menu 4F"},
        {"Menu 4G"},
        {0},
        {0},
        {0}
    },
    {
        {"Menu 5A"},
        {"Menu 5B"},
        {"Menu 5C"},
        {"Menu 5D"},
        {"Menu 5E"},
        {"Menu 5F"},
        {"Menu 5G"},
        {0},
        {0},
        {0}
    },
    {
        {"Menu 5A"},
        {"Menu 5B"},
        {"Menu 5C"},
        {"Menu 5D"},
        {"Menu 5E"},
        {"Menu 5F"},
        {"Menu 5G"},
        {0},
        {0},
        {0}
    }
};

void MenuInit( void );
void MenuTimer( void );
void MenuTask( void );

#endif	/* _MENU_H_ */

