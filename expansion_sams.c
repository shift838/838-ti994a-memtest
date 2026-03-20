#include <vdp.h>
#include <system.h>

extern int testBlock(const unsigned int row, unsigned char* addr);
extern char* int2str(int value);

/* ---------------------------------------------------------
   SAMS mapping helpers
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

/* ---------------------------------------------------------
   SAMS detection
--------------------------------------------------------- */

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

/* ---------------------------------------------------------
   SAMS test — split into two functions to avoid PC8 overflow
--------------------------------------------------------- */

static int __attribute__ ((noinline))
testSamsBlock(int j) {
  int ec = 0;

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
  return ec;
}

int __attribute__ ((noinline))
testSams(int pagecount) {
  writestring(2, 16, int2str(4*pagecount));
  writestring(2, 22, "K");

  int ec = 0;
  for (int j = 0; j < pagecount && ec == 0; j += 8 ) {
    ec += testSamsBlock(j);
  }
  return ec;
}
