/*
 * Copyright (c) 2025 Jeff Gough
 *
 * This software is released under the MIT License.
 * See the LICENSE file for full license text.
 */

#ifndef LCD_H
#define LCD_H

//LCD initialisation code
void lcd_init();
//clear LCD
void lcd_clear();
//display a value on LCD
void lcd_dispvalue(unsigned int value);

#endif
