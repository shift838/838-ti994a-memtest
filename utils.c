#include <vdp.h>
#include <system.h>
#include <string.h>

void writehex(unsigned int row, unsigned int col, const unsigned int value)
{
    char buf[5];
    static const char hex[] = "0123456789ABCDEF";

    buf[0] = hex[(value >> 12) & 0xF];
    buf[1] = hex[(value >> 8) & 0xF];
    buf[2] = hex[(value >> 4) & 0xF];
    buf[3] = hex[(value >> 0) & 0xF];
    buf[4] = 0;

    writestring(row, col, buf);
}

void printSummary(int ec)
{
    if (ec == 0)
        writestring(12, 10, "ALL TESTS PASSED");
    else
        writestring(12, 10, "ERRORS DETECTED");
}
