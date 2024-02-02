#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// Change as per the size of your grammar.
#define MAX_TOKENS_IN_RULE 1024 
#define MAX_RULES_FOR_NONTERMINAL 1024
#define MAX_NONTERMINALS_IN_GRAMMAR 128
#define MAX_STRINGS_IN_LANGUAGE 100

/**
 * A Grammar is an array of NonTerminals. 
 * A NonTerminal has a name (an 8-bit id), and an array of rules.
 * A Rule consists of several Tokens, e.g., "<article> <noun>".
 * The Tokens are written in 8-bit form.
 */

typedef uint8_t Token;                  // Represents an 8-bit token.

typedef struct Rule
{
    size_t num_tokens;                  // Number of tokens in the rule.
    Token tokens[MAX_TOKENS_IN_RULE];   // Array of tokens in the rule.
} Rule;

typedef struct NonTerminal
{
    Token name;                             // 8-bit id representing the name of the non-terminal.
    size_t num_rules;                       // Number of rules associated with the non-terminal.
    Rule rules[MAX_RULES_FOR_NONTERMINAL];  // Array of rules associated with the non-terminal.
} NonTerminal;

typedef struct Grammar
{
    size_t num_non_terminals;               // Number of non-terminals in the grammar.
    NonTerminal non_terminals[MAX_NONTERMINALS_IN_GRAMMAR];    // Array of non-terminals in the grammar.
} Grammar;

typedef struct TokenArray 
{
    size_t index;                           // Current index in the token array.
    Token tokens[MAX_STRINGS_IN_LANGUAGE];  // Array of tokens representing strings in the language.
} TokenArray;

extern Grammar GRAMMAR;  // Declaration of the external variable representing the grammar.

/**
 * @brief Tests if the given key is a non-terminal by checking if the MSB is
 * set. If so, the function returns the index of the non-terminal in the
 * Grammar `non_terminals` array. The index will be the LSB, given that the 
 * Grammar struct was created properly.
 * 
 * @param key The key to be verified as non-terminal.
 * @param grammar The grammar struct representing a BNF grammar.
 * @return int The index of the non-terminal inside the grammar, otherwise -1.
 */
int is_non_terminal(Token key);

void print_token_array(TokenArray* fuzzed);

#endif