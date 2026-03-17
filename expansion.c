extern char* int2str(int value);

#include <vdp.h>
#include <system.h>

extern int try_limit;
extern int corcompCru;

/* ---------------------------------------------------------
   CRU helpers
--------------------------------------------------------- */

void __attribute__((noinline)) crubit(int cru, int onoff) {
  __asm__(
    "MOV %0,r12\n\t"
    : : "r" (cru) : "r12"
  );

  if (onoff) {
    __asm__("SBO 0\n\t");
  } else {
    __asm__("SBZ 0\n\t");
  }
}

/* ---------------------------------------------------------
   Foundation / Myarc
--------------------------------------------------------- */

#define CRU_FOUNDATION 0x1E00
#define CRU_MYARC      0x1000

void __attribute__ ((noinline)) foundationBank(int page, int crubase) {
  // int adjusted = page << 8;
  int adjusted = page << 32;

  __asm__(
    "MOV %1, r12\n\t"
    "LDCR %0,8\n\t"
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
  for(int i = 0; i < 64; i++) {
    foundationBank(i, crubase);
    *lower_exp = 0x1234;
  }
  foundationBank(0, crubase);
  int pages = 0;
  while(pages < 64 && *lower_exp != 0xFFFF) {
    *lower_exp = 0xFFFF;
    foundationBank(++pages, crubase);
  }
  foundationBank(0, crubase);
  return pages;
}

int __attribute__ ((noinline)) testFoundation(int pagecount, int crubase) {
  extern int testBlock(const unsigned int row, unsigned char* addr);

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

/* ---------------------------------------------------------
   CorComp
--------------------------------------------------------- */

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
  extern int testBlock(const unsigned int row, unsigned char* addr);

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

/* ---------------------------------------------------------
   SAMS
--------------------------------------------------------- */

void __attribute__ ((noinline)) samsMapOn() {
  __asm__(
    "LI r12, >1E00\n\t"
    "SBO 1\n\t"
  );
}

void __attribute__ ((noinline)) samsMapOff() {
  __asm__(
    "LI r12, >1E00\n\t"
    "SBZ 1\n\t"
  );
}

void __attribute__ ((noinline)) samsMapPage(int page, int location) {
  __asm__(
      "LI r12, >1E00\n\t"
      "SRL %0, 12\n\t"
      "SLA %0, 1\n\t"
      "SWPB %1\n\t"
      "SBO 0\n\t"
      "MOV %1, @>4000(%0)\n\t"
      "SBZ 0\n\t"
      "SWPB %1\n\t"
      :
      : "r"(location), "r"(page)
      : "r12");
}

int __attribute__ ((noinline)) hasSams() {
  volatile int* lower_exp = (volatile int*) 0x2000;
  samsMapOn();
  samsMapPage(0, 0x2000);
  *lower_exp = 0x1234;
  samsMapPage(1, 0x2000);
  *lower_exp = 0;
  samsMapPage(0, 0x2000);
  int detected = (*lower_exp == 0x1234);
  samsMapOff();
  return detected;
}

int __attribute__ ((noinline)) samsPagecount() {
  samsMapOn();
  volatile int* lower_exp = (volatile int*) 0x2000;

  for(int i = 0; i < 4096; i++) {
    samsMapPage(i, 0x2000);
    *lower_exp = 0x1234;
  }

  samsMapPage(0, 0x2000);
  int pages = 0;
  while(pages < 4096 && *lower_exp != 0xFFFF) {
    *lower_exp = 0xFFFF;
    samsMapPage(++pages, 0x2000);
  }
  samsMapOff();
  return pages;
}

int __attribute__ ((noinline)) testSams(int pagecount) {
  extern int testBlock(const unsigned int row, unsigned char* addr);

  writestring(2, 16, int2str(4*pagecount));
  writestring(2, 21, "K");

  int ec = 0;
  for (int j = 0; j < pagecount && ec == 0; j += 8 ) {
    samsMapOn();
    samsMapPage(j,   0x2000);
    samsMapPage(j+1, 0x3000);
    samsMapPage(j+2, 0xA000);
    samsMapPage(j+3, 0xB000);
    samsMapPage(j+4, 0xC000);
    samsMapPage(j+5, 0xD000);
    samsMapPage(j+6, 0xE000);
    samsMapPage(j+7, 0xF000);

    writestring(4, 16, "pages            ");
    writestring(4,22, int2str(j));
    writestring(4,26, "->");
    writestring(4,29, int2str(j+7));

    ec += testBlock(6, (unsigned char*)0x2000);
    ec += testBlock(7, (unsigned char*)0xA000);
    ec += testBlock(8, (unsigned char*)0xC000);
    ec += testBlock(9, (unsigned char*)0xE000);

    samsMapOff();
  }
  return ec;
}