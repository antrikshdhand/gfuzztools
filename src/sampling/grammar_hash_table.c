#include "../../include/sampling/sampling.h"
#include "../../include/sampling/helpers.h"

int hash(Token key)
{
    // Jenkins one-at-a-time hash
    uint32_t hash = 0;

    for (size_t i = 0; i < 8; ++i) {
        hash += key & 1;
        hash += (hash << 10);
        hash ^= (hash >> 6);
        key >>= 1;
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash % GRAMMAR_TABLE_SIZE;
}

void print_grammar_hash_table(GrammarHashTable* table)
{
    printf("----------- GRAMMAR HASH TABLE -------------\n");
    for (size_t i = 0; i < GRAMMAR_TABLE_SIZE; i++)
    {
        if ((*table)[i] == NULL)
        {
            printf("\t%zu\t---\n", i);
        }
        else
        {
            printf("\t%zu\t", i);
            GrammarHashTableVal* tmp = (*table)[i];
            while (tmp != NULL) {
                printf("%s -> ", tmp->str);
                tmp = tmp->next;
            }
            printf("\n");
        }
    }
    printf("-----------------------------------------\n");
}

int insert_grammar(GrammarHashTable* table, GrammarHashTableVal* ts)
{
    if (ts == NULL) return -1;
    int index = hash(ts->key);

    // Replace the current start of the linked-list with p.
    ts->next = (*table)[index];
    (*table)[index] = ts;

    return 0;
}

GrammarHashTableVal* get_grammar(GrammarHashTable* table, Token key)
{
    int index = hash(key);
    GrammarHashTableVal* tmp = (*table)[index];
    while (tmp != NULL && tmp->key != key)
    {
        tmp = tmp->next;
    }
    return tmp;
}

GrammarHashTableVal* delete_grammar(GrammarHashTable* table, Token key)
{
    int index = hash(key);
    
    GrammarHashTableVal* tmp = (*table)[index];
    GrammarHashTableVal* prev = NULL;
    while (tmp != NULL && tmp->key != key)
    {
        prev = tmp;
        tmp = tmp->next;
    }

    if (tmp == NULL) return NULL; // No match found.

    if (prev == NULL)
    {
        // The node to delete was the head of the linked-list.
        (*table)[index] = tmp->next;
    }
    else
    {
        prev->next = tmp->next;
    }

    return tmp;
}

void free_token_str(GrammarHashTableVal *token_str) 
{
    while (token_str != NULL) {
        GrammarHashTableVal *next = token_str->next;
        free(token_str->str);
        free(token_str);
        token_str = next;
    }
}

void breakdown_grammar_hash_table(GrammarHashTable* table)
{
    for (size_t i = 0; i < GRAMMAR_TABLE_SIZE; i++) {
        free_token_str((*table)[i]);
    }
}

void insert_token_str(GrammarHashTable* table, Token key, 
    char* str, int strlen) 
{
    GrammarHashTableVal* ts = malloc(sizeof(GrammarHashTableVal));
    *ts = (GrammarHashTableVal) {key, strdup(str), strlen, NULL};
    insert_grammar(table, ts);
}

void init_grammar_hash_table(GrammarHashTable* table)
{
    // Set entire table to be empty.
    for (size_t i = 0; i < GRAMMAR_TABLE_SIZE; i++)
    {
        (*table)[i] = NULL;
    } 

    insert_token_str(table, 0x80, "<start>", LENGTH_NA);
    insert_token_str(table, 0x81, "<sentence>", LENGTH_NA);
    insert_token_str(table, 0x82, "<noun_phrase>", LENGTH_NA);
    insert_token_str(table, 0x83, "<verb>", LENGTH_NA);
    insert_token_str(table, 0x84, "<article>", LENGTH_NA);
    insert_token_str(table, 0x85, "<noun>", LENGTH_NA);
    insert_token_str(table, 0x00, "horse", 5);
    insert_token_str(table, 0x01, "dog", 3);
    insert_token_str(table, 0x02, "hamster", 7);
    insert_token_str(table, 0x03, "a", 1);
    insert_token_str(table, 0x04, "the", 3);
    insert_token_str(table, 0x05, "stands", 6);
    insert_token_str(table, 0x06, "walks", 5);
    insert_token_str(table, 0x07, "jumps", 5);
}
