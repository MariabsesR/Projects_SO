gcc = CC
CFLAGS = -Wall -I include -g
LIBS = -lm -g
OBJ_dir = obj
OBJECTOS = main.o client.o enterprise.o intermediary.o memory.o process.o
main.o = memory.h main.h process.h
client.o = client.h memory.h main.h
enterprise.o = memory.h main.h
intermediary.o = intermediary.h main.h
memory.o = memory.h main.h
process.o = process.h client.h intermediary.h enterprise.h main.h

admpor: $(OBJECTOS)
	$(CC) $(addprefix $(OBJ_dir)/,$(OBJECTOS)) -o bin/$@ $(LIBS)
%.o: src/%.c $($@) 
	$(CC) $(CFLAGS) -o $(OBJ_dir)/$@ -c $<
clean:
	rm –f $(OBJ_dir)/*.o bin/admpor