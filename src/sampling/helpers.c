#include "../../include/sampling/helpers.h"

KeyNode* create_key_node(Token key, size_t l_str, int count, RuleNode* rules)
{
    KeyNode* kn = malloc(sizeof(KeyNode));
    kn->token = key;
    kn->l_str = l_str;
    kn->count = count;
    kn->rules = rules;
    kn->next = NULL;

    return kn;
}

RuleNode* create_rule_node(KeyNode* key, RuleNode* tail, 
                            size_t l_str, int count)
{
    RuleNode* rn = malloc(sizeof(RuleNode));
    rn->key = key;
    rn->tail = tail;
    rn->l_str = l_str;
    rn->count = count;
    rn->next = NULL;

    return rn;
}

int dtas_equal(DynTokenArray* dta1, DynTokenArray* dta2)
{
    if (dta1->length != dta2->length)
        return 0;
    
    for (size_t i = 0; i < dta1->length; i++)
    {
        if (dta1->list[i] != dta2->list[i])
        {
            return 0;
        }
    }

    return 1;
}

int dta_lists_duplicate(DynTokenArray* dta_list1, DynTokenArray* dta_list2)
{
    int is_duplicate = 0;
    DynTokenArray* outer_ptr = dta_list1;
    DynTokenArray* inner_ptr = dta_list2;
    while (outer_ptr != NULL)
    {
        inner_ptr = dta_list2;
        while (inner_ptr != NULL)
        {
            if (dtas_equal(outer_ptr, inner_ptr))
            {
                is_duplicate = 1;
                break;
            }
            inner_ptr = inner_ptr->next_dta;
        }
        outer_ptr = outer_ptr->next_dta;
    }

    return is_duplicate;
}

void append_token_arrs(DynTokenArray** arr1, DynTokenArray* arr2) 
{
    // Handle empty list case:
    if (*arr1 == NULL) 
    {
        *arr1 = arr2;
        return;
    }

    // Find the last node of the list:
    DynTokenArray* last = *arr1;
    while (last->next_dta != NULL) 
    {
        last = last->next_dta;
    }

    // Append arr2 to the end of the list:
    arr2->next_dta = NULL; 
    last->next_dta = arr2;
}

DynTokenArray* concat_token_arrs(DynTokenArray* arr1, DynTokenArray* arr2) 
{
    DynTokenArray* result = malloc(sizeof(DynTokenArray));
    result->length = arr1->length + arr2->length;
    result->list = malloc(result->length * sizeof(Token));
    result->next_dta = NULL;

    // Copy tokens from arr1
    for (size_t i = 0; i < arr1->length; i++) 
    {
        result->list[i] = arr1->list[i];
    }

    // Copy tokens from arr2
    for (size_t i = 0; i < arr2->length; i++) 
    {
        result->list[arr1->length + i] = arr2->list[i];
    }

    return result;
}

DynTokenArray* concat_dta_lists(DynTokenArray* list1, DynTokenArray* list2) 
{
    DynTokenArray* result = NULL;

    for (DynTokenArray* ptr1 = list1; ptr1 != NULL; ptr1 = ptr1->next_dta) 
    {
        for (DynTokenArray* ptr2 = list2; ptr2 != NULL; ptr2 = ptr2->next_dta)
        {
            DynTokenArray* concatenated = concat_token_arrs(ptr1, ptr2);
            append_token_arrs(&result, concatenated);
        }
    }

    return result;
}

void free_token_array(DynTokenArray* arr) 
{
    free(arr->list);
    free(arr);
}

void free_key_node(KeyNode* kn) 
{
    KeyNode* current = kn;
    while (current != NULL) {
        KeyNode* next = current->next;
        free(current);
        current = next;
    }
}

void free_rule_node(RuleHashTableVal* val) 
{
    RuleHashTableVal* current = val;
    while (current != NULL) {
        RuleHashTableVal* next = current->next;
        free(current->list);
        free(current);
        current = next;
    }
}

void print_dta(DynTokenArray* dta)
{
    if (dta == NULL)
    {
        printf("NULL\n");
        return;
    }

    for (size_t i = 0; i < dta->length; i++)
    {
        printf("0x%x ", dta->list[i]);
    }
    printf("\n");
}

void print_list_of_dtas(DynTokenArray* head)
{
    if (head == NULL)
    {
        printf("NULL\n");
        return;
    }

    int i = 0;
    while (head != NULL)
    {
        printf("DTA linked-list node %d (len: %lu):\n", i, head->length); 
        print_dta(head);
        i++;
        head = head->next_dta;
    }
    printf("\n");
}
