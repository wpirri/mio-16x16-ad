/* ************************************************************************** *
 *
 *  Filename:           bitmacro.h
 *  Date:               11 de Noviembre de 2017
 *  File Version:       1.0
 *  Assembled using:    XC8
 *
 *  Author:             Walter Pirri
 *                      (walter***AT***pirri***DOT***com***DOT***ar)
 *  Company:            WGP
 *
 *************************************************************************** */
#ifndef _BITMACRO_H_
#define	_BITMACRO_H_

#define SetBit(p, b) ( p |= (0b00000001 << b) )
#define ResetBit(p, b) ( p &= ((0b00000001 << b)^0xff) )
#define InvertBit(p, b) ( p ^= (0b00000001 << b) )
#define IsBit(p, b) ( p & (0b00000001 << b) )

// Microcip
#define SetInput(p, b) ( p |= (0b00000001 << b) )
#define SetOutput(p, b) ( p &= ((0b00000001 << b)^0xff) )
#define SetAnalog(p, b) ( p |= (0b00000001 << b) )
#define SetDigital(p, b) ( p &= ((0b00000001 << b)^0xff) )

//// AVR
//#define SetOutput(p, b) ( p |= (0b00000001 << b) )
//#define SetInput(p, b) ( p &= ((0b00000001 << b)^0xff) )
////#define SetAnalog(p, b) ( p |= (0b00000001 << b) )
////#define SetDigital(p, b) ( p &= ((0b00000001 << b)^0xff) )


#endif	/* _BITMACRO_H_ */

