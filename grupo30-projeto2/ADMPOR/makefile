gcc = CC
CFLAGS = -Wall -I include -g
LIBS = -lm -g
OBJ_dir = obj
OBJECTOS = main.o client.o enterprise.o configuration.o log.o stats.o intermediary.o memory.o process.o synchronization.o aptime.o apsignal.o

admpor: $(OBJECTOS)
	$(CC) $(addprefix $(OBJ_dir)/,$(OBJECTOS)) -o bin/$@ $(LIBS)
%.o: src/%.c $($@) 
	$(CC) $(CFLAGS) -o $(OBJ_dir)/$@ -c $<
clean:
	rm $(OBJ_dir)/*.o bin/admpor