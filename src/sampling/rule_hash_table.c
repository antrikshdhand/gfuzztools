#include "../../include/sampling/sampling.h"
#include "../../include/sampling/helpers.h"

int hash_rule(Rule* rule, size_t l_str)
{
    uint32_t hash = 5382;

    for (size_t i = 0; i < rule->num_tokens; i++)
    {
        hash = ((hash << 5) + hash) ^ rule->tokens[i];
    }
    hash = ((hash << 5) + hash) ^ (uint32_t)l_str;

    return hash % RULE_TABLE_SIZE;
}

void init_rule_hash_table(RuleHashTable* table)
{
    // Set entire table to be empty.
    for (size_t i = 0; i < RULE_TABLE_SIZE; i++)
    {
        (*table)[i] = NULL;
    } 
}

// void print_rule_hash_table(RuleHashTable* table)
// {
//     printf("------------ RULE HASH TABLE --------------\n");
//     for (size_t i = 0; i < RULE_TABLE_SIZE; i++)
//     {
//         if ((*table)[i] == NULL)
//         {
//             printf("\t%zu\t---\n", i);
//         }
//         else
//         {
//             printf("\t%zu\t", i);
//             RuleNode* tmp = (*table)[i];
//             while (tmp != NULL) {
//                 printf("(0x%x, l: %lu, c: %d, tail: [", tmp->key->token, tmp->l_str, tmp->count);
                
//                 // Print the tail linked list
//                 RuleNode* tailNode = tmp->tail;
//                 while (tailNode != NULL) {
//                     printf("(0x%x, l: %lu, c: %d) -> ", tailNode->key->token, tailNode->l_str, tailNode->count);
//                     tailNode = tailNode->next;
//                 }
                
//                 printf("]) -> ");
                
//                 tmp = tmp->next;
//             }
//             printf("\n");
//         }
//     }
//     printf("--------------------------------------------\n");
// }

void insert_rule(RuleHashTable* table, Rule* rule, size_t l_str, RuleNode* rn)
{
    if (rn == NULL) return;

    int index = hash_rule(rule, l_str);

    RuleHashTableVal* new_val = malloc(sizeof(RuleHashTableVal));
    new_val->list = rn;
    new_val->l_str = l_str;
    new_val->next = (*table)[index];
    (*table)[index] = new_val;
}

// Here tmp is the head of the linked list at a specific RuleHashTableVal.
int rule_list_equals(RuleNode* head, Rule* rule) 
{
    RuleNode* ptr = head;
    while (ptr != NULL)
    {
        // Check if head token is equal.
        size_t head_index;
        for (head_index = 0; head_index < rule->num_tokens; head_index++)
        {
            if (rule->tokens[head_index] != EMPTY_TOKEN)
                break;
        }
        
        if (rule->tokens[head_index] != ptr->key->token)
            return 0;
        
        if (rule->tokens[head_index + 1] != ptr->tail->key->token)
            return 0;

        ptr = ptr->next;
    }

    return 1;
}

RuleNode* get_rule(RuleHashTable* table, Rule* rule, size_t l_str)
{
    int index = hash_rule(rule, l_str);
    
    RuleHashTableVal* tmp = (*table)[index];
    while (tmp != NULL) 
    {
        if (tmp->l_str == l_str) {
            if (rule_list_equals(tmp->list, rule)) 
            {
                return tmp->list;
            }
        }
        tmp = tmp->next;
    }
    return NULL;
}

void breakdown_rule_hash_table(RuleHashTable* table)
{
    for (size_t i = 0; i < RULE_TABLE_SIZE; i++)
    {
        free_rule_node((*table)[i]);
    }
}

void print_rule_node(RuleNode* rn)
{
    if (rn == NULL)
    {
        printf("NULL\n");
        return;
    }

    printf("key: 0x%x, l_str: %lu, count: %d\n", rn->key->token, rn->l_str, rn->count);
}