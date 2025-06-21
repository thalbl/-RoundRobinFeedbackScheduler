CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SRCDIR = src
BINDIR = bin
TARGET = $(BINDIR)/scheduler

all: $(TARGET)

$(TARGET): $(SRCDIR)/scheduler.c
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(BINDIR)

run: all
	@$(TARGET)

.PHONY: all clean run