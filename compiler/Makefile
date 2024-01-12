defualt:

reader:
	gcc src/reader.c -o bin/reader

test:
	gcc src/*.c -o test -D TEST

main:
	gcc src/cx86.c -o bin/main

inst_dtb:
	python3 ./database/dtb.py ./database/database.1 > ./database/instuction_dtb2.c


run:
	cat test/test_data_in | ./bin/reader | ./bin/main