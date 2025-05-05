/*
 * Copyright (c) 2025 Jeff Gough
 *
 * This software is released under the MIT License.
 * See the LICENSE file for full license text.
 */

#ifndef __PRINT_H
#define __PRINT_H

typedef enum{PRINT_IDLE = 0, PRINT_PRINTING, PRINT_FINISHED} printstate_t;
typedef enum{JOB_NORMAL = 0, JOB_TEST} jobstate_t;

extern volatile printstate_t printstate;
extern volatile jobstate_t jobstate;

void print_cleancycle();

void print_init();
void  print_nextpixel();
void print_line();
#endif
