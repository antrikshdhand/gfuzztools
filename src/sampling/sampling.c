#include "../../include/sampling/sampling.h"
#include "../../include/sampling/helpers.h"

KeyHashTable key_strs;
RuleHashTable rule_strs;
GrammarHashTable grammar_hash;

KeyNode* key_get_def(Token key, Grammar* grammar, size_t l_str)
{
    if (get_key(&key_strs, key, l_str) != NULL)
        return get_key(&key_strs, key, l_str);
    
    size_t nt_index;
    if ((nt_index = is_non_terminal(key)) != -1)
    {
        RuleNode* s = NULL; 
        int count = 0;
        for (size_t i = 0; i < grammar->non_terminals[nt_index].num_rules; i++) {
            RuleNode* s_ = rules_get_def(
                &grammar->non_terminals[nt_index].rules[i], 
                grammar, 
                l_str
            );

            if (s_ == NULL) 
                continue;

            for (RuleNode* r = s_; r != NULL; r = r->next) 
            {
                count += r->count;
            }

            // Append s_ to s.
            if (s == NULL)
            {
                s = s_;
            }
            else
            { 
                RuleNode* last = s;
                while (last->next != NULL)
                {
                    last = last->next;
                }
                last->next = s_;
            }
        }

        KeyNode* kn = create_key_node(key, l_str, count, s);
        insert_key(&key_strs, key, l_str, kn);
        return kn;
    }
    else
    {
        // `key` is a terminal symbol.
        if (l_str == get_grammar(&grammar_hash, key)->strlen)
        {
            KeyNode* kn = create_key_node(key, l_str, 1, NULL);
            insert_key(&key_strs, key, l_str, kn);
            return kn;
        }
        else
        {
            KeyNode* empty_key = create_key_node(EMPTY_TOKEN, 0, -1, NULL);
            return empty_key;
        }
    }
}

RuleNode* rules_get_def(Rule* rule, Grammar* grammar, size_t l_str)
{
    if (rule->num_tokens == 0) 
        return NULL;
    
    RuleNode* memoized_result;
    if ((memoized_result = get_rule(&rule_strs, rule, l_str)) != NULL)
        return memoized_result;

    // The head is the first non-empty token.
    Token head = EMPTY_TOKEN;
    size_t head_index;
    for (head_index = 0; head_index < rule->num_tokens; head_index++)
    {
        if (rule->tokens[head_index] != EMPTY_TOKEN)
        {
            head = rule->tokens[head_index];
            break;
        }
    }
    if (head == EMPTY_TOKEN) return NULL;

    // If the head is the last token in the array, then there is no tail.
    if (head_index == rule->num_tokens - 1)
    {
        KeyNode* s_ = key_get_def(head, grammar, l_str);
        if (s_->count == -1) 
            return NULL;

        RuleNode* rn = create_rule_node(s_, NULL, l_str, s_->count);
        return rn;
    }

    // Create a new rule with head_index set to EMPTY_TOKEN.
    Rule* rule_copy = malloc(sizeof(Rule));
    rule_copy->num_tokens = rule->num_tokens;
    for (size_t i = 0; i < rule->num_tokens; i++)
    {
        rule_copy->tokens[i] = rule->tokens[i];
    }
    rule_copy->tokens[head_index] = EMPTY_TOKEN;

    RuleNode* sum_rule = NULL; // List of RuleNodes
    for (size_t partition = 1; partition <= l_str; partition++)
    {
        size_t h_len = partition;
        size_t t_len = l_str - partition;

        KeyNode* s_in_h = key_get_def(head, grammar, h_len);
        if (s_in_h->count == -1) 
            continue;

        RuleNode* s_in_t = rules_get_def(rule_copy, grammar, t_len);
        if (s_in_t == NULL) 
            continue;

        int count = 0;
        for (RuleNode* r = s_in_t; r != NULL; r = r->next)
        {
            count += s_in_h->count * r->count;
        }

        if (count == 0) 
            continue;

        // Create a new RuleNode for the current partition.
        RuleNode* rn = create_rule_node(s_in_h, s_in_t, partition, count);

        // Append rn to the linked list
        if (sum_rule == NULL)
        {
            sum_rule = rn;
        }
        else
        {
            RuleNode* last = sum_rule;
            while (last->next != NULL)
            {
                last = last->next;
            }
            last->next = rn;
        }
    }

    // Memoize.
    if (sum_rule != NULL)
        insert_rule(&rule_strs, rule_copy, l_str, sum_rule);

    return sum_rule;
}

