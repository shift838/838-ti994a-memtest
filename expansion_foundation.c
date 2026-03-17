#include <vdp.h>
#include <system.h>

extern int try_limit;
extern int corcompCru;

extern int testBlock(const unsigned int row, unsigned char* addr);
extern char* int2str(int value);

#define CRU_FOUNDATION 0x1E00
#define CRU_MYARC      0x1000

void __attribute__ ((noinline)) foundationBank(int page, int crubase) {
  int adjusted = page << 8;

  __asm__(
    "MOV %1, r12\n\t"
    "LDCR %0,4\n\t"
    : : "r"(adjusted), "r"(crubase+2) : "r12"
  );
}

int __attribute__ ((noinline)) hasFoundation(int crubase) {
  volatile int* lower_exp = (volatile int*) 0x2000;
  foundationBank(0, crubase);
  *lower_exp = 0x1234;
  foundationBank(1, crubase);
  *lower_exp = 0;
  foundationBank(0, crubase);
  return (*lower_exp == 0x1234);
}

int __attribute__ ((noinline)) foundationPagecount(int crubase) {
  volatile int* lower_exp = (volatile int*) 0x2000;
  for(int i = 0; i < 16; i++) {
    foundationBank(i, crubase);
    *lower_exp = 0x1234;
  }
  foundationBank(0, crubase);
  int pages = 0;
  while(pages < 16 && *lower_exp != 0xFFFF) {
    *lower_exp = 0xFFFF;
    foundationBank(++pages, crubase);
  }
  foundationBank(0, crubase);
  return pages;
}

int __attribute__ ((noinline)) testFoundation(int pagecount, int crubase) {
  writestring(2, 20, int2str(32*pagecount));
  writestring(2, 23, "K");

  int ec = 0;
  for (int j = 0; j < pagecount && ec == 0; j++) {
    foundationBank(j, crubase);
    writestring(4, 16, "page ");
    writestring(4,21, int2str(j));

    ec += testBlock(6, (unsigned char*)0x2000);
    ec += testBlock(7, (unsigned char*)0xA000);
    ec += testBlock(8, (unsigned char*)0xC000);
    ec += testBlock(9, (unsigned char*)0xE000);
  }
  return ec;
}