#include "../../include/sampling/sampling.h"
#include "../../include/sampling/hash.h"
#include "../../include/sampling/helpers.h"

Grammar GRAMMAR = {
	6,
	{
		{
			// <start>
			0x80,
			1,
			{
				{
					// <sentence>
					1,
					{0x81}
				}
			}
		},
		{
			// <sentence>
			0x81,
			1,
			{
				{
					// <noun_phrase>, <verb>
					2,
					{0x82, 0x83}
				}
			}
		},
		{
			// <noun_phrase>
			0x82,
			1,
			{
				{
					// <article>, <noun>
					2,
					{0x84, 0x85}
				}
			}
		},
		{
			// <verb>
			0x83,
			3,
			{
				{
					// stands
					1,
					{0x5}
				},
				{
					// walks
					1,
					{0x6}
				},
				{
					// jumps
					1,
					{0x7}
				}
			}
		},
		{
			// <article>
			0x84,
			2,
			{
				{
					// a
					1,
					{0x3}
				},
				{
					// the
					1,
					{0x4}
				}
			}
		},
		{
			// <noun>
			0x85,
			3,
			{
				{
					// horse
					1,
					{0x0}
				},
				{
					// dog
					1,
					{0x1}
				},
				{
					// hamster
					1,
					{0x2}
				}
			}
		}
	}
};

#define START_TOKEN 0x80

KeyHashTable key_strs;
RuleHashTable rule_strs;
GrammarHashTable grammar_hash;

int main()
{
    // Setup
    srand((unsigned int)time(NULL));
    init_key_hash_table(&key_strs);
    init_rule_hash_table(&rule_strs);
    init_grammar_hash_table(&grammar_hash);

    // Use
    DynTokenArray* string = string_sample_UAR(0x80, &GRAMMAR, 11);
    print_dta(string);

    // Cleanup
    breakdown_key_hash_table(&key_strs);
    breakdown_rule_hash_table(&rule_strs);
    breakdown_grammar_hash_table(&grammar_hash);

    return 0;
}