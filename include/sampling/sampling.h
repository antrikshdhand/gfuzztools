#ifndef SAMPLING_H 
#define SAMPLING_H 

#include "../grammar.h"
#include "hash.h"
#include <time.h>

/**
 * @brief Retrieves or computes the definition of a non-terminal or terminal key
 * in the grammar given a specific string length. A definition is characterised 
 * by a KeyNode struct and essentially contains the number of strings of length 
 * `l_str` a `token` can produce.
 * 
 * If not already memoized, this function searches for the definition of the 
 * specified key in the grammar and returns a KeyNode representing the result. 
 * If the key is a non-terminal, it recursively explores its rules to construct
 * the definition. If the key is a terminal, it checks if the provided length 
 * `l_str` matches the length of the corresponding string in the grammar.
 * 
 * @param key The token representing the non-terminal or terminal key for which
 *      the definition needs to be retrieved or computed.
 * @param grammar A pointer to the Grammar structure containing non-terminals 
 *      and their rules.
 * @param l_str The length of the string to be produced by the key.
 * 
 * @return KeyNode* A pointer to the KeyNode representing the definition of the 
 *      specified key. If the key is not found or the length mismatch occurs, 
 *      an appropriate empty KeyNode is created to indicate the result. 
 * 
 * @note It is the responsibility of the caller to free the memory allocated 
 *      for the KeyNode when it is no longer needed.
 * 
 * @note This function requires the three hash tables `key_strs`, `rule_strs` 
 *      and `grammar_hash` to be defined as global variables in the calling 
 *      program.
 * 
 * @see create_key_node, insert_key, get_key, rules_get_def, free_key_node
 */
KeyNode* key_get_def(Token key, Grammar* grammar, size_t l_str);

/**
 * @brief Retrieves or computes the definition of a rule in the grammar given a 
 * specific string length.
 * 
 * This function recursively explores the production rules defined by the given 
 * Rule structure, attempting to construct a definition for the specified string
 * length. The definition is characterised by a RuleNode struct, representing 
 * the combination of different subrules that can produce strings of the desired 
 * length.
 * 
 * If the result is not already memoized, the function searches for the 
 * definition using recursion and returns a RuleNode representing the result. 
 * If the rule is composed of multiple tokens, the function divides the string 
 * length and explores the combinations of subrules that can produce each 
 * portion.
 * 
 * @param rule A pointer to the Rule structure representing the production rule 
 *      to be analysed.
 * @param grammar A pointer to the Grammar structure containing non-terminals 
 *      and their rules.
 * @param l_str The length of the string to be produced by the rule.
 * 
 * @return RuleNode* A pointer to the RuleNode representing the definition of 
 *      the specified rule for the given string length. If the rule is unable 
 *      to produce strings of the desired length, or if the result is memoized
 *      as empty, the function returns NULL.
 * 
 * @note It is the responsibility of the caller to free the memory allocated 
 *      for the RuleNode when it is no longer needed.
 * 
 * @see key_get_def, create_rule_node, insert_rule, get_rule, free_rule_node
 */
RuleNode* rules_get_def(Rule* rule, Grammar* grammar, size_t l_str);

/**
 * @brief Retrieves the total count of strings that a KeyNode can produce.
 * 
 * If the count of the KeyNode is 1, it directly returns 1. Otherwise, it 
 * iterates through the linked list of RuleNodes associated with the KeyNode, 
 * calculating the count for each rule and accumulating the total count.
 * 
 * @param kn A pointer to the KeyNode for which the count needs to be calculated.
 * @return int The total count of strings that the KeyNode can produce.
 */
int key_get_count(KeyNode* kn);

/**
 * @brief Retrieves the total count of strings that a RuleNode can produce.
 * 
 * The function calculates the count by multiplying the count of the RuleNode's key
 * with the count of each subsequent RuleNode in the linked list. If the RuleNode has
 * no tail, it directly returns the count of its key.
 * 
 * @param rn A pointer to the RuleNode for which the count needs to be calculated.
 * @return int The total count of strings that the RuleNode can produce.
 */
int rule_get_count(RuleNode* rn);

