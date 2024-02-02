#ifndef HASH_H
#define HASH_H

#include <string.h>
#include "../grammar.h"

// Use the print_%_hash_table functions to adjust these empirically.
#define GRAMMAR_TABLE_SIZE 20
#define KEY_TABLE_SIZE 60 
#define RULE_TABLE_SIZE 30 

#define LENGTH_NA -1
#define EMPTY_TOKEN 0x88

// We hash these to retrieve an index.
typedef struct 
{
    Token key;          // Represents the token associated with the key.
    size_t l_str;       // The length of the string we want to generate.
} KeyLengthTuple;

// What we are storing in the HashMaps.
typedef struct KeyNode KeyNode;
typedef struct RuleNode RuleNode;
typedef struct RuleHashTableVal RuleHashTableVal;

// Represents a node in the linked list of keys.
struct KeyNode
{
    Token token;            // Token associated with the key.
    size_t l_str;           // Length of the string we want to produce.
    int count;              // The number of strings of length l_str that token can produce.
    RuleNode* rules;        // Pointer to a linked list of RuleNode structs representing the rules associated with the key.
    struct KeyNode* next;   // Pointer to the next KeyNode in the linked list.
};

// Represents a node in the linked list of rules.
struct RuleNode
{
    struct KeyNode* key;    // Pointer to the KeyNode struct representing the head or starting point of the rule.
    RuleNode* tail;         // Pointer to the tail or continuation of the rule.
    size_t l_str;           // The length of the string we want to produce.
    int count;              // The number of strings of length l_str that key can produce.
    struct RuleNode* next;  // Pointer to the next RuleNode in the linked list.
};

// Represents the values stored in the RuleHashTable, which is a linked list filled with RuleNodes.
struct RuleHashTableVal
{
    RuleNode* list;                 // Pointer to the head of a linked list of RuleNode structs.
    size_t l_str;                   // The length of the string associated with the rule.
    struct RuleHashTableVal* next;  // Pointer to the next RuleHashTableVal in the linked list.
};

// Represents the values stored in the GrammarHashTable for external chaining.
typedef struct GrammarHashTableVal 
{
    Token key;                          // Token associated with the key.
    char* str;                          // Pointer to a string associated with the key.
    size_t strlen;                      // Length of the string associated with the key.
    struct GrammarHashTableVal* next;   // Pointer to the next GrammarHashTableVal for external chaining.
} GrammarHashTableVal;

// Represents a "string" as an array of tokens.
typedef struct DynTokenArray
{
    Token* list;                        // Pointer to an array of tokens.
    size_t length;                      // The length of the array.
    struct DynTokenArray* next_dta;     // Pointer to the next DynTokenArray in the linked list.
} DynTokenArray;

// Our hash tables are arrays of pointers to structs.
typedef KeyNode* KeyHashTable[KEY_TABLE_SIZE];
typedef RuleHashTableVal* RuleHashTable[RULE_TABLE_SIZE];
typedef GrammarHashTableVal* GrammarHashTable[GRAMMAR_TABLE_SIZE];

/* FUNCTION DECLARATIONS */

// key_hash_table.c
int hash_key(Token key, size_t l_str);
void init_key_hash_table(KeyHashTable* table);
void print_key_hash_table(KeyHashTable* table);
void insert_key(KeyHashTable* table, Token key, size_t l_str, KeyNode* kn);
KeyNode* get_key(KeyHashTable* table, Token key, size_t l_str);
void breakdown_key_hash_table(KeyHashTable* table);
void print_key_node(KeyNode* kn);

// rule_hash_table.c
int hash_rule(Rule* rule, size_t l_str);
void init_rule_hash_table(RuleHashTable* table);
// void print_rule_hash_table(RuleHashTable* table);
void insert_rule(RuleHashTable* table, Rule* rule, size_t l_str, RuleNode* rn);
int rule_list_equals(RuleNode* head, Rule* rule);
RuleNode* get_rule(RuleHashTable* table, Rule* rule, size_t l_str);
void breakdown_rule_hash_table(RuleHashTable* table);
void print_rule_node(RuleNode* rn);

// grammar_hash_table.c
int hash(Token key);
void print_grammar_hash_table(GrammarHashTable* table);
int insert_grammar(GrammarHashTable* table, GrammarHashTableVal* ts);
GrammarHashTableVal* get_grammar(GrammarHashTable* table, Token key);
GrammarHashTableVal* delete_grammar(GrammarHashTable* table, Token key);
void free_token_str(GrammarHashTableVal *token_str);
void breakdown_grammar_hash_table(GrammarHashTable* table);
void insert_token_str(GrammarHashTable* table, Token key, 
    char* str, int strlen);
void init_grammar_hash_table(GrammarHashTable* table);

#endif // HASH_H