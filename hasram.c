extern int hasFoundation(int crubase);
extern int hasCorcomp(int crubase);
extern int hasSams(void);

/* ---------------------------------------------------------
   Basic 32K RAM detection
   --------------------------------------------------------- */
static int has32k(void)
{
    volatile int *lower = (volatile int*)0x2000;
    *lower = 0xA55A;
    return (*lower == 0xA55A);
}

/* ---------------------------------------------------------
   Top-level RAM presence check
   --------------------------------------------------------- */
int hasRam(void)
{
    /* SAMS provides RAM */
    if (hasSams())
        return 1;

    /* CorComp 512K card */
    if (hasCorcomp(0x1F00))
        return 1;

    /* Foundation/Myarc 128K/512K card */
    if (hasFoundation(0x1E00))
        return 1;

    /* Standard 32K expansion */
    if (has32k())
        return 1;

    /* No RAM detected */
    return 0;
}
