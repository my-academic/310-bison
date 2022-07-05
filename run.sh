yacc -d -y 1805047.y
echo 'Generated the parser C file as well the header file'
g++-11 -w -c -o y.o y.tab.c
echo 'Generated the parser object file'
flex 1805047.l
echo 'Generated the scanner C file'
g++-11 -w -c -o l.o lex.yy.c
# if the above command doesn't work try g++ -fpermissive -w -c -o l.o lex.yy.c
echo 'Generated the scanner object file'
g++-11 y.o l.o -ll -o 1805047.out
echo 'All ready, running'
./1805047.out $1
