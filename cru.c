// cru.c — CRU bit helper

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