#ifndef HELPERS_H
#define HELPERS_H

#include "sampling.h"

/**
 * @brief Create a KeyNode object on the stack and return a pointer to it.
 * A KeyNode object has (key, l_str) as its primary key – that is, a new
 * KeyNode object is created for a new combination of (key, l_str).
 * 
 * @param key Represents the token associated with `key`.
 * @param l_str The length of the string we want to produce.
 * @param count The number of strings of length `l_str` that `key` can produce.
 * @param rules A pointer to a linked list of `RuleNode` structs, representing 
 *      the rules associated with `key`.
 * @return KeyNode* A pointer to a new `KeyNode` object.
 */
KeyNode* create_key_node(Token key, size_t l_str, int count, RuleNode* rules);

/**
 * @brief Create a RuleNode object on the stack and return a pointer to it. 
 * A RuleNode object has (key, l_str) as its primary key – that is, a new
 * RuleNode object is created for a new combination of (key, l_str).
 * 
 * @param key A pointer to a `KeyNode` struct, representing the head or 
 *      starting point of the rule.
 * @param tail A pointer to the tail or continuation of the rule. 
 * @param l_str The length of the string we want to produce.
 * @param count The number of strings of length `l_str` that `key` can produce. 
 * @return RuleNode* A pointer to a new RuleNode object.
 */
RuleNode* create_rule_node(KeyNode* key, RuleNode* tail, 
                            size_t l_str, int count);

/**
 * @brief Compare two individual `DynTokenArray`s (DTAs) for equality. 
 * Checks for equal lengths and equal contents of their respective lists.
 * 
 * @param dta1 A pointer to the first DTA to compare.
 * @param dta2 A pointer to the second DTA to compare.
 * @return int `1` if equal, otherwise `0`.
 */
int dtas_equal(DynTokenArray* dta1, DynTokenArray* dta2);

/**
 * @brief Compare two lists of `DynTokenArray`s (DTAs) for duplicate DTA
 * entries.
 * 
 * @param dta_list1 A pointer to the first DTA to check.
 * @param dta_list2 A pointer to the second DTA to check.
 * @return int `1` if duplicate found, `0` otherwise.
 */
int dta_lists_duplicate(DynTokenArray* dta_list1, DynTokenArray* dta_list2);

/**
 * @brief Append `arr2` to the end of `arr1`. Works regardless of whether 
 * `arr2` is a list of `DynTokenArray`s (DTAs) or an individual DTA.
 * 
 * @param arr1 A pointer to the head of a linked-list of DTAs.
 * @param arr2 A pointer to the DTA object/list to be appended.
 */
void append_token_arrs(DynTokenArray** arr1, DynTokenArray* arr2);

/**
 * @brief Concatenate the contents of two `DynTokenArray`s (DTAs) into
 * a third DTA and return a pointer to it. This is equivalent to
 * concatenating two strings (strcat(3)).
 * 
 * @param arr1 A pointer to the first DTA to be concatenated.
 * @param arr2 A pointer to the second DTA to be concatenated.
 * @return DynTokenArray* A pointer to the concatenated DTA.
 */
DynTokenArray* concat_token_arrs(DynTokenArray* arr1, DynTokenArray* arr2);

/**
 * @brief Pairwise concatenate two lists of `DynTokenArray`s (DTAs).
 * 
 * @example [one, two, three] and [four, five, six] would result in 
 * [one four, one five, one six, two four, ..., three five, three six].
 * 
 * @param list1 A pointer to the first list of DTAs to be concatenated.
 * @param list2 A pointer to the second list of DTAs to be concatenated.
 * @return DynTokenArray* A pointer to the head of the new concatenated list
 *      of DTAs. The original lists remain unchanged.
 */
DynTokenArray* concat_dta_lists(DynTokenArray* list1, DynTokenArray* list2);

void free_token_array(DynTokenArray* arr);

void free_key_node(KeyNode* kn);

void free_rule_node(RuleHashTableVal* val);

void print_dta(DynTokenArray* dta);

void print_list_of_dtas(DynTokenArray* head);

#endif // HELPERS.H