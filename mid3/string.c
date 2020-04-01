/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
#pragma once

extern int kgets();
extern int kgetc();
int kprintf(char *, ...);
int strlen(char *s)

{
  int i = 0;
  while(*s){
    i++; s++;
  }
  return i;
}

int strcmp(char *s1, char *s2)
{
  while((*s1 && *s2) && (*s1==*s2)){
      s1++; s2++;
  }
  if ((*s1==0) && (*s2==0))
    return 0;
  return(*s1 - *s2);

}

int strcpy(char *dest, char *src)
{
  while(*src){
    *dest++ = *src++;
  }
  *dest = 0;
}

int atoi(char *s)
{
  int v = 0;
  while (*s){
    v = 10*v + (*s - '0');
    s++;
  }
  //kprintf("v=%d\n", v);
  return v;
}
int geti()
{
  char s[16];
  kgets(s);
  return atoi(s);
}
char *bprints(char *dest, char *s)
{
  while(*s){
    *dest++ = *s;
    s++;
  }
  return dest;
}

char *brpx(char *dest, int x)
{
  char c;
  if (x){
     c = tab[x % 16];
     dest = brpx(dest, x / 16);
  }
  *dest++ = c;
  return dest;
}

char *bprintx(char *dest, int x)
{
  *dest++ = '0';
  *dest++ = 'x';
  if (x==0)
    *dest++ = '0';
  else
    dest = brpx(dest, x);
  return dest; 
}

char *brpu(char *dest, int x)
{
  char c;
  if (x){
     c = tab[x % 10];
     dest = brpu(dest, x / 10);
     *dest++ = c;
  } 
  return dest;
}

char *bprintu(char *dest, int x)
{
  if (x==0){
    *dest++ = '0';
  }
  else
    dest = brpu(dest, x);
  return dest;
}

char *bprinti(char *dest, int x)
{
  if (x<0){
    *dest++ = '-';
    x = -x;
  }
  return bprintu(dest, x);
}

// prints to a buffer. returns pointer to null terminating char.
char *bprintf(char *dest, char *fmt,...)
{
  int *ip;  // pointer to current fmt arg
  char *cp; // pointer to current byte of format string
  char *dp = dest; // pointer to last byte of destination buffer
  cp = fmt;
  ip = (int *)&fmt + 1;

  while(*cp){
    if (*cp != '%'){
      *dp = *cp;
      cp++;
      dp++;
      continue;
    }
    cp++;
    switch(*cp){
    case 'c': *dp++ = *ip;      break;
    case 's': dp = bprints(dp, (char *)*ip);  break;
    case 'd': dp = bprinti(dp, *ip);          break;
    case 'u': dp = bprintu(dp, *ip);          break;
    case 'x': dp = bprintx(dp, *ip);          break;
    }
    cp++; ip++;
  }

  return dp;
}

