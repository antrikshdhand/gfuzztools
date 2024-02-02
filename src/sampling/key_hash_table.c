#include "../../include/sampling/sampling.h"
#include "../../include/sampling/helpers.h"

int hash_key(Token key, size_t l_str)
{
    uint32_t hash = 17;

    // Combine the key and l_str into the hash
    hash = (hash << 5) ^ key ^ (uint32_t)l_str;

    return hash % KEY_TABLE_SIZE;
}

void init_key_hash_table(KeyHashTable* table)
{
    // Set entire table to be empty.
    for (size_t i = 0; i < KEY_TABLE_SIZE; i++)
    {
        (*table)[i] = NULL;
    } 
}

void print_key_hash_table(KeyHashTable* table)
{
    printf("------------ KEY HASH TABLE -------------\n");
    for (size_t i = 0; i < KEY_TABLE_SIZE; i++)
    {
        if ((*table)[i] == NULL)
        {
            printf("\t%zu\t---\n", i);
        }
        else
        {
            printf("\t%zu\t", i);
            KeyNode* tmp = (*table)[i];
            while (tmp != NULL) {
                if (tmp->token == EMPTY_TOKEN) {
                    printf("EMPTY_KEY -> ");
                } else {
                    if (tmp->count != 0)
                    printf("(0x%x, l: %lu, c: %d) -> ", tmp->token, tmp->l_str, tmp->count);
                }
                tmp = tmp->next;
            }
            printf("\n");
        }
    }
    printf("-------------------------------------------\n");
}

void insert_key(KeyHashTable* table, Token key, size_t l_str, KeyNode* kn)
{
    if (kn == NULL) return;

    int index = hash_key(key, l_str);

    // Replace the current start of the linked-list with kn.
    kn->next = (*table)[index];
    (*table)[index] = kn;
}

KeyNode* get_key(KeyHashTable* table, Token key, size_t l_str)
{
    int index = hash_key(key, l_str);
    KeyNode* tmp = (*table)[index];
    while (tmp != NULL && (tmp->token != key || tmp->l_str != l_str))
    {
        tmp = tmp->next;
    }
    return tmp;
}

void breakdown_key_hash_table(KeyHashTable* table)
{
    for (size_t i = 0; i < KEY_TABLE_SIZE; i++)
    {
        free_key_node((*table)[i]);
    }
}

void print_key_node(KeyNode* kn)
{
    if (kn == NULL) 
    {
        printf("NULL\n");
        return;
    }

    if (kn->rules == NULL)
    {
        printf("key: 0x%x, l_str: %lu, count: %d\n, rules: NULL", kn->token, kn->l_str, kn->count);
    }
    else
    {
        printf("key: 0x%x, l_str: %lu, count: %d, rules[0] = 0x%x\n", kn->token, kn->l_str, kn->count, kn->rules[0].key->token);
    }
}