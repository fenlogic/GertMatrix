//
//
//  Matrix demo mode
//
//

#include "matrix.h"

// on_time has different delay here because 
// we use a totally different delay method
extern volatile unsigned *gpio;

int demo_time;
int key; // remember last key pressed
//
// wait some time or until key is pressed
// if time==1M wait only until key is pressed
int demo_key()
{ int d;
  d = demo_time;
  do {
    key=fgetc(stdin);
    if (key==' ')
      return 0;
    if (key!=-1)
      return 1;
    if (demo_time!=1000000)
      d--;
  } while (d>0);
  return 0;
} // demo key


int show_pattern_demo(int p)
{ 
  // clear column
  GPIO_CLR0 = COL_MASK;
  // Set first column bit pattern, have to shift bits 0-4 to position 7-11
  GPIO_SET0 = (p<<7) & COL_MASK; // 5 bits
  // Active first row by pulling bit LOW
  GPIO_CLR0 = ROW0;
  if (demo_key()==1)
    return 1;
  // de-activate
  GPIO_SET0 = ROW0;

  // clear column
  GPIO_CLR0 = COL_MASK;
  // Set second column bit pattern, have to shift bits 5-9 to position 7-11
  GPIO_SET0 = (p << 2) & COL_MASK;
  // Active second row
  GPIO_CLR0 = ROW1;
  if (demo_key()==1)
    return;
  // de-activate
  GPIO_SET0 = ROW1;

  // clear column
  GPIO_CLR0 = COL_MASK;
  // Set third column bit pattern, have to shift bits 10-14 to position 7-11
  GPIO_SET0 = (p>>3) & COL_MASK;
  // Active third row
  GPIO_CLR0 = ROW2;
  if (demo_key()==1)
    return;
  // de-activate
  GPIO_SET0 = ROW2;

  // clear column
  GPIO_CLR0 = COL_MASK;
  // Set forth column bit pattern, have to shift bits 15-19 to position 7-11
  GPIO_SET0 = (p>>8) & COL_MASK;
  // Active forth row
  GPIO_CLR0 = ROW3;
  if (demo_key()==1)
    return;
  // de-activate
  GPIO_SET0 = ROW3;

  // clear column
  GPIO_CLR0 = COL_MASK;
  // Set fifth column bit pattern, have to shift bits 20-24 to position 7-11
  GPIO_SET0 = (p>>13) & COL_MASK;
  // Active last row
  GPIO_CLR0 = ROW4;
  if (demo_key()==1)
    return;
  // de-activate
  GPIO_SET0 = ROW4;
} // show_pattern_demo

void demo_mode()
{ 
  int p;
  printf("\n\nRunning in Demo mode.\n");
  printf("Showing diagonal pattern. Press <space> to step through\n");
  printf("Then try increasingly run speed using 1,2,3,4,...8\n");
  printf("'d' returns to stepping, 'q' to quit demo mode\n");
  demo_time = 1000000; // manual
  p =0;
  key=fgetc(stdin);
  do {
    show_pattern_demo(0x01F79C61);
    show_pattern(0); // clean up after key press
    p++;
    if (p==5)
      p = 0;
    switch (key) // use 'global' key
    {
    case '8' : demo_time = 1000; break;
    case '7' : demo_time = 2000; break;
    case '6' : demo_time = 4000; break;
    case '5' : demo_time = 8000; break;
    case '4' : demo_time = 10000; break;
    case '3' : demo_time = 20000; break;
    case '2' : demo_time = 30000; break;
    case '1' : demo_time = 100000; break;
    case 'd' : demo_time = 1000000; break;
    }
  } while (key!='q' && key!='Q');
  printf("\nExiting demo mode\n\n");
} // demo mode 

