#include "../include/grammar.h"

int is_non_terminal(Token key) 
{
    if ((key & 0x80) == 0x80) {
        return key & 0x0F;
    }
    return -1;
}