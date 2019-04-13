all:
	gcc -o json json.c jsmn.c

clean: 
	rm json
