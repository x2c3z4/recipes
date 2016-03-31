all:clean eggshell vulnerable

eggshell:
	gcc -o eggshell eggshell.c
vulnerable:
	gcc -o vulnerable -g -fno-stack-protector -z execstack vulnerable.c
clean:
	rm -rf eggshell vulnerable
