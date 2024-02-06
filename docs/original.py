import random

class KeyNode:
    def __init__(self, token, l_str, count, rules):
        self.token = token
        self.l_str = l_str
        self.count = count
        self.rules = rules

    def __str__(self):
        return "key: %s <%d> count:%d" % (repr(self.token), self.l_str, self.count)

    def __repr__(self):
        return "key: %s <%d> count:%d" % (repr(self.token), self.l_str, self.count)

class RuleNode:
    def __init__(self, key, tail, l_str, count):
        self.key = key
        self.tail = tail
        self.l_str = l_str
        self.count = count
        assert count

    def __str__(self):
        return "head: %s tail: (%s) <%d> count:%d" % (repr(self.key.token), repr(self.tail), self.l_str, self.count)

    def __repr__(self):
        return "head: %s tail: (%s) <%d> count:%d" % (repr(self.key.token), repr(self.tail), self.l_str, self.count)

def is_nonterminal(v):
    return (v[0], v[-1]) == ('<', '>')

def key_get_def(key, grammar, l_str):
    if (key, l_str) in key_strs: return key_strs[(key, l_str)]

    if not is_nonterminal(key):
        if l_str == len(key):
            key_strs[(key, l_str)] = KeyNode(
                    token=key, l_str=l_str, count=1, rules = [])
            return key_strs[(key, l_str)]
        else:
            key_strs[(key, l_str)] = EmptyKey
            return key_strs[(key, l_str)]
    s = []
    count = 0
    rules = grammar[key]
    for rule in rules:
        s_ = rules_get_def(rule, grammar, l_str)
        count += sum([_.count for _ in s_])
        s.extend(s_)
    key_strs[(key, l_str)] = KeyNode(token=key, l_str=l_str, count=count, rules=s)
    return key_strs[(key, l_str)]

def rules_get_def(rule_, grammar, l_str):
    rule = tuple(rule_)
    if not rule: return []
    if (rule, l_str) in rule_strs: return rule_strs[(rule, l_str)]

    token, *tail = rule
    if not tail:
        s_ = key_get_def(token, grammar, l_str)
        if not s_.count: return []
        return [RuleNode(key=s_, tail=[], l_str=l_str, count=s_.count)]

    sum_rule = []
    count = 0
    for partition in range(1, l_str+1):
        h_len, t_len = partition, l_str - partition
        s_in_h = key_get_def(token, grammar, h_len)
        if not s_in_h.count: continue

        s_in_t = rules_get_def(tail, grammar, t_len)
        if not s_in_t: continue

        count_ = 0
        for r in s_in_t:
            count_ += s_in_h.count * r.count

        if not count_: continue

        count += count_
        rn = RuleNode(key=s_in_h, tail=s_in_t, l_str=partition, count=count_)
        sum_rule.append(rn)

    rule_strs[(rule, l_str)] = sum_rule
    return rule_strs[(rule, l_str)]

rule_strs = { }
key_strs = { }
EmptyKey = KeyNode(token=None, l_str=None, count=0, rules = None)

def key_extract_strings(key_node):
    # key node has a set of rules
    if not key_node.rules: return [key_node.token]
    strings = []
    for rule in key_node.rules:
        s = rule_extract_strings(rule)
        if s:
            strings.extend(s)
    return strings

def rule_extract_strings(rule_node):
    s_h = key_extract_strings(rule_node.key)
    if not rule_node.tail: return s_h

    strings = []
    for rule in rule_node.tail:
        s_t = rule_extract_strings(rule)
        for s1 in s_h:
            for s2 in s_t:
                strings.append(s1 + s2)
    return strings

def key_get_string_at(key_node, at):
    assert at < key_node.count
    if not key_node.rules: return key_node.token

    at_ = 0
    for rule in key_node.rules:
        if at < (at_ + rule.count):
            return rule_get_string_at(rule, at - at_)
        else:
            at_ += rule.count
    return None 

def rule_get_string_at(rule_node, at):
    assert at < rule_node.count
    if not rule_node.tail:
        s_k = key_get_string_at(rule_node.key, at)
        return s_k

    at_ = 0
    len_s_h = rule_node.key.count
    for rule in rule_node.tail:
        for head_idx in range(len_s_h):
            if at < (at_ + rule.count):
                s_k = key_get_string_at(rule_node.key, head_idx)
                return s_k + rule_get_string_at(rule, at - at_)
            else:
                at_ += rule.count
    return None

GRAMMAR = {
    "<start>": [["<sentence>"]],
    "<sentence>": [["<noun_phrase>", "<verb>"]],
    "<noun_phrase>": [["<article>", "<noun>"]],
    "<noun>": [["horse"], ["dog"], ["hamster"]],
    "<article>": [["a"], ["the"]],
    "<verb>": [["stands"], ["walks"], ["jumps"]]
}

for i in range(1000000):
    key_node_g = key_get_def('<start>', GRAMMAR, 11)
    at = random.randint(0, key_node_g.count - 1)
    string = key_get_string_at(key_node_g, at)
