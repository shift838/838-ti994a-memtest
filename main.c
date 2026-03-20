#include <vdp.h>
#include <system.h>
#include <string.h>

// prototypes from expansion.c
int hasRam(void);
void printSummary(int ec);
void writehex(unsigned int row, unsigned int col, const unsigned int value);

// prototypes from testblock.c
int testBlock(const unsigned int row, unsigned char* addr);
int test32k(void);

// prototypes from expansion.c
int hasSams(void);
int samsPagecount(void);
int hasFoundation(int crubase);
int foundationPagecount(int crubase);
int hasCorcomp(int crubase);
int corcompPagecount(void);
int testSams(int pagecount);
int testFoundation(int pagecount, int crubase);
int testCorcomp(int pagecount);

#define PROG_VERSION "3.0"

extern char mcolor;
extern char scolor;
extern char fcolor;

#define SCREEN_COLOR mcolor
#define ERROR_COLOR  fcolor
#define SUCCESS_COLOR scolor

int try_limit;
int corcompCru;

#define NOMEM      0
#define FOUNDATION 1
#define SAMS       2
#define MYARC      3
#define BASE32K    4
#define CORCOMP    5

void main(int passcount)
{
    set_text();
    VDP_SET_REGISTER(VDP_REG_COL, SCREEN_COLOR);
    vdpmemset(0x0000, ' ', nTextEnd);
    charsetlc();

    char* ver = PROG_VERSION;

    try_limit = 12;
    if (passcount == 0)
    {
        try_limit = 1;
        passcount = 1;
        writestring(0, 0, "Memory Quick Check ver ");
        writestring(0, 23, ver);
    }
    else
    {
        writestring(0, 0, "Expansion Memory Test ver ");
        writestring(0, 26, ver);
    }

    writestring(23, 0, "- Modified by SHIFT838 & GaryOPA -");

    int memtype = NOMEM;

    if (!hasRam())
    {
        writestring(2, 0, "No RAM detected");
        while (1) { }
    }
    else if (hasSams() && samsPagecount() > (128 / 4))
    {
        writestring(2, 0, "SAMS detected");
        memtype = SAMS;
    }
    else if (hasCorcomp(0x1000) || hasCorcomp(0x1400))
    {
        writestring(2, 0, "Corcomp 256k/512k");
        memtype = CORCOMP;
    }
    else if (hasFoundation(0x1000))
    {
        writestring(2, 0, "Myarc detected");
        memtype = MYARC;
    }
    else if (hasFoundation(0x1E00))
    {
        writestring(2, 0, "Foundation detected");
        memtype = FOUNDATION;
    }
    else
    {
        writestring(2, 0, "Standard 32K detected");
        memtype = BASE32K;
    }

    if (passcount > 1)
    {
        writestring(4, 0, "Burnin");
        writestring(4, 7, "Pass ");
    }

    int pagecount = 0;

    if (memtype == SAMS)
    {
        pagecount = samsPagecount();
    }
    else if (memtype == MYARC)
    {
        pagecount = foundationPagecount(0x1000);
        //pagecount = 64;
    }
    else if (memtype == FOUNDATION)
    {
        pagecount = foundationPagecount(0x1E00);
    }
    else if (memtype == CORCOMP)
    {
        pagecount = corcompPagecount();
    }

    int ec = 0;
    for (int i = 1; i <= passcount && ec == 0; i++)
    {
        if (passcount > 1)
        {
            writestring(4, 12, int2str(i));
        }

        switch (memtype)
        {
            case BASE32K:
                ec += test32k();
                break;
            case SAMS:
                ec += testSams(pagecount);
                break;
            case MYARC:
                ec += testFoundation(pagecount, 0x1000);
                break;
            case FOUNDATION:
                ec += testFoundation(pagecount, 0x1E00);
                break;
            case CORCOMP:
                ec += testCorcomp(pagecount);
                break;
        }
    }

    printSummary(ec);

    while (1) { }
}
