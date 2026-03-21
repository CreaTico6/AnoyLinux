#-------------------------------#
#	AnoyPC						#
#								#
#	by: creatico6@gmail.com		#
#								#
#					march 2026	#
#-------------------------------#

# AnoyingPC Linux - Makefile
#
# Targets:
#	make			- Compile the program
#	make clean		- Remove compiled binaries and object files
#	make install	- Install to system (optional)
#	make uninstall	- Remove from system
#

# ============================================================================
# VARIABLES
# ============================================================================

# Compiler and flags
CC = cc
CFLAGS = -std=c99 -O2 -Wall -Wextra -Wpedantic # flag -02 - fica mais rápido
LDFLAGS = 

# Source and output files
PROGRAM = AnoyPC
SOURCE = AnoyPC.c
OBJECT = AnoyPC.o

# Installation paths (optional)
INSTALL_DIR = $(HOME)/.local/bin

# ============================================================================
# DEFAULT TARGET
# ============================================================================

# Default target: compile the program
all: $(PROGRAM)

# ============================================================================
# BUILD RULES
# ============================================================================

# Link object file to create executable
$(PROGRAM): $(OBJECT)
	@echo "Linking $@..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✓ Build complete: $(PROGRAM)"

# Compile source to object file
$(OBJECT): $(SOURCE)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================================================
# UTILITY TARGETS
# ============================================================================

# Clean: remove object files and executable
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(OBJECT) $(PROGRAM)
	@echo "✓ Clean complete"

# Install: copy executable to system directory (optional)
install: $(PROGRAM)
	@echo "Installing $(PROGRAM) to $(INSTALL_DIR)..."
	@if [ ! -d "$(INSTALL_DIR)" ]; then \
		echo "Creating $(INSTALL_DIR)..."; \
		mkdir -p "$(INSTALL_DIR)"; \
	fi
	@cp $(PROGRAM) $(INSTALL_DIR)/$(PROGRAM)
	@chmod 755 $(INSTALL_DIR)/$(PROGRAM)
	@echo "✓ Installation complete"

# Uninstall: remove executable from system directory (optional)
uninstall:
	@echo "Uninstalling $(PROGRAM) from $(INSTALL_DIR)..."
	@rm -f $(INSTALL_DIR)/$(PROGRAM)
	@echo "✓ Uninstall complete"

# Help: display available targets
help:
	@echo "AnoyingPC Linux - Build Help"
	@echo "============================"
	@echo "Targets:"
	@echo "  make              - Compile the program (default)"
	@echo "  make clean        - Remove compiled files"
	@echo "  make install      - Install to ~/.local/bin"
	@echo "  make uninstall    - Remove from ~/.local/bin"
	@echo "  make help         - Display this help message"
	@echo ""
	@echo "Usage:"
	@echo "  make clean && make"
	@echo "  ./setup.sh"

# Mark targets as "phony" (not actual files)
.PHONY: all clean install uninstall help

# Compiler warnings (strict)
# CFLAGS += -Wstrict-prototypes -Wshadow -Werror
