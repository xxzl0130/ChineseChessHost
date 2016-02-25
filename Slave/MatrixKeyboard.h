/* MatrixKeyboard.h : A simple way to manage a matrix keyboard up to 16*16 */

/* (C) 2016 by zaxs <zaxs0130@gmail.com>
*
* All Rights Reserved
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
*/
#ifndef __MATRIX_KEYBOARD_H__
#define __MATRIX_KEYBOARD_H__

/* MatrixKeyboard : A simple class to connect up to 256 buttons
Example:(3*3)
row/col   0	  1   2         Key numbers (returned by getKeyPresing)
          |   |   |
  0   ----X---X---X         0x00  0x01  0x02
          |   |   |
  1   ----X---X---X         0x10  0x11  0x12
          |   |   |
  2   ----X---X---X         0x20  0x21  0x22

  The first 4bits of key numbers is the number of row, and the second is column.
  eg:
  0x21:  0010 0001
          row  col
*/

#include <arduino.h>

class MatrixKeyboard
{
private:
	// The start Pin of rows
	unsigned char rowS;
	// The end Pin of rows
	unsigned char rowT;
	// The start Pin of columns
	unsigned char colS;
	// The end Pin of columns
	unsigned char colT;
public:
	MatrixKeyboard(unsigned char, unsigned char, unsigned char, unsigned char);

	/*
	Get all the keys beging pressed.
	Parameter buf is an arrary whose size should be at least
	the size of keyboard (rows * columns).The keys' information
	will be saved in this arrary.
	Parameter count will store the number of keys beging pressed.
	*/
	void getKeyPresing(unsigned char* buf,unsigned char* count) const;
	/*
	Test whether the key is bing pressed.
	The Parameter key uses 4 bits to present row and column respectively.
	*/
	bool isPressing(unsigned char key) const;
	/*
	Test whether the key is bing pressed which is confirmed by row&col.
	*/
	bool isPressing(unsigned char row, unsigned char col) const;
};

// Test whether the state of pin is HIGH.
bool isHigh(unsigned char pin);

#endif // __MATRIX_KEYBOARD_H__