/**
 * @brief Extracts all possible strings generated by a KeyNode in the grammar.
 * 
 * If the KeyNode has no associated rules, it returns a DynTokenArray (DTA)
 * representing a single string with the KeyNode's token. Otherwise, it iterates
 * through the linked list of RuleNodes associated with the KeyNode, 
 * extracting strings for each rule, and concatenates them into a linked list 
 * of DynTokenArrays representing all possible strings.
 * 
 * @param kn A pointer to the KeyNode for which strings need to be extracted.
 * @return DynTokenArray* A linked list of DynTokenArrays representing all 
 *      possible strings generated by the KeyNode.
 * 
 * @note It is the responsibility of the caller to free the memory allocated 
 *      for the DTA when it is no longer needed.
 * 
 * @see rule_extract_strings, append_token_arrs, concat_dta_lists, 
 *      dta_lists_duplicate
 */
DynTokenArray* key_extract_strings(KeyNode* kn);

/**
 * @brief Extracts all possible strings generated by a RuleNode in the grammar.
 * 
 * The function extracts strings for the head of the rule using 
 * `key_extract_strings`, and then recursively combines them with the strings 
 * generated by the tail of the rule.
 * 
 * @param rn A pointer to the RuleNode for which strings need to be extracted.
 * @return DynTokenArray* A linked list of DynTokenArrays representing all 
 *      possible strings generated by the RuleNode. 
 * 
 * @note It is the responsibility of the caller to free the memory allocated 
 *      for the DTA when it is no longer needed.
 * 
 * @see key_extract_strings, rule_extract_strings, append_token_arrs, concat_dta_lists, dta_lists_duplicate
 */ 
DynTokenArray* rule_extract_strings(RuleNode* rn);

/**
 * @brief Retrieves the string at a specified position from a KeyNode's list of
 * possible strings.
 * 
 * If the specified position is beyond the total count of strings the KeyNode 
 * can produce, it prints an error message and returns NULL. 
 * 
 * If the KeyNode has no associated rules, it creates a DynTokenArray (DTA) 
 * representing a single string with the KeyNode's token. Otherwise, it 
 * iterates through the linked list of RuleNodes associated with the KeyNode, 
 * finding the rule that covers the specified position and extracts the 
 * corresponding string.
 * 
 * @param kn A pointer to the KeyNode for which the string needs to be retrieved.
 * @param at The 0-indexed position of the string to be retrieved.
 * @return DynTokenArray* A DynTokenArray representing the string at the 
 *      specified position.
 * 
 * @note It is the responsibility of the caller to free the memory allocated 
 *      for the DTA when it is no longer needed.
 * 
 * @note This function requires the three hash tables `key_strs`, `rule_strs` 
 *      and `grammar_hash` to be defined as global variables in the calling 
 *      program.
 * 
 * @see rule_get_string_at
 */
DynTokenArray* key_get_string_at(KeyNode* kn, int at);

/**
 * @brief Retrieves the string at a specified position from a RuleNode's list
 * of possible strings.
 * 
 * If the specified position is beyond the total count of strings the RuleNode
 * can produce, it prints an error message and returns NULL. 
 * 
 * If the RuleNode has no tail, it delegates the task to key_get_string_at for 
 * the head of the rule. Otherwise, it iterates through the linked list of 
 * RuleNodes associated with the tail, finding the rule that covers the 
 * specified position and concatenates the strings generated by the head and 
 * tail rules.
 * 
 * @param rn A pointer to the RuleNode for which the string needs to be 
 *      retrieved.
 * @param at The position of the string to be retrieved.
 * @return DynTokenArray* A DynTokenArray representing the string at the 
 *      specified position.
 * 
 * @note It is the responsibility of the caller to free the memory allocated 
 *      for the DTA when it is no longer needed.
 * 
 * @see key_get_string_at, concat_token_arrs
 */
DynTokenArray* rule_get_string_at(RuleNode* rn, int at);

/**
 * Uniformly at random samples a string of length `l_str` from the `grammar` 
 * starting from the specified `key`.
 * 
 * Ensure to set the seed of the rand() function in your calling function using
 * `srand((unsigned int)time(NULL));` to ensure randomness.
 * 
 * @param key The starting key for sampling.
 * @param grammar Pointer to the Grammar structure.
 * @param l_str The desired length of the sampled string.
 * 
 * @return DynTokenArray* A dynamically allocated single TokenArray representing 
 *      the sampled string.
 * 
 * @see key_get_def, key_get_string_at
 * 
 * @note This function requires the three hash tables `key_strs`, `rule_strs` 
 *      and `grammar_hash` to be defined as global variables in the calling 
 *      program.
 */
DynTokenArray* string_sample_UAR(Token key, Grammar* grammar, size_t l_str);

#endif // SAMPLING.h