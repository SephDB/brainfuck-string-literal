# Compile-time esolang interpreters

This is a project to practice my TMP skills and to try out writing a user-defined literal.
It uses the string literal extension from gcc, which recent versions of clang support as well with the std=c++14 flag.

It is best to compile this with optimizations enabled on -O2 or -O3, because the compilers don't optimize much template-heavy code on lower levels.

Each .cpp is a separate compilation unit for a different esolang(currently brainfuck and partial befunge).
Befunge support is partial in that the program rewriting instructions(g and p) are not supported because of the fact this parser is compile-time only.
