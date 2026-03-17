#include <vdp.h>
#include <system.h>

extern char mcolor;
extern char scolor;
extern char fcolor;

#define SCREEN_COLOR mcolor
#define ERROR_COLOR  fcolor
#define SUCCESS_COLOR scolor

extern const unsigned int byte2hex[256];   // correct signature
extern void writestring(int row, int col, char *pStr);

void __attribute__ ((noinline))
writehex(unsigned int row, unsigned int col, const unsigned int value) {
  unsigned char buf[3] = { 0, 0, 0 };
  *((unsigned int*)buf) = byte2hex[value >> 8];
  writestring(row, col, buf);
  *((unsigned int*)buf) = byte2hex[0xFF & value];
  writestring(row, col + 2, buf);
}

void __attribute__ ((noinline))
printSummary(int ec) {
  if (ec == 0) {
    VDP_SET_REGISTER(VDP_REG_COL, SUCCESS_COLOR);
    writestring(21, 11, "All Memory Passed");
  } else {
    VDP_SET_REGISTER(VDP_REG_COL, ERROR_COLOR);
    writestring(21, 14, "Found errors");
  }
}

int __attribute__ ((noinline))
hasRam(void) {
  volatile int* lower_exp = (volatile int*) 0x2000;
  *lower_exp = 0;
  *lower_exp = 0x1234;
  return (*lower_exp == 0x1234);
}