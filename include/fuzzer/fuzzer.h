#ifndef FUZZER_H 
#define FUZZER_H 

#include "../grammar.h"
#include <time.h>

/**
 * @brief Perform inverse unification on a given key in a grammar and stores
 * the fuzzed strings into a `TokenArray`.
 * 
 * @param key
 * @param grammar
 * @param fuzzed
 */
void unify_key_inv(Token key, Grammar* grammar, TokenArray* fuzzed);

/**
 * @brief For each token of the given rule, generate some terminal strings by
 * running `unify_key_inv` on it.
 * 
 * @param rule A specific rule contained in a NonTerminal from which to generate strings.
 * @param grammar A Grammar struct representing a BNF grammar.
 * @param fuzzed A previously allocated array in which to store all fuzzed strings.
 */
void unify_rule_inv(Rule rule, Grammar* grammar, TokenArray* fuzzed);

#endif