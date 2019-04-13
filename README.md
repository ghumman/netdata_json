# Compile
gcc -o json json.c jsmn.c
or
make

# Valgrid Test
valgrind --leak-check=yes ./json

# Valgrind Test Results on My Computer(Ubuntu 16.04)
\==21729== Memcheck, a memory error detector
\==21729== Copyright (C) 2002-2015, and GNU GPL'd, by Julian Seward et al.
\==21729== Using Valgrind-3.11.0 and LibVEX; rerun with -h for copyright info
\==21729== Command: ./json
\==21729==
e.type is JSON OBJECT
case: JSON_OBJECT, wb:  = OBJECT
JSON:  = OBJECT
case: JSON_STRING, wb: name1": "element1", "name2": "element2" = element1
JSON: name1": "element1", "name2": "element2" = element1
\==21729==
\==21729== HEAP SUMMARY:
\==21729==     in use at exit: 0 bytes in 0 blocks
\==21729==   total heap usage: 5 allocs, 5 frees, 18,048 bytes allocated
\==21729==
\==21729== All heap blocks were freed -- no leaks are possible
\==21729==
\==21729== For counts of detected and suppressed errors, rerun with: -v
\==21729== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)


# Notes
Following function/system calls have been replaced. I've kept them in the code as comments so that it can be tested in netdata project.
Original | Replaced
------------ | -------------
BUFFER | char\*
buffer_sprintf | sprintf
buffer_strcat | strcat
info | printf
buffer_free | free
mallocz | malloc
reallocz | realloc
freez | free
error | printf
unlikely | !

