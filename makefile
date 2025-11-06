SRCS=$(wildcard *.c)
OUT=build/main.out
CFLAGS=-Wall -std=c11 -g

$(OUT) : $(SRCS)
	gcc $(CFLAGS) $(SRCS) -o $(OUT)

run : $(OUT)
	./$(OUT)

clean:
	rm build/*.out 




