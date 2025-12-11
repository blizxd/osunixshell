SRCS=$(wildcard *.c)
OUT=build/main
CFLAGS=-Wall -std=c11 -g

$(OUT) : $(SRCS)
	clang $(CFLAGS) $(SRCS) -o $(OUT)

run : $(OUT)
	./$(OUT)

clean:
	rm build/* 




