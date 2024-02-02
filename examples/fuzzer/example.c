#include "../../include/grammar.h"
#include "../../include/fuzzer/fuzzer.h"

/* data/test_grammar.txt */
Grammar GRAMMAR = {
    6,
    {
        {
            0x80,
            1,
            {
                {
                    1, 
                    {0x81}
                }
            }
        },
        {
            0x81,
            1,
            {
                {
                    2,
                    {0x82, 0x83}
                }
            }
        },
        {
            0x82,
            1,
            {
                {
                    2,
                    {0x84, 0x85}
                }
            }
        },
        {
            0x83,
            3,
            {
                {
                    1,
                    {0x05}
                },
                {
                    1,
                    {0x06}
                },
                {
                    1,
                    {0x07}
                }
            }
        },
        {
            0x84,
            2,
            {
                {
                    1,
                    {0x03}
                },
                {
                    1,
                    {0x04}
                }
            }
        },
        {
            0x85,
            3,
            {
                {
                    1,
                    {0x00}
                },
                {
                    1,
                    {0x01}
                },
                {
                    1,
                    {0x02}
                }
            }
        }
    }
};

#define START_TOKEN 0x80

int main() 
{
    srand((unsigned int)time(NULL));

    TokenArray fuzzed;
    fuzzed.index = 0;

    unify_key_inv(START_TOKEN, &GRAMMAR, &fuzzed);
    print_token_array(&fuzzed);

    return 0;
}