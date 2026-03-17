extern char* int2str(int value);
#include <vdp.h>
#include <system.h>

extern int try_limit;

void writehex(unsigned int row, unsigned int col, const unsigned int value);

int testBlock(const unsigned int row, unsigned char* addr);

int testBlock(const unsigned int row, unsigned char* addr)
{
  int ec = 0;
  unsigned int* end=(unsigned int*)(addr + 0x1FFF);

  writestring(row, 3, "Testing");
  writehex(row, 11, (int)addr);
  writestring(row, 16, "->");
  writehex(row, 19, (int)end);
  writestring(row, 24, ":");
  writestring(row, 26, "      ");

  const char spinner[4] = { '-', '\\', '|', '/' };
  int spinneridx = 0;

  for( int tries=0; tries<try_limit; tries++) {

    vdpchar(VDP_SCREEN_TEXT(row, 26), spinner[spinneridx++ % 4]);
    unsigned int val = 0x0000;

    for( volatile unsigned int* a=(unsigned int*)addr; a<=end; a++ ) {
      *a = val;
      val++;
    }

    val = 0x0000;
    for (volatile unsigned int *a = (unsigned int *)addr; a <= end; a++) {
      if (*a != val) ec++;
      val++;
    }

    if (try_limit > 1) {
      vdpchar(VDP_SCREEN_TEXT(row, 26), spinner[spinneridx++ % 4]);
      val = 0x0000;

      for( volatile unsigned int* a=(unsigned int*)addr; a<=end; a++ ) {
        *a = ~val;
        val++;
      }

      val = 0x0000;
      for( volatile unsigned int* a=(unsigned int*)addr; a<=end; a++ ) {
        if (*a != ~val) ec++;
        val++;
      }

      vdpchar(VDP_SCREEN_TEXT(row, 26), spinner[spinneridx++ % 4]);
      val = 0x8000;

      for( volatile unsigned int* a=(unsigned int*)addr; a<=end; a++ ) {
        for( unsigned char i = 0; i<<8; i++) {
          *a = val;
          if (*a != val) ec++;
          val = val << i;
        }
      }

      vdpchar(VDP_SCREEN_TEXT(row, 26), spinner[spinneridx++ % 4]);
      val = 0xA5A5;
      for( volatile unsigned int* a=(unsigned int*)addr; a<=end; a++ ) *a = val;

      val = 0xA5A5;
      for( volatile unsigned int* a=(unsigned int*)addr; a<=end; a++ ) {
        if (*a != val) ec++;
      }

      vdpchar(VDP_SCREEN_TEXT(row, 26), spinner[spinneridx++ % 4]);
      val = ~0xA5A5;
      for( volatile unsigned int* a=(unsigned int*)addr; a<=end; a++ ) *a = val;

      val = ~0xA5A5;
      for( volatile unsigned int* a=(unsigned int*)addr; a<=end; a++ ) {
        if (*a != val) ec++;
      }
    }
  }

  if (ec == 0) writestring(row, 26, "PASSED");
  else writestring(row, 26, "FAILED");

  return ec;
}

int test32k()
{
  int ec = 0;
  ec += testBlock(6, (unsigned char*)0x2000);
  ec += testBlock(7, (unsigned char*)0xA000);
  ec += testBlock(8, (unsigned char*)0xC000);
  ec += testBlock(9, (unsigned char*)0xE000);
  return ec;
}