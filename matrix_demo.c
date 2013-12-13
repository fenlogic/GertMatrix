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
//
// wait some time or 
// if key is pressed
// if time==1M wait only until key is pressed
int demo_key()
{ char key;
  do {
    key=fgetc(stdin);
    if (key==' ')
      return 1;
    if (demo_time!=1000000)
      demo_time--;
  } while (demo_time>0);
  return 0;
} // demo key


void show_pattern_demo(int p)
{ char key;
  // clear column
  GPIO_CLR0 = COL_MASK;
  // Set first column bit pattern, have to shift bits 0-4 to position 7-11
  GPIO_SET0 = (p<<7) & COL_MASK; // 5 bits
  // Active first row by pulling bit LOW
  GPIO_CLR0 = ROW0;
  if (demo_key()==1)
    return;
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
  int p,key;
  demo_time = 1000000; // very slow
  p =0;
  do {
    show_pattern(0x01F79C61);
    p++;
    if (p==5)
      p = 0;
    key=fgetc(stdin);
    switch (key)
    {
    case '0' : demo_time = 100; break;
    case '1' : demo_time = 200; break;
    case '2' : demo_time = 400; break;
    case '3' : demo_time = 800; break;
    case '4' : demo_time = 1000; break;
    case '5' : demo_time = 3000; break;
    case '6' : demo_time = 6000; break;
    case '7' : demo_time = 10000; break;
    case '8' : demo_time = 20000; break;
    case '9' : demo_time = 1000000; break;
    }
  } while (key!='q');
} // demo mode 

