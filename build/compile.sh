#!/bin/bash

echo "compiling..."
for f in ../*.c; do
	ofil="${f%.c}"
	ofil="${ofil:3}"
	ofil="${ofil}.o"
	echo "$ofil"
	gcc -std=c11 "-Ic:/msys64/usr/local/include" -O0 -g3 -Wall -c -fmessage-length=0 -o ${ofil} ${f} 
done
# 
echo "linking..."
lst=""
for f in *.o; do
	lst="${lst} $f"
done
echo "$lst"
ar -r libpolysoup.a $lst
echo "done!"