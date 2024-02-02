<br />
<div align="center">
    <img src="img/logo.png" alt="Logo" width="125" height="125">
    <h3 align="center"><code>gfuzztools</code></h3>
    <p align="center">
        Tools for working with grammars written in C.
        <br />
        <!--<a href="">View User Guide</a>
        ·
        <a href="">View Developer Guide</a>
        ·
        <a href="">About Us</a>-->
    </p>
</div>

`gfuzztools` aims to provide users with the foundational structures and functions to work with a BNF grammar in the C programming language. It is the low-level implementation of a [Python library](https://rahul.gopinath.org/post/2021/07/27/random-sampling-from-context-free-grammar/#populating-the-linked-data-structure) written by [Dr Rahul Gopinath](https://www.sydney.edu.au/engineering/about/our-people/academic-staff/rahul-gopinath.html).

### Key features
- Optimised grammar representation: define grammars in C using an 8-bit representation instead of strings, avoiding slow string operations.
- JSON Converter: convert your existing JSON representation into our C representation effortlessly.
- Fuzzing functions: generate strings from the defined grammar for testing and validation.
- String extraction: extract all strings from a grammar or retrieve a specific string at a given index.
- Speed: preliminary testing shows a 20x speedup in the C implementation compared to the Python implementation.
- [Future expansion] Ongoing work to include functionality for randomly sampling strings from grammars.

<br>

![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
![Python](https://img.shields.io/badge/python-3670A0?style=for-the-badge&logo=python&logoColor=ffdd54)

## Table of Contents

- [Getting started](#getting-started)
- [Structure](#structure)
    - [The 8-bit representation used in C](#the-8-bit-representation-used-in-c)
    - [Our grammar representation](#our-grammar-representation)
    - [Converting your grammar](#converting-your-grammar)
- [Using the fuzzer](#using-the-fuzzer) 
- [Sampling a string from a grammar](#sampling-a-string-from-a-grammar) 
    - [The cornerstone functions](#the-cornerstone-functions)
    - [Supplementary functions](#supplementary-functions)
- [Future work and contributions](#future-work-and-contributions) 
- [Licence](#licence)
- [Contact](#contact)


## Getting started

This program follows the C99 standard, and the project uses [`gcc`](https://gcc.gnu.org) for compilation. 

Our directory structure is as follows:

```
gfuzztools/
├── bin/            # .o files
├── data/           # Grammar src files
├── docs/           # README.md, LICENSE etc.
├── examples/       # Example usage of gfuzztools
│   └── tool_name/
├── include/        # C header files
│   ├── tool_1/
│   ├── tool_2/
│   └── # common header files
├── src/            # C source files
│   ├── tool_1/
│   ├── tool_2/
│   └── # common source files
└── .gitignore
```

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
To this end, we have developed a JSON-to-C converter which takes in a JSON grammar file as a command-line argument and outputs the C initialisation code of the grammar in the above format.

### Converting your grammar

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

## Using the fuzzer

The first and most basic tool to try in `gfuzztools` is the fuzzer. The program simply aims to generates random strings from a given grammar.

The relevant files can be found in the `./fuzzer` directories inside `src/`, `include/` and `examples/`.

The program is reliant on two inputs to run:

1. `GRAMMAR`: the grammar you want to generate strings from, and
2. `START_TOKEN`: which token you wish to start fuzzing from

Refer to [Converting your grammar](#converting-your-grammar) for more details on how to set up `GRAMMAR`. Set `START_TOKEN` to any token in the grammar (usually the first non-terminal, 0x80). Initialise your `TokenArray`, call `unify_key_inv()`, and then see the fuzzed result with `print_token_array()`.

> **Try it out!**
> 
> Execute the following commands once you have cloned the repository locally:
> `make fuzzer_example` and `./bin/fuzzer_example.o`.
>
> You should see three different 8-bit tokens printed to your terminal each time you run the program.

The fuzzer here is an implementation of [The simplest grammar fuzzer in the world](https://rahul.gopinath.org/post/2019/05/28/simplefuzzer-01/), originally written by [Dr Rahul Gopinath](https://www.sydney.edu.au/engineering/about/our-people/academic-staff/rahul-gopinath.html). 

## Sampling a string from a grammar

The primary objective of this tool is to enable the random sampling of valid strings generated from a specified grammar. The sampling algorithm unfolds in three fundamental steps:

1. Counting strings: Calculate the total number of strings of a given size that can be produced from the grammar.
2. Enumeration: Enumerate all the strings that the grammar can produce.
3. Random sampling: Randomly select a specific string from the enumeration, allowing for uniform random sampling.

### The cornerstone functions

The `key_get_def()` and `rule_get_def()` functions serve as the cornerstone for obtaining the definition of a non-terminal or terminal key in the grammar, given a specific string length. To use:
```c
Token key = ...; // specify the key
Grammar* grammar = ...; // provide the grammar
size_t l_str = ...; // specify the string length
KeyNode* definition = key_get_def(key, grammar, l_str);
```
These functions efficiently explore the grammar, recursively constructing the definition for non-terminals by analysing their rules. For terminal keys, it verifies the length match before establishing the definition. Memoization enhances performance by storing and retrieving previously computed results.

### Supplementary functions

These functions build upon the `key_get_def()` to add to the functionality of the program:

#### `get_count()`

The `get_count()` functions calculate the total count of possible strings of a given length that a key node can produce. 

```c
KeyNode* definition = key_get_def(...) // the KeyNode result from above
int count = get_count(definition);
```

It considers both the count of the key itself and recursively includes the counts from its associated rules.

#### `extract_strings()`

The `extract_strings()` functions generate a list of dynamic token arrays (DTAs), each representing a possible string produced by a key node.

```c
KeyNode* definition = key_get_def(...) // the KeyNode result from above
DynTokenArray* strings = extract_strings(definition);
```

#### `get_string_at()`

For exact string retrieval, the `get_string_at()` functions enable the extraction of a specific string at a given index. 
```c
KeyNode* definition = key_get_def(...) // the KeyNode result from above
int index = ...; // specify the index
DynTokenArray* string = get_string_at(definition, index);
```
It navigates through the grammar considering both key nodes and rule nodes to pinpoint the desired string.

> **Try it out!**
> 
> Execute the following commands once you have cloned the repository locally:
> `make sampling_strings` and `./bin/sampling_strings.o`.
>
> You should see a print-out of multiple nodes (representing phrases) containing several tokens (representing words).

These functions collectively allow us to efficiently explore and extract valid strings from a grammar.

## Future work and contributions

`gfuzztools` is a work-in-progress. There are several improvements and additions yet to come, including:
- Implementing a UAR sampling function
- Printing out stringified tokens rather than 8-bit numbers
- fix print_rule_hash_table() function

All contributions are greatly appreciated! If you have a suggestion that would make this project better, please fork the repo and create a pull request.

1. Fork the Project
2. Create your Feature Branch (git checkout -b feature/branch-FeatureName)
3. Commit your Changes (git commit -m 'Add some FeatureName')
4. Push to the Branch (git push origin feature/AmazingFeature)
5. Open a Pull Request

## Licence

Distributed under the MIT License. See `LICENSE.txt` for more information.

## Contact

This project was created as part of a summer research program at the University of Sydney. 

See below for contact details.

| Supervisor | Dr. Rahul Gopinath | rahul.gopinath@sydney.edu.au |
|------------|--------------------|------------------------------|
| Student    | Antriksh Dhand     | adha5655@uni.sydney.edu.au   |