int key_get_count(KeyNode* kn)
{
    if (kn->count == 1)
        return 1;

    int s_len = 0;
    for (RuleNode* ptr = kn->rules; ptr != NULL; ptr = ptr->next)
    {
        int s = rule_get_count(ptr);
        s_len += s;
    }
    return s_len;
}

int rule_get_count(RuleNode* rn)
{
    int s_len = 0;
    int s_k = key_get_count(rn->key);
    
    if (rn->tail == NULL)
        return s_k;

    for (RuleNode* ptr = rn->tail; ptr != NULL; ptr = ptr->next)
    {
        int s_t = rule_get_count(ptr);
        s_len = s_k * s_t;
    }

    return s_len;
}

DynTokenArray* key_extract_strings(KeyNode* kn)
{
    if (kn->rules == NULL)
    {
        DynTokenArray* dta = malloc(sizeof(DynTokenArray));
        dta->length = 1;
        dta->list = malloc(dta->length * sizeof(Token));
        dta->list[0] = kn->token;
        dta->next_dta = NULL;

        return dta;
    }

    // A list of DTAs where a DTA represents one possible string.
    DynTokenArray* valid_strings = NULL;
    for (RuleNode* ptr = kn->rules; ptr != NULL; ptr = ptr->next)
    {
        DynTokenArray* s = rule_extract_strings(ptr);
        if (s == NULL)
            continue;

        append_token_arrs(&valid_strings, s); 
    } 

    return valid_strings;

}

DynTokenArray* rule_extract_strings(RuleNode* rn)
{
    DynTokenArray* s_h = key_extract_strings(rn->key);
    
    if (rn->tail == NULL)
        return s_h;

    DynTokenArray* valid_strings = NULL;
    for (RuleNode* ptr = rn->tail; ptr != NULL; ptr = ptr->next)
    {
        DynTokenArray* s_t = rule_extract_strings(ptr);
        
        DynTokenArray* s1 = s_h;
        while (s1 != NULL)
        {
            DynTokenArray* s2 = s_t;
            while (s2 != NULL)
            {
                // Concatenate s1 and s2 and append to valid_strings.
                DynTokenArray* tmp = concat_dta_lists(s1, s2);

                // Check for duplicates before appending.
                int is_duplicate = dta_lists_duplicate(valid_strings, tmp);
                if (!is_duplicate) {
                    append_token_arrs(&valid_strings, tmp);
                }

                s2 = s2->next_dta;
            }
            s1 = s1->next_dta;
        }
    } 

    return valid_strings;
}

DynTokenArray* key_get_string_at(KeyNode* kn, int at)
{
    if (at >= kn->count)
    {
        printf("`at` should be < KeyNode->count\n");
        return NULL;
    }

    if (kn->rules == NULL)
    {
        DynTokenArray* dta = malloc(sizeof(DynTokenArray));
        dta->length = 1;
        dta->list = malloc(dta->length * sizeof(Token));
        dta->list[0] = kn->token;
        dta->next_dta = NULL;
        
        return dta;
    }

    int at_ = 0;
    RuleNode* ptr = kn->rules;
    while (ptr != NULL)
    {
        if (at < (at_ + ptr->count))
        {
            return rule_get_string_at(ptr, at - at_);
        }
        else
        {
            at_ += ptr->count;
        }
        ptr = ptr->next;
    }

    return NULL;
}

DynTokenArray* rule_get_string_at(RuleNode* rn, int at)
{
    if (at >= rn->count)
    {
        printf("`at` should be < RuleNode->count\n");
        return NULL;
    }

    if (rn->tail == NULL)
        return key_get_string_at(rn->key, at);

    int at_ = 0;
    int len_s_h = rn->key->count;
    RuleNode* ptr = rn->tail;
    while (ptr != NULL)
    {
        for (int head_idx = 0; head_idx < len_s_h; head_idx++)
        {
            if (at < (at_ + ptr->count))
            {
                DynTokenArray* s_k = key_get_string_at(rn->key, head_idx);
                return concat_token_arrs(s_k, rule_get_string_at(ptr, at - at_)); 
            }
            else
            {
                at_ += ptr->count;
            }
        }
        ptr = ptr->next;
    }

    return NULL;
}

DynTokenArray* string_sample_UAR(Token key, Grammar* grammar, size_t l_str)
{

    KeyNode* kn = key_get_def(key, grammar, l_str);
    
    int at = rand() % kn->count;
    DynTokenArray* string = key_get_string_at(kn, at);

    return string;
}