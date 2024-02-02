import json
import argparse

parser = argparse.ArgumentParser(
    description='Convert from a JSON grammar file to a C Grammar struct'
)
parser.add_argument(
    'file_path', 
    metavar='path_to_json_grammar', 
    type=str, 
    help='Path to the JSON grammar file'
)
parser.add_argument(
    '--debug', 
    action='store_true', 
    help='Output C Grammar struct with strings instead of 8-bit keys'
)

args = parser.parse_args()

# True: outputs a C representation of the grammar with strings instead of keys.
# False (default): outputs a C representation of the grammar using 8-bit keys.
if args.debug:
    TO_STRING = True
else:
    TO_STRING = False

FILEPATH = args.file_path
with open(FILEPATH, "r") as f:
    grammar = json.load(f)

def create_lookup_table(grammar) -> dict:
    # Order the tokens in the grammar
    ordered_nonterminals = []
    ordered_terminals = []
    for nonterminal in grammar:
        if nonterminal not in ordered_nonterminals:
            ordered_nonterminals.append(nonterminal)
        
        for rule in grammar[nonterminal]:
            for token in rule:
                if is_nonterminal(token):
                    ordered_nonterminals.append(token)
                else:
                    ordered_terminals.append(token)
    
    # Assign an 8-bit key to each token in order
    # i.e.  non-terminal 1 will be assigned 0x80
    #       non-terminal 2 will be assigned 0x81
    #       terminal 1 will be assigned 0x00
    #       terminal 2 will be assigned 0x01
    lookup = {}
    key_nt = 0x80
    key_t = 0x00
    for non_terminal in ordered_nonterminals:
        lookup[non_terminal] = key_nt
        key_nt += 0x01
    for terminal in ordered_terminals:
        lookup[terminal] = key_t
        key_t += 0x01
    
    with open("./data/grammar_lookup.txt", "w") as f:
        for key, val in lookup.items():
            f.write(f"{key}: {hex(val)}\n")
    
    return lookup, ordered_nonterminals, ordered_terminals

def is_nonterminal(token: str) -> bool:
    '''
    In the sample grammars provided, non-terminals are identified by their
    enclosure in a pair of angle brackets e.g. <start>.

    Edit this code depending on how you identify non-terminals in your own
    grammars.
    '''
    return (token[0], token[-1]) == ('<', '>')

def export_grammar(grammar: dict, to_string: bool) -> None:
    lookup, ordered_nt, ordered_t = create_lookup_table(grammar)
    if to_string:
        f = open("./data/grammar_c_str.txt", "w")
    else:
        f = open("./data/grammar_c_8bit.txt", "w")

    # Main Grammar {}
    f.write("Grammar GRAMMAR = {\n")
    
    # Number of NonTerminals
    f.write("\t")
    f.write(str(len(grammar)) + ",\n")
    
    # Array of NonTerminals
    f.write("\t")
    f.write("{\n")

    for i, nonterminal in enumerate(ordered_nt):
        # NonTerminal {}
        f.write("\t\t")
        f.write("{\n")

        # Name of NonTerminal
        f.write("\t\t\t")
        if to_string:
            f.write(f'"{nonterminal}",\n') 
        else:
            f.write(f'// {nonterminal}\n')
            f.write("\t\t\t")
            f.write(f'{hex(lookup[nonterminal])},\n')

        # Number of rules NonTerminal has
        f.write("\t\t\t")
        f.write(str(len(grammar[nonterminal])) + ",\n")

        # Array of Rules for each NonTerminal {}
        f.write("\t\t\t")
        f.write("{\n") # array of rules

        for j in range(len(grammar[nonterminal])):
            rule = grammar[nonterminal][j]

            # Rule {}
            f.write("\t\t\t\t")
            f.write("{\n")

            if not to_string:
                f.write("\t\t\t\t\t")
                f.write("// ")
                for k in range(len(rule)):
                    token = rule[k]
                    f.write(f'{token}')
                    if k != len(rule) - 1:
                        f.write(", ")
                    else:
                        f.write("\n")

            # Number of tokens in rule
            f.write("\t\t\t\t\t")
            f.write(str(len(rule)) + ",\n")

            # Token {}
            f.write("\t\t\t\t\t")
            f.write("{")
            
            for k in range(len(rule)):
                token = rule[k]
                
                if to_string:
                    f.write(f'"{token}"')
                else:
                    f.write(f'{hex(lookup[token])}')
                
                if k != len(rule) - 1:
                    f.write(", ")

            # Token {}                    
            f.write("}\n")

            # Rule {}
            if j == len(grammar[nonterminal]) - 1:
                f.write("\t\t\t\t")
                f.write("}\n")
            else:
                f.write("\t\t\t\t")
                f.write("},\n")
        
        # Array of Rules for each NonTerminal {}
        f.write("\t\t\t")
        f.write("}\n")

        # NonTerminal {}
        if i == len(grammar) - 1:
            f.write("\t\t")
            f.write("}\n")
        else:
            f.write("\t\t")
            f.write("},\n")
    
    # Array of NonTerminals {}
    f.write("\t")
    f.write("}\n")

    # Grammar {}
    f.write("};")    

    f.close()

export_grammar(grammar, to_string=TO_STRING)