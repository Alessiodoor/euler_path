# Makefile for euler_path
P = main
LIBS 	= cmdline.o
GRAPH_LIBS = `pkg-config --libs --cflags igraph`

bin: $(P) $(LIBS)

$(P): main.c euler_path.c $(LIBS) 
	gcc main.c euler_path.c $(LIBS) $(GRAPH_LIBS) -o main.exe

clean:
	@echo "Cleaning..."
	rm -rf *.o *.d $(P).exe cmdline.* tests/output/*

.PHONY: clean

ifneq "$(MAKECMDGOALS)" "clean"
-include ${SOURCES:.c=.d}
-include ${T_SOURCES:.c=.d}
endif

cmdline.c cmdline.h: $(P).ggo
	gengetopt -i $< --output-dir=./
