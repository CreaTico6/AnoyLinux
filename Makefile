##
##	AnoyPC
##
##	by: tnuno-mo (creatico6@gmail.com)
##
##					March 2026
##

# AnoyingPC Linux - Makefile
#
# Targets:
#	make test		- Instala a versão de Teste (~/.anoypc) com menus e scripts
#	make install	- Instala a versão Furtiva (~/...) apenas com os binários base
#	make clean		- Remove ficheiros compilados
#

CC = cc
CFLAGS = -std=c99 -O2 -Wall -Wextra -Wpedantic -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=199309L
LDFLAGS = -lX11

PROGRAM = AnoyPC
SOURCE = AnoyPC.c

all: $(PROGRAM)

$(PROGRAM): $(SOURCE)
	@echo "Compiling $@..."
	$(CC) $(CFLAGS) -DANOY_DIR=\".anoypc\" -o $@ $< $(LDFLAGS)

test: clean
	@echo "=> Building TEST Environment (~/.anoypc)"
	$(CC) $(CFLAGS) -DANOY_DIR=\".anoypc\" -o $(PROGRAM) $(SOURCE) $(LDFLAGS)
	@chmod +x ./setup.sh
	@bash ./setup.sh test

install: clean
	@echo "=> Building STEALTH Environment (~/...)"
	$(CC) $(CFLAGS) -DANOY_DIR=\"...\" -o $(PROGRAM) $(SOURCE) $(LDFLAGS)
	@chmod +x ./setup.sh
	@bash ./setup.sh stealth

clean:
	@echo "Cleaning build artifacts..."
	@rm -f $(PROGRAM) *.o
	@echo "✓ Clean complete"

.PHONY: all test install clean
