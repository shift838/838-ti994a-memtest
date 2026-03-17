#include <vdp.h>
#include <system.h>

extern int corcompCru;
extern int testBlock(const unsigned int row, unsigned char* addr);
extern char* int2str(int value);

void __attribute__((noinline)) crubit(int cru, int onoff);

int __attribute__((noinline)) bank2cru(int page, int crubase) {
  unsigned char crus[8] = { 0x04, 0x0C, 0x14, 0x1C, 0x24, 0x2C, 0x34, 0x3C };
  return crubase + crus[page%8];
}

void __attribute__((noinline)) corcompBank(int page, int crubase) {
  int bank = page % 8;

  int bit = bank2cru(bank, crubase);

  crubit(crubase + 6, bank == page);

  int oldbank = bank == 0 ? 7 : bank - 1;
  int oldbit = bank2cru(oldbank, crubase);

  crubit(oldbit, 0);
  crubit(bit, 1);
}

int __attribute__((noinline)) hasCorcomp(int crubase) {
  volatile int* lower_exp = (volatile int*)0x2000;

  crubit(crubase + 2, 0);
  *lower_exp = 0xCCCC;
  if (*lower_exp == 0xCCCC) {
    return 0;
  }

  crubit(crubase + 2, 1);

  corcompBank(0, crubase);
  *lower_exp = 0x1234;
  corcompBank(1, crubase);
  *lower_exp = 0;
  corcompBank(0, crubase);

  if (*lower_exp == 0x1234) {
    corcompCru = crubase;
    return 1;
  } else {
    corcompCru = 0;
    return 0;
  }
}

int __attribute__((noinline)) corcompPagecount() {
  volatile int* lower_exp = (volatile int*)0x2000;
  for (int i = 0; i < 16; i++) {
    corcompBank(i, corcompCru);
    *lower_exp = 0x1234;
  }
  corcompBank(0, corcompCru);
  int pages = 0;
  while (pages < 16 && *lower_exp != 0xFFFF) {
    *lower_exp = 0xFFFF;
    corcompBank(++pages, corcompCru);
  }
  corcompBank(0, corcompCru);
  return pages;
}

int __attribute__((noinline)) testCorcomp(int pagecount) {
  writestring(2, 20, int2str(32 * pagecount));
  writestring(2, 23, "K");

  int ec = 0;
  for (int j = 0; j < pagecount && ec == 0; j++) {
    corcompBank(j, corcompCru);
    writestring(4, 16, "page ");
    writestring(4, 21, int2str(j));

    ec += testBlock(6, (unsigned char*)0x2000);
    ec += testBlock(7, (unsigned char*)0xA000);
    ec += testBlock(8, (unsigned char*)0xC000);
    ec += testBlock(9, (unsigned char*)0xE000);
  }
  return ec;
}