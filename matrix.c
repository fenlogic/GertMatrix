/*
 * Gertboard 5x5 LED matrix demo
 * Written in C for speed
 *
 */
//////     reading pattern

// matrix_pattern.cpp : Defines the entry point for the console application.
// seperated

#include "matrix.h"


FILE *infile;

set_type set[MAX_SET];
int set_count;
int line_number;
char *this_program;
int pattern_count;

int mem_fd;
char *gpio_mem, *gpio_map;
volatile unsigned *gpio;


// These are specific for the matrix program


/*
   Use GPIO 7,8,9,10,11 as LED row driver
   as these are all adjacent so easy to write
   Use GPIO 22,23,24,25,17 as ROWumn sink
   we have 25 bits as pattern which fits nice in a 32-bit integer

   row 0-4 are bits 0-4,
 */

int on_time = 1000;
int rep = 10;
char *this_program;

int *pattern_store;

void usage()
{
  fprintf(stderr,"Usage %s <inputfile>\n",this_program);
  fprintf(stderr,"%s is a program to show patterns on a 5x5 matrix LED matrix\n",this_program);
  fprintf(stderr,"The input file contains a text representation of the patterns\n");
  fprintf(stderr,"It should contain 5x5 patterns seperated by empty lines\n");
  fprintf(stderr,"The following indicate a LED which is on: x,X,#\n");
  fprintf(stderr,"The following indicate a LED which is off: .,<space>,<comma>\n");
  fprintf(stderr,"Lines starting with / are ignored\n");
  fprintf(stderr,"The fifth line can be followed by a number (1..128) which specifies\n");
  fprintf(stderr,"how long the pattern should be displayed\n");
  fprintf(stderr,"Alternative you can give the delay time a name and use that\n");
  fprintf(stderr,"Use 'set <name> = <dec value>' to set a delay name\n");
fprintf(stderr,"//                        \n");
fprintf(stderr,"//  Example pattern file  \n");
fprintf(stderr,"//        \n");
fprintf(stderr,"set TA = 5\n");
fprintf(stderr,"set TB = 1\n");
fprintf(stderr,".....\n");
fprintf(stderr,".....\n");
fprintf(stderr,"..x..\n");
fprintf(stderr,".....\n");
fprintf(stderr,".....TA\n");
fprintf(stderr,"\n");
fprintf(stderr,".....\n");
fprintf(stderr,".xxx.\n");
fprintf(stderr,".xxx.\n");
fprintf(stderr,".xxx.\n");
fprintf(stderr,".....TA\n");
fprintf(stderr,"\n");
fprintf(stderr,"xxxxx\n");
fprintf(stderr,"xxxxx\n");
fprintf(stderr,"xxxxx\n");
fprintf(stderr,"xxxxx\n");
fprintf(stderr,"xxxxxTB\n");
fprintf(stderr,"\n");
fprintf(stderr,"xxxxx\n");
fprintf(stderr,"xxxxx\n");
fprintf(stderr,"xx.xx\n");
fprintf(stderr,"xxxxx\n");
fprintf(stderr,"xxxxx2\n");
  exit(1);
}

// Need 10 outputs
void setup_gpio()
{
   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("Can't open /dev/mem\n");
      printf("Did you forget to use 'sudo .. ?'\n");
      exit (-1);
   }

   /*
    * mmap GPIO
    */
   if ((gpio_mem = malloc(BLOCK_SIZE + (PAG_SIZE-1))) == NULL) {
      printf("allocation error \n");
      exit (-1);
   }
   if ((unsigned long)gpio_mem % PAG_SIZE)
     gpio_mem += PAG_SIZE - ((unsigned long)gpio_mem % PAG_SIZE);

   gpio_map = (unsigned char *)mmap(
      (caddr_t)gpio_mem,
      BLOCK_SIZE,
      PROT_READ|PROT_WRITE,
      MAP_SHARED|MAP_FIXED,
      mem_fd,
      GPIO_BASE
   );

   if ((long)gpio_map < 0) {
      printf("gpio mmap error %d\n", (int)gpio_map);
      exit (-1);
   }
   gpio = (volatile unsigned *)gpio_map;


   INP_GPIO(7);   OUT_GPIO(7);
   INP_GPIO(8);   OUT_GPIO(8);
   INP_GPIO(9);   OUT_GPIO(9);
   INP_GPIO(10);  OUT_GPIO(10);
   INP_GPIO(11);  OUT_GPIO(11);
   INP_GPIO(22);  OUT_GPIO(22);
   INP_GPIO(23);  OUT_GPIO(23);
   INP_GPIO(24);  OUT_GPIO(24);
   INP_GPIO(25);  OUT_GPIO(25);
   INP_GPIO(18);  OUT_GPIO(18);
}

void show_pattern(int p)
{
  // clear column
  GPIO_CLR0 = COL_MASK;
  // Set first column bit pattern, have to shift bits 0-4 to position 7-11
  GPIO_SET0 = (p<<7) & COL_MASK; // 5 bits
  // Active first row by pulling bit LOW
  GPIO_CLR0 = ROW0;
  usleep(on_time); // sleep
  // de-activate
  GPIO_SET0 = ROW0;

  // clear column
  GPIO_CLR0 = COL_MASK;
  // Set second column bit pattern, have to shift bits 5-9 to position 7-11
  GPIO_SET0 = (p << 2) & COL_MASK;
  // Active second row
  GPIO_CLR0 = ROW1;
  usleep(on_time); // sleep
  // de-activate
  GPIO_SET0 = ROW1;

  // clear column
  GPIO_CLR0 = COL_MASK;
  // Set third column bit pattern, have to shift bits 10-14 to position 7-11
  GPIO_SET0 = (p>>3) & COL_MASK;
  // Active third row
  GPIO_CLR0 = ROW2;
  usleep(on_time); // sleep
  // de-activate
  GPIO_SET0 = ROW2;

  // clear column
  GPIO_CLR0 = COL_MASK;
  // Set forth column bit pattern, have to shift bits 15-19 to position 7-11
  GPIO_SET0 = (p>>8) & COL_MASK;
  // Active forth row
  GPIO_CLR0 = ROW3;
  usleep(on_time); // sleep
  // de-activate
  GPIO_SET0 = ROW3;

  // clear column
  GPIO_CLR0 = COL_MASK;
  // Set fifth column bit pattern, have to shift bits 20-24 to position 7-11
  GPIO_SET0 = (p>>13) & COL_MASK;
  // Active last row
  GPIO_CLR0 = ROW4;
  usleep(on_time); // sleep
  // de-activate
  GPIO_SET0 = ROW4;
} // show_pattern

//
// Change keyboard behaviour
//
void immediate_key_action()
{ int flags;
   // switch I/O to non-blocking
   flags = fcntl(STDIN_FILENO, F_GETFL);
   fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

   // Switch TTY input to non buffered
   system("stty -icanon");
}
//
// The main program 
//
int main(int argc,char *argv[])
{ int key,p,r;
  int status;
  
  this_program = argv[0];
  if (argc==1)
    usage();
  
  // first time read just to find out number of patterns
  status = read_pattern(1,argv[1]); 
  if (status==0)
    return 1;
  pattern_store = (int *)malloc(pattern_count*sizeof(int));
  if (!pattern_store)
  { fprintf(stderr," Not enough memory to hold pattern\n");
    exit(1);
  }
  // second time actually read the patterns
  read_pattern(2,argv[1]);
   
  // Now display them
  setup_gpio();
  immediate_key_action();

  printf("Control the speed with the keys 1-8\n");
  printf("d=demo mode, q = quit\n");
  p=0;
  r = rep;
  do {
    show_pattern(pattern_store[p]);
    r--;
    if (r<=0)
    {
      p++;
      if (p>=pattern_count) 
        p = 0;
      // Use top 7 bits of pattern for 'speed'
      r = rep * (1+ ((pattern_store[p] >> 25) & 0x7F));
    }
    key=fgetc(stdin);
    switch (key)
    {
    case '8' : rep = 1; break;
    case '7' : rep = 2; break;
    case '6' : rep = 4; break;
    case '5' : rep = 6; break;
    case '4' : rep = 8; break;
    case '3' : rep = 10; break;
    case '2' : rep = 15; break;
    case '1' : rep = 20; break;
    case 'd' : demo_mode();
    }
  } while (key!='q' && key!='Q');
  
  // Before stopping switch all LEDS off
  show_pattern(0);

  system("stty sane");
} // main


/***************************************************************************\
 *                                                                         *
 *  This is the part where the text file gets converted to a binary format *
 *                                                                         *
\***************************************************************************/

int read_pattern(int pass, char *filename)
{ int line_number,error;
  int row,col;
  int extra,scan_status;
  int pattern;
  char line[1024]; // if that does not fit the text we are in trouble anyway 
 
  set_count=0;
  line_number=0;
  
  infile=fopen(filename,"r");
  if (!infile)
  {
    fprintf(stderr,"Error in %s: Could not open '%s' for reading\n",this_program,filename);
    return 0;
  }
  line_number = 0;
  row = 0;
  error = 0;
  pattern_count = 0;

  while (fgets(line,1024,infile) && !error)
  { line_number++;
    if (line[0]=='/')
      continue;
    if (!strncmp(line,"set",3))
    { error = do_set(line);
      continue;
    }
    
    if (row==0)
    { // start of new pattern
      pattern = 0;
    }

    if (row==5)
    { // must be empty line
      if (line[0]!=0x0a && line[0]!=0x0d)
      { error = 1;
        fprintf(stderr,"%s: error in line %d of %s, Expected empty line\n",
                  this_program,line_number,filename);
      }
      row = 0;
    }
    else
    { // process one row 
      for (col=0; col<5; col++)
      { // is character a 'bit-set' one?
        if (line[col]=='x' || line[col]=='X' || line[col]=='#' )
          pattern = pattern | (0x01 << (row*5+col));
        else
        { // must be bit clear. If not error
          if (line[col]!=' ' && line[col]!='.' && line[col]!=',' )
          { error = 1;
            fprintf(stderr,"%s: error in line %d of %s, Illegal pattern character\n",
                      this_program,line_number,filename);
          } // bit clear? 
        } // bit set 
      } // process 5 chars
      row++;
    } // process row 
    if (row==5)
    { // finish row 

      // Here I scan for a number or variable at the end of row 5 
      // That can be used as a 'time' for the pattern 
      if (isdigit(line[5]))
        scan_status = sscanf(line+5,"%d",&extra);
      else
        if (isalpha(line[5]))
        { error = match_set(line+5,&extra);
          scan_status = !error;
        }

      if (scan_status==1)
      { // Have 32-25=7 bits left
        if (extra<0 || extra>127)
        { error = 1;
          fprintf(stderr,"%s: error in line %d of %s, Illegal extra value\n",
                    this_program,line_number,filename);
        } 
        else
          pattern = pattern | (extra << 25);
      }

      // The first pass I don't store anything, just count
      // It is just to found out how many patterns there are
      if (pass==1)
         pattern_count++;
       else
         // store the pattern 
         pattern_store[pattern_count++]=pattern; 
    }
  }
  fclose(infile);
  return error ? 0 : 1;
} // read_pattern 

//
// Read 'set' from input file
// Syntax set <name>=<dec value>
// name must start with character followed by characters,
// numbers or _ and max 32 long. 
//
// return 0 on OK, 1 on error
//
int do_set(char line[1024])
{ int l;
  int n;
  char name[33];
  int  value;

  if (set_count==128)
  { fprintf(stderr,"%s, line %d: Error : max set count reached\n",
                 this_program,line_number);
    return 1;
  }

  l = 3;
  // skip spaces
  while (line[l]==' ')
    l++;

  // get name 
  n=0;
  while (line[l] && line[l]!='=' && line[l]!=' ' && n<32)
    name[n++]=line[l++];
  name[n]=0;

  // Has this name been used already ?
  for (n=0; n<set_count; n++)
    if (!strcmp(name,set[n].name))
    { fprintf(stderr,"%s, line %d: Error : Set name already used\n",
                   this_program,line_number);
      return 1;
    }
 
  // is the name legal?
  n = 0;
  while (name[n])
  {
    if ( (n==0 && !isalpha(name[n])) ||
         (n>0  && !isalnum(name[n]) && name[n]!='_') )
    { fprintf(stderr,"%s, line %d: Error : Illegal set name character\n",
                   this_program,line_number);
      return 1;
    }
    n++;
  }

  // skip spaces
  while (line[l]==' ')
    l++;

  if (line[l]!='=')
  { fprintf(stderr,"%s, line %d: Error set expecting '='\n",
                 this_program,line_number);
    return 1;
  }
  l++; // skip '='

  // skip spaces
  while (line[l]==' ')
    l++;

  // get number
  n = sscanf(line+l,"%d",&value);
  if (n!=1)
  { fprintf(stderr,"%s, line %d: Error set value syntax error\n",
                 this_program,line_number);
    return 1;
  }

  strcpy(set[set_count].name,name);
  set[set_count].value = value;
  set_count++;

  return 0;
} // do_set 

int match_set(char *line,int *value)
{ char name[33];
  int  l,n;
  // get name 
  l=0; n=0;
  while (line[l] && line[l]!=' ' && line[l]!=0x0a && line[l]!=0x0d && n<32)
    name[n++]=line[l++];
  name[n]=0;

  // Find name in set 
  for (n=0; n<set_count; n++)
    if (!strcmp(name,set[n].name))
    { // Have a match
      *value = set[n].value;
      return 0;
    }
  fprintf(stderr,"%s, line %d: Error name '%s' found\n",
               this_program,line_number,name);
  return 1;

} // match_set
