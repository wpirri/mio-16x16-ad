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
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <xc.h>

#include "menu.h"

unsigned char menu_menulevel;
unsigned char menu_menuitem;
unsigned char menu_task;

void MenuInit( void )
{
    menu_menulevel = 0;
    menu_menuitem = 0;
    menu_task = 0;
}

void MenuTimer( void )
{
    
}

void MenuTask( void )
{
    unsigned char i;
    
    switch(menu_task)
    {
        case 0:     /* Reposo */
            
            
            menu_task++;
            break;
        case 1:     /* Mostrar Menu */
            LCDClear();
            for(i = 0; i < LCD_LINES; i++)
            {
                if(menu_menuitem == i)
                    LCDMsg(i, 1, ">");
                LCDMsg(i, 2, main_menu[menu_menulevel][i]);
            }
            menu_task++;
            break;
        case 2:     /* Navegar Menu */
            
            break;
        case 3:     /* Ejecutar menu */
            
            break;
        default:
            menu_task = 0;
            break;
            
    }
}
