CFLAGS=-g -Iparser `pkg-config --cflags QtCore`
LDFLAGS=`pkg-config --libs QtCore`

qt2sexp: qt2sexp.o parser.o
	gcc $(LDFLAGS) $^ -o $@

qt2sexp.o: qt2sexp.cpp parser/*.h
	gcc $(CFLAGS) -c $< -o $@

parser.o: parser/*
	gcc $(CFLAGS) -r -nostdlib parser/*.cpp -o parser.o

clean:
	rm -f qt2sexp *.o
