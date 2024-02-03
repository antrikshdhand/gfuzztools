# Developer Guide

Welcome to the `gfuzztools` Developer Guide! 

This is a concise reference to help you navigate and utilize the `gfuzztools` C library for grammars. This guide covers fundamental concepts such as the 8-bit representation, grammar structures, and conversion from JSON. It also contains brief documentation on how to use key functions for fuzzing and sampling strings. 


## Table of Contents

- [Quickstart](#quickstart)
- [Functions](#functions)
- [Structure](#structure)
    - [The 8-bit representation used in C](#the-8-bit-representation-used-in-c)
    - [Our grammar representation](#our-grammar-representation)
    - [Converting your grammar](#converting-your-grammar)
- [Sampling a string from a grammar](#sampling-a-string-from-a-grammar)
    - [The cornerstone functions](#the-cornerstone-functions)
    - [Supplementary functions](#supplementary-functions)


## Quickstart

1. Store the grammar you want to work with in a JSON file.
2. Use our [converter](#converting-your-grammar) to convert the JSON representation of the grammar into our C representation.
3. Copy and paste the output of the conversion (found in `./data`) into your C file as a `Grammar` object.
4. Run your desired function (see below for documentation).

## Functions

### `unify_key_inv()` – the basic fuzzer

 The program simply aims to generates random strings from a given grammar.  The program is reliant on two inputs to run: your `Grammar`, and the `Token` you wish to start fuzzing from.

1. Initialise your `TokenArray` to store the fuzzed strings:
```c
TokenArray fuzzed;
fuzzed.index = 0;
```

2. Run the function:
```c
Token token = ... 
Grammar grammar = ...

unify_key_inv(token, &grammar, &fuzzed)
```

3. Print the result:
```c
print_token_array(&fuzzed);
```

This is an implementation of [The simplest grammar fuzzer in the world](https://rahul.gopinath.org/post/2019/05/28/simplefuzzer-01/) in C.

> **Try it out!**
> 
> Execute the following commands once you have cloned the repository locally:
> `make fuzzer_example` and `./bin/fuzzer_example.o`.
>
> You should see three different 8-bit tokens printed to your terminal each time you run the program.


### `key_get_def()`

`key_get_def()` serves as the cornerstone for obtaining the complete definition of a non-terminal or terminal key in the grammar, given a specific string length.

You need to run this function in order to use all other functionality in `gfuzztools`. 

#### Set up hash tables

#### Usage

```c
Token token = ...; 
Grammar grammar = ...;
size_t l_str = ...; // specify the desired string length

KeyNode* definition = key_get_def(start_token, grammar, l_str);
```
For most applications, `token` is usually the first token of the grammar `0x80`.

### `key_get_count()`

Calculates the total number of possible strings of a given length that a key node can produce. 

```c
// First run the cornerstone function.
KeyNode* definition = key_get_def(...)

int count = get_count(definition);
```

It considers both the count of the key itself and recursively includes the counts from its associated rules.

### `key_extract_strings()`

Generates a list of dynamic token arrays (DTAs), each representing a possible string produced by a key node.

```c
// First run the cornerstone function.
KeyNode* definition = key_get_def(...) 

DynTokenArray* strings = extract_strings(definition);
```
You may then print the strings using `print_list_of_dtas(strings);`

> **Try it out!**
> 
> Execute the following commands once you have cloned the repository locally:
> `make sampling_strings` and `./bin/sampling_strings.o`.
>
> You should see a print-out of multiple nodes (representing phrases) containing several tokens (representing words).

### `key_get_string_at()`

Enables the extraction of a specific string at a given index. 
```c
// First run the cornerstone function.
KeyNode* definition = key_get_def(...)

int index = ...; 
DynTokenArray* string = get_string_at(definition, index);
```
It navigates through the grammar considering both key nodes and rule nodes to pinpoint the desired string.

These functions collectively allow us to efficiently explore and extract valid strings from a grammar.

## Structure

### The 8-bit representation used in C

In order to optimise the code we try to avoid the use of strings in `gfuzztools`. Instead, we map each token in the grammar to an 8-bit number and then generate random "strings" from these bytes. This mapping allows us to see some smart optimisations such as checking if a token is non-terminal or not in O(1) time.

The mapping is simple: nonterminals are assigned 8-bit keys starting from `0x80`, and terminals are assigned 8-bit keys starting from `0x00`. That is, _if the MSB of the key is set, the token is a non-terminal._ 

We also store and assign keys to all non-terminals in the order they appear in the grammar.  For example, in the simple grammar
```
<expression> ::= <term> "+" <term>
<term> ::= <factor> "*" <factor>
<factor> ::= "0" | "1" | "2" | ... | "9"
```
the first non-terminal (NT) is `<expression>` and so it would receive the key `0x80`. The second NT is `<term>` which would receive the key `0x81`, and finally `<factor>` would receive the key `0x82`. We would also store the grammar as such:
```
GRAMMAR = [<NT-struct for 0x80>, <NT-struct for 0x81>, <NT-struct for 0x82>]
```
This allows for the 4 least-significant bits of the key to also provide us with the index of the non-terminal in the grammar.

### Our grammar representation

We use structures in C to represent each part of the grammar, and use a typedef'd `uint_8` data type to represent individual tokens in the grammar.

```c
/* ./include/grammar.h */

typedef uint8_t Token;

typedef struct Rule
{
    size_t num_tokens;
    Token tokens[MAX_TOKENS_IN_RULE];
} Rule;

typedef struct NonTerminal
{
    Token name;
    size_t num_rules;
    Rule rules[MAX_RULES_FOR_NONTERMINAL];
} NonTerminal;

typedef struct Grammar
{
    size_t num_non_terminals;
    NonTerminal non_terminals[MAX_NONTERMINALS_IN_GRAMMAR];
} Grammar;
```

The use of structures naturally leads to a more complex grammar initialisation:

```c
Grammar GRAMMAR = {
	// Number of NonTerminals 
    6,

    // Array of NonTerminals
    {
        // NonTerminal 1
        {
            // Name of NonTerminal
            0x80,
    
            // Number of rules NonTerminal has
            1,

            // Array of Rules
            {
                // Rule 1
                {
                    // Number of Tokens in Rule
                    1,

                    // Array of Tokens
                    {0x81}
                }
            }
        },

        // NonTerminal 2
        {
            0x81,
            1,
            {
                {
                    2,
                    {0x82, 0x83}
                }
            }
        },
        
        ...
        
    }
};
```

### Converting your grammar

We have developed a JSON-to-C converter which takes in a JSON grammar file as a command-line argument and outputs the C initialisation code of the grammar in the above format.

Suppose you have a BNF grammar you wish to work with:

```
<start> ::= <sentence>
<sentence> ::= <noun_phrase> <verb>
<noun_phrase> ::= <article> <noun>
<noun> ::= "horse" | "dog" | "hamster"
<article> ::= "a" | "the"
<verb> ::= "stands" | "walks" | "jumps"
```

This toolkit requires that you have this grammar converted into JSON format:

```json
{
    "<start>": [["<sentence>"]],
    "<sentence>": [["<noun_phrase>", "<verb>"]],
    "<noun_phrase>": [["<article>", "<noun>"]],
    "<noun>": [["horse"], ["dog"], ["hamster"]],
    "<article>": [["a"], ["the"]],
    "<verb>": [["stands"], ["walks"], ["jumps"]]
}
```

Then, from the `./src/fuzzer/` directory, execute the Python script:
```bash
python3 ./src/converter.py <path_to_json_grammar_file>
```

Replace the contents of `GRAMMAR` in your main `.c` file with the outputted code found in `./data/grammar_c_8bit.txt`.

Notes:
- `converter.py` outputs the C initialisation code as well as a lookup table `grammar_lookup.txt` which shows you the keys for every token in the grammar.
- `converter.py` can be run with the `--debug` flag to output a debug-friendly version of the C initialisation code which uses the raw token strings rather than the 8-bit keys.
- We do not dynamically read in the JSON and convert it to C in order to optimise resources. Having the grammar stored in static memory is much faster.