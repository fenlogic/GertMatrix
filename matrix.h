//
// Matrix header file
//


#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>

#define MAX_SET 128




int do_set(char line[1024]);
int match_set(char *line,int *value);

typedef struct {
  char name[33];
  int value;
} set_type;

/////////////////////////////////////
// showing pattern


#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO   */


#define PAG_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

#define TRUE  1
#define FALSE 0

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET0   *(gpio+7)  // Set GPIO high bits 0-31
#define GPIO_SET1   *(gpio+8)  // Set GPIO high bits 32-53

#define GPIO_CLR0   *(gpio+10) // Set GPIO low bits 0-31
#define GPIO_CLR1   *(gpio+11) // Set GPIO low bits 32-53

#define GPIO_IN0    *(gpio+13) // Read GPIO status bits 0-31
#define GPIO_IN1    *(gpio+14) // Read GPIO status bits 32-53

#define GPIO_PULL   *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock


#define COL_MASK  0x00000F80 // bits 7-11
#define ROW0      (1<<18)
#define ROW1      (1<<22)
#define ROW2      (1<<23)
#define ROW3      (1<<24)
#define ROW4      (1<<25)


#endif // MATRIX_H
