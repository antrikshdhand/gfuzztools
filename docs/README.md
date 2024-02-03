<br />
<div align="center">
    <img src="img/logo.png" alt="Logo" width="125" height="125">
    <h3 align="center"><code>gfuzztools</code></h3>
    <p align="center">
        Tools for working with grammars written in C.
        <br />
        <a href="./DeveloperGuide.md">Developer Guide</a>
        ·
        <a href="https://github.com/antrikshdhand/gfuzztools/issues">Issues</a>
        ·
        <a href="https://github.com/antrikshdhand/gfuzztools/pulls">PRs</a>
    </p>
</div>


`gfuzztools` is a collection of tools designed to facilitate working with BNF grammars in the C programming language.  It is the low-level implementation of a [Python library](https://rahul.gopinath.org/post/2021/07/27/random-sampling-from-context-free-grammar/#populating-the-linked-data-structure) written by [Dr Rahul Gopinath]().

### Key Features
- **Optimized Grammar Representation:** Define grammars in C using an 8-bit representation, offering a performance boost over traditional string-based approaches.
- **JSON Converter:** Seamlessly convert existing JSON representations into our efficient C representation.
- **Fuzzing Functions:** Generate strings from defined grammars for comprehensive testing and validation.
- **String Extraction:** Extract all strings from a grammar or retrieve a specific string at a given index.
- **Speed:** Preliminary testing indicates a 20x speedup compared to the Python implementation.
- **Future Expansion:** Ongoing work includes implementing functionality for randomly sampling strings from grammars.

<br>

![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
![Python](https://img.shields.io/badge/python-3670A0?style=for-the-badge&logo=python&logoColor=ffdd54)

## Table of Contents

- [Getting started](#getting-started)
- [Usage](#usage) 
- [Roadmap](#roadmap)
- [Contributions](#contributions)
- [Licence](#licence)
- [Contact](#contact)

## Getting started

This program adheres to the C99 standard, utilizing [`gcc`](https://gcc.gnu.org) for compilation. Clone the repository and run any of the examples using:

```make [example, counts, strings, at]```

## Usage

Refer to our [Developer Guide](./DeveloperGuide.md) for an in-depth explanation of the codebase and how to utilise the `gfuzztools` library effectively.

## Roadmap 

### Motivation

Formal grammars are often overlooked in the initial stages of programming language development, resulting in a disconnect between the intended grammar and the actual code. Grammar inference, both white-box and black-box, plays a crucial role in understanding a language's grammar. State-of-the-art grammar inference algorithms include [Angluin's L* algorithm](https://people.eecs.berkeley.edu/~dawnsong/teaching/s10/papers/angluin87.pdf) and the [TTT algorithm](https://link.springer.com/chapter/10.1007/978-3-319-11164-3_26).

The original motivation of `gfuzztools` was to set up a toolchain to compare the effectiveness of grammar inference algorithms. This comparison relies on two empirical values:

- Precision: Measures how close the output of the grammar inference algorithm is to running a parser on the original grammar.
- Recall: Measures how close the output of running a generating fuzzer on the original grammar is to running a parser on the inferred grammar.

We can then combine these two values using the $\text{F1}$ score:
$$\text{F1 score} = \frac{2 \times \text{Precision} \times \text{Recall}}{\text{Precision} + \text{Recall}}$$

However, it is key that the generating fuzzer is sampling strings **uniformly at random (UAR)**, otherwise [the measure will not be accurate](https://rahul.gopinath.org/post/2021/07/27/random-sampling-from-context-free-grammar/#random-sampling). Hence, the first milestone in `gfuzztools` is to produce a UAR sampler.

### Future work

`gfuzztools` is a work-in-progress. There are several improvements and additions yet to come, including:
- Implementing the UAR sampling function
- Work in the field of grammar inference, including:
    - Implementing Angluin's L* algorithm in C

See the [open issues](https://github.com/antrikshdhand/gfuzztools/issues) for a full list of proposed features and known issues.

## Contributions

[![Contributors](https://img.shields.io/github/contributors/antrikshdhand/gfuzztools?label=Contributors&style=flat-square)](https://github.com/AY2324S1-CS2113-T18-1/tp/graphs/contributors)
[![Issues](https://img.shields.io/github/issues/antrikshdhand/gfuzztools?style=flat-square&label=Latest%20issues)](https://github.com/AY2324S1-CS2113-T18-1/tp/issues)
[![PRs](https://img.shields.io/github/issues-pr/antrikshdhand/gfuzztools?style=flat-square&label=Pull%20requests)](https://github.com/AY2324S1-CS2113-T18-1/tp/pulls)

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