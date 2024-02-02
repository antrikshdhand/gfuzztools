default: ; Options: fuzzer_example, sampling_counts, sampling_strings, sampling_at

# This Makefile is used to compile the scripts found in ./examples/
fuzzer_example:
	gcc examples/fuzzer/example.c src/fuzzer/fuzzer.c -o bin/fuzzer_example.o

sampling_counts:
	gcc examples/sampling/counts.c src/sampling/sampling.c src/sampling/helpers.c src/sampling/grammar_hash_table.c src/sampling/key_hash_table.c src/sampling/rule_hash_table.c src/grammar.c -o bin/sampling_counts.o

sampling_strings:
	gcc examples/sampling/strings.c src/sampling/sampling.c src/sampling/helpers.c src/sampling/grammar_hash_table.c src/sampling/key_hash_table.c src/sampling/rule_hash_table.c src/grammar.c -o bin/sampling_strings.o

sampling_at:
	gcc examples/sampling/at.c src/sampling/sampling.c src/sampling/helpers.c src/sampling/grammar_hash_table.c src/sampling/key_hash_table.c src/sampling/rule_hash_table.c src/grammar.c -o bin/sampling_at.o

clean:
	rm -rf bin/*