CC=cc
CFLAGS=-Wall -Wextra -Wshadow -pedantic -O2 -ansi
EXEC=alarm
SRC=alarm.c

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(EXEC)

.PHONY: clean
