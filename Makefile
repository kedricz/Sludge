CC=gcc
SLUDGE = sludge

build: $(FILES)
	$(CC) -o $(SLUDGE) $(SLUDGE).c


clean:
	$(RM) $(SLUDGE)
