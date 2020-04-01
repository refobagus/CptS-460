/*******************************************************
*                      t.c file                        *
*******************************************************/
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"

typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;
GD    *gp;
INODE *ip;
DIR   *dp;

u16 NSEC = 2;
char buf1[BLK], buf2[BLK];

int prints(char *s)
{
// write YOUR code
while(*s){
    putc(*s++);
}
}

int gets(char *s)
{
// write YOUR code
while((*s=getc())!='\r'){
    putc(*s++);
}
*s=0;
}


u16 getblk(u16 blk, char *buf)
{
    readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
}

u16 search(INODE *ip, char *name)
{
  //search for name in the data block of INODE; 
  //return its inumber if found
  //else error();
  int i;
  DIR * dp;
  char *cp,rohis[1024];
   for(i = 0; i < 12; i++)
    {
      if (ip->i_block[i] == 0) break;
        getblk((u16)ip->i_block[i], rohis);
        cp = rohis;
        dp = (DIR*)rohis;
        while(cp < rohis + BLK)
        {
            if(strncmp(dp->name, name,dp->name_len) == 0)
                return (u16)dp->inode - 1;
            cp+=dp->rec_len;
            dp = (DIR*)cp;
        } 
}
  error();
}

main()
{ 
  //1. Write YOUR C code to get the INODE of /boot/mtx
  /* INODE *ip --> INODE

   if INODE has indirect blocks: get i_block[12] int buf2[  ]*/


//2. setes(0x1000);  // MTX loading segment = 0x1000

//3. load 12 DIRECT blocks of INODE into memory beginning at segment 0x1000

//4. load INDIRECT blocks, if any, into memory

//   prints("go?"); getc();
  u16    i, iblk;
  char   temp[64];
  u16    inode;
  u32    *blk2;

  getblk((u16)2, buf1);
  gp = (GD *)buf1;
  iblk = (u16)gp->bg_inode_table;
  getblk(iblk, buf1);
  ip = (INODE *)buf1 + 1;
  
  inode = search(ip, "boot");
  getblk(iblk + (inode / 8), buf1);
  ip = (INODE*)buf1 + (inode % 8);

  inode = search(ip, "mtx");
  getblk(iblk + (inode / 8), buf1);
  ip = (INODE*)buf1 + (inode % 8);
  
  getblk((u16)ip->i_block[12], buf2);
  blk2 = (u32*)buf2;

  setes(0x1000);

  for (i=0; i<12; i++){
    getblk((u16)ip->i_block[i], 0);
    inces();
  }

  while (*blk2 != 0)
  {
    getblk(*blk2++, 0);
    inces();
  }
  prints("go?"); getc();
}  
