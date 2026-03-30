#!/bin/bash
#
# AnoyingPC Linux - Setup Script
#
# This script handles the complete installation of AnoyingPC on Linux.
#
# Usage:
#   ./setup.sh
#
# What it does:
#   1. Checks local user environment
#   2. Compiles the C program using make
#   3. Creates ~/.anoypc directory structure
#   4. Copies compiled binary and control scripts
#   5. Enables all features by default (creates marker files)
#   6. Registers a background scheduler (Stealth Mode)
#   7. Configures shell persistence hacks
#   8. Creates convenient symlinks in ~/.local/bin/
#   9. Sets up logging
#
# Author: tnuno-mo (CreaTico6)
# Date: March 2026
#

# ==============================================================================
# CONFIGURATION VARIABLES
# ==============================================================================

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
BOLD='\033[1m'
NC='\033[0m'  # No Color

# Directory variables
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ANOYPC_DIR="$HOME/.anoypc"
BIN_DIR="$HOME/.local/bin"
BINARY_NAME="AnoyPC"
BINARY_PATH="$SCRIPT_DIR/$BINARY_NAME"

# Feature names (must match enum in AnoyPC.c - exactly 20 features)
FEATURES=(
    "BELL"
    "MESSAGE"
    "BLOCK_SCREEN"
    "FLASH"
    "ALERT_SCREEN"
    "CALENDAR"
    "SYSINFO"
    "UPSIDE_DOWN"
    "CAPS_ON"
    "MOUSE_JITTER"
    "MOUSE_TELEPORT"
    "MOUSE_SWAP"
    "CHASING_BUTTON"
    "CUSTOM_CURSOR"
    "MOUSE_POOP"
    "CLICK_DISABLE"
    "TERMINAL_LOOP"
    "BRIGHTNESS_PULSE"
    "GRAYSCALE"
    "MATRIX"
)

# ==============================================================================
# UTILITY FUNCTIONS
# ==============================================================================

# Print colored output
print_header() {
    echo -e "${BLUE}${BOLD}=== $1 ===${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

print_info() {
    echo -e "${BLUE}ℹ $1${NC}"
}

# ==============================================================================
# PREREQUISITE CHECKS
# ==============================================================================

# Check local user context
check_user_context() {
    print_header "Checking User Context"
    print_success "Running as user: $(id -un)"
}

# Check for required build tools
check_build_tools() {
    print_header "Checking Build Tools"
    
    # Check for gcc
    if ! command -v gcc &> /dev/null; then
        print_error "GCC compiler not found"
        echo "Please install it:"
        echo "  Ubuntu/Debian: apt-get install build-essential"
        echo "  Fedora/RHEL: dnf groupinstall 'Development Tools'"
        echo "  Arch: pacman -S base-devel"
        exit 1
    fi
    print_success "GCC compiler found"
    
    # Check for make
    if ! command -v make &> /dev/null; then
        print_error "make utility not found"
        echo "Please install it:"
        echo "  Ubuntu/Debian: apt-get install make"
        echo "  Fedora/RHEL: dnf install make"
        echo "  Arch: pacman -S make"
        exit 1
    fi
    print_success "make utility found"
    
    # Check for cron (not strictly required, but recommended)
    if ! command -v crontab &> /dev/null; then
        print_warning "cron not found - scheduling will not work"
        print_info "Install with: apt-get install cron (or similar for your distro)"
    else
        print_success "cron scheduler found"
    fi
}

# ==============================================================================
# COMPILATION
# ==============================================================================

# Compile the C program
compile_program() {
    print_header "Compiling AnoyPC"
    
    cd "$SCRIPT_DIR"
    
    # Clean old build artifacts
    print_info "Cleaning previous builds..."
    make clean > /dev/null 2>&1 || true
    
    # Compile
    print_info "Compiling C source code..."
    if ! make; then
        print_error "Compilation failed"
        exit 1
    fi
    
    # Verify binary was created
    if [ ! -f "$BINARY_PATH" ]; then
        print_error "Binary not found after compilation"
        exit 1
    fi
    
    print_success "Compilation complete: $BINARY_PATH"
}

# ==============================================================================
# DIRECTORY AND FILE SETUP
# ==============================================================================

# Create ~/.anoypc directory structure
create_directories() {
    print_header "Creating Directory Structure"
    
    # Create main directory
    if [ -d "$ANOYPC_DIR" ]; then
        print_info "Directory $ANOYPC_DIR already exists"
    else
        mkdir -p "$ANOYPC_DIR"
        print_success "Created $ANOYPC_DIR"
    fi
    
    # Create ~/.local/bin if it doesn't exist (for convenience symlinks)
    if [ ! -d "$BIN_DIR" ]; then
        mkdir -p "$BIN_DIR"
        print_success "Created $BIN_DIR"
    fi
}

# Copy binary to ~/.anoypc
copy_binary() {
    print_header "Installing Binary"
    
    cp "$BINARY_PATH" "$ANOYPC_DIR/$BINARY_NAME"
    chmod 755 "$ANOYPC_DIR/$BINARY_NAME"
    
    print_success "Binary installed: $ANOYPC_DIR/$BINARY_NAME"
}

# Create feature marker files (all enabled by default)
enable_features() {
    print_header "Enabling Features"
    
    for feature in "${FEATURES[@]}"; do
        touch "$ANOYPC_DIR/feat_$feature.on"
        print_success "Enabled: $feature"
    done
}

# Create initial log file
create_log() {
    print_header "Setting Up Logging"
    
    touch "$ANOYPC_DIR/anoypc.log"
    chmod 644 "$ANOYPC_DIR/anoypc.log"
    
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] AnoyPC installed and activated" >> "$ANOYPC_DIR/anoypc.log"
    
    print_success "Log file: $ANOYPC_DIR/anoypc.log"
}

# ==============================================================================
# CONTROL SCRIPTS
# ==============================================================================

# Create run.sh - the main runner that gets called by cron
create_run_script() {
    print_header "Creating Control Scripts"
    
    local RUN_SCRIPT="$ANOYPC_DIR/run.sh"
    
    cat > "$RUN_SCRIPT" << 'EOF'
#!/bin/bash
# Internal runner called by cron scheduler
# Executes a random enabled feature or a specified feature

# Diagnóstico de ambiente cron
export PATH="/usr/local/bin:/usr/bin:/bin:$PATH"
export DISPLAY=":0"
echo "[ $(date)] run.sh started (UID=$UID, PATH=$PATH, DISPLAY=$DISPLAY)" >> "$HOME/.anoypc/anoypc.log" 2>&1

ANOYPC_DIR="$HOME/.anoypc"

# If feature is provided as argument, run that specific one
if [ $# -gt 0 ]; then
    "$ANOYPC_DIR/AnoyPC" "$1"
else
    "$ANOYPC_DIR/AnoyPC"
fi
EOF
    
    chmod 755 "$RUN_SCRIPT"
    print_success "Created: run.sh"
}

# Create features.sh - menu to toggle features on/off
create_features_script() {
    local FEATURES_SCRIPT="$ANOYPC_DIR/features.sh"
    
    cat > "$FEATURES_SCRIPT" << 'EOF'
#!/bin/bash
# Feature Manager - Toggle individual prank features ON or OFF

ANOYPC_DIR="$HOME/.anoypc"

# Feature names
FEATURES=(
    "BELL"
    "MESSAGE"
    "BLOCK_SCREEN"
    "FLASH"
    "ALERT_SCREEN"
    "CALENDAR"
    "SYSINFO"
    "UPSIDE_DOWN"
    "CAPS_ON"
    "MOUSE_JITTER"
    "MOUSE_TELEPORT"
    "MOUSE_SWAP"
    "CHASING_BUTTON"
    "CUSTOM_CURSOR"
    "MOUSE_POOP"
    "CLICK_DISABLE"
    "TERMINAL_LOOP"
    "BRIGHTNESS_PULSE"
    "GRAYSCALE"
    "MATRIX"
)

# Function to show feature status
show_status() {
    local feature=$1
    if [ -f "$ANOYPC_DIR/feat_$feature.on" ]; then
        echo "  [ON]"
    else
        echo "  [OFF]"
    fi
}

# Function to toggle feature
toggle_feature() {
    local feature=$1
    if [ -f "$ANOYPC_DIR/feat_$feature.on" ]; then
        rm "$ANOYPC_DIR/feat_$feature.on"
        echo "✓ Feature $feature DISABLED"
    else
        touch "$ANOYPC_DIR/feat_$feature.on"
        echo "✓ Feature $feature ENABLED"
    fi
    sleep 1
}

# Main menu loop
while true; do
    clear
    echo "========================================================="
    echo "  AnoyPC - Feature Manager"
    echo "========================================================="
    echo ""
    echo "  Toggle features on or off (all ON by default):"
    echo ""
    
    # Display each feature with status
    for i in "${!FEATURES[@]}"; do
        feature_num=$((i + 1))
        printf "  %2d. %-20s" "$feature_num" "${FEATURES[$i]}"
        show_status "${FEATURES[$i]}"
    done
    
    echo ""
    echo "  $((${#FEATURES[@]} + 1)). Enable ALL features"
    echo "  $((${#FEATURES[@]} + 2)). Disable ALL features"
    echo "  $((${#FEATURES[@]} + 3)). Exit"
    echo ""
    read -p "Choose (1-$((${#FEATURES[@]} + 3))): " choice
    
    # Process choice dynamically based on length
    if [[ "$choice" =~ ^[0-9]+$ ]] && (( choice >= 1 && choice <= ${#FEATURES[@]} )); then
        idx=$((choice-1))
        toggle_feature "${FEATURES[$idx]}"
    elif [ "$choice" == "$((${#FEATURES[@]} + 1))" ]; then
        echo "Enabling all features..."
        for feature in "${FEATURES[@]}"; do
            touch "$ANOYPC_DIR/feat_$feature.on"
        done
        echo "✓ All features enabled"
        sleep 1
    elif [ "$choice" == "$((${#FEATURES[@]} + 2))" ]; then
        echo "Disabling all features..."
        for feature in "${FEATURES[@]}"; do
            rm -f "$ANOYPC_DIR/feat_$feature.on"
        done
        echo "✓ All features disabled"
        sleep 1
    elif [ "$choice" == "$((${#FEATURES[@]} + 3))" ] || [[ "$choice" =~ ^[qQ]$ ]]; then
        exit 0
    else
        echo "Invalid choice"
        sleep 1
    fi
done
EOF
    
    chmod 755 "$FEATURES_SCRIPT"
    print_success "Created: features.sh"
}

# Create test.sh - menu to test individual features
create_test_script() {
    local TEST_SCRIPT="$ANOYPC_DIR/test.sh"
    
    cat > "$TEST_SCRIPT" << 'EOF'
#!/bin/bash
# Prank Tester - Run individual pranks for testing

ANOYPC_DIR="$HOME/.anoypc"

FEATURES=(
    "BELL"
    "MESSAGE"
    "BLOCK_SCREEN"
    "FLASH"
    "ALERT_SCREEN"
    "CALENDAR"
    "SYSINFO"
    "UPSIDE_DOWN"
    "CAPS_ON"
    "MOUSE_JITTER"
    "MOUSE_TELEPORT"
    "MOUSE_SWAP"
    "CHASING_BUTTON"
    "CUSTOM_CURSOR"
    "MOUSE_POOP"
    "CLICK_DISABLE"
    "TERMINAL_LOOP"
    "BRIGHTNESS_PULSE"
    "GRAYSCALE"
    "MATRIX"
)

# Main menu loop
while true; do
    clear
    echo "========================================================="
    echo "  AnoyPC - Prank Test Mode"
    echo "========================================================="
    echo ""
    echo "  Run a specific prank immediately for testing:"
    echo ""
    for i in "${!FEATURES[@]}"; do
        printf "  %2d. %s\n" "$((i+1))" "${FEATURES[$i]}"
    done
    echo "   q. Exit"
    echo ""

    read -p "Choose (1-${#FEATURES[@]} or q): " choice

    if [[ "$choice" =~ ^[0-9]+$ ]] && (( choice >= 1 && choice <= ${#FEATURES[@]} )); then
        idx=$((choice-1))
        echo "Running \"${FEATURES[$idx]}\" prank..."
        "$ANOYPC_DIR/AnoyPC" "${FEATURES[$idx]}"
    elif [[ "$choice" =~ ^[qQ]$ ]]; then
        exit 0
    else
        echo "Invalid choice"
        sleep 1
    fi

    echo ""
    read -p "Press Enter to continue..."
done
EOF
    
    chmod 755 "$TEST_SCRIPT"
    print_success "Created: test.sh"
}

# Create anoyon.sh - enable cron job
create_on_script() {
    local ON_SCRIPT="$ANOYPC_DIR/anoyon.sh"
    
    cat > "$ON_SCRIPT" << 'EOF'
#!/bin/bash
# Enable AnoyPC cron job

# Canonical schedule
CRON_JOB="*/26 * * * * $HOME/.anoypc/run.sh"

# Rebuild crontab removing old/duplicate AnoyPC entries, then add canonical one
TMP_CRON="$(mktemp)"
crontab -l 2>/dev/null | grep -v "/.anoypc/run.sh" > "$TMP_CRON" || true
echo "$CRON_JOB" >> "$TMP_CRON"
crontab "$TMP_CRON"
rm -f "$TMP_CRON"

echo "✓ AnoyPC background schedule activated"
echo "  System running in stealth mode."
EOF
    
    chmod 755 "$ON_SCRIPT"
    print_success "Created: anoyon.sh"
}

# Create anoyoff.sh - disable cron job
create_off_script() {
    local OFF_SCRIPT="$ANOYPC_DIR/anoyoff.sh"
    
    cat > "$OFF_SCRIPT" << 'EOF'
#!/bin/bash
# Disable AnoyPC cron job temporarily

# Remove AnoyPC cron job
crontab -l 2>/dev/null | grep -v "/.anoypc/run.sh" | crontab - 2>/dev/null
echo "✓ AnoyPC scheduler disabled"
echo "  Run anoyon.sh to re-enable"
EOF
    
    chmod 755 "$OFF_SCRIPT"
    print_success "Created: anoyoff.sh"
}

# Create anoydel.sh - complete removal
create_del_script() {
    local DEL_SCRIPT="$ANOYPC_DIR/anoydel.sh"
    
    cat > "$DEL_SCRIPT" << 'EOF'
#!/bin/bash
# Complete removal of AnoyPC

ANOYPC_DIR="$HOME/.anoypc"

echo "WARNING: This will remove AnoyPC completely!"
read -p "Are you sure? (yes/no): " confirm

if [ "$confirm" = "yes" ]; then
    # Remove cron job
    crontab -l 2>/dev/null | grep -v "/.anoypc/run.sh" | crontab - 2>/dev/null
    
    # Remove symlinks from ~/.local/bin
    rm -f "$HOME/.local/bin/anoypc-features" 2>/dev/null
    rm -f "$HOME/.local/bin/anoypc-test" 2>/dev/null
    
    # Remove persistence lines from Shell config files
    for file in "$HOME/.zshenv" "$HOME/.zshrc" "$HOME/.zprofile"; do
        if [ -f "$file" ]; then
            sed -i '/# AnoyPC Persistence/d' "$file"
            sed -i '/anoyon.sh/d' "$file"
        fi
    done

    # Remove the entire ~/.anoypc directory
    rm -rf "$ANOYPC_DIR"
    cd ~

    echo "✓ AnoyPC completely removed"
    echo "  Order has been restored"
else
    echo "Removal cancelled"
fi
EOF
    
    chmod 755 "$DEL_SCRIPT"
    print_success "Created: anoydel.sh"
}

# ==============================================================================
# CRON SCHEDULING
# ==============================================================================

# Register cron job for periodic execution
setup_cron() {
    print_header "Setting Up Stealth Schedule"
    
    local CRON_JOB="*/26 * * * * $ANOYPC_DIR/run.sh"

    # Rebuild crontab: remove old/duplicate entries and enforce canonical schedule
    local TMP_CRON
    TMP_CRON="$(mktemp)"
    crontab -l 2>/dev/null | grep -v "/.anoypc/run.sh" > "$TMP_CRON" || true
    echo "$CRON_JOB" >> "$TMP_CRON"
    echo "" >> "$TMP_CRON"
    crontab "$TMP_CRON"
    rm -f "$TMP_CRON"

    print_success "Cron job registered/updated"
    
    echo "  Schedule: Stealth Mode (Unpredictable Intervals)"
    echo "  Command: [REDACTED]"
}

# ==============================================================================
# PERSISTENCE HACK
# ==============================================================================

setup_persistence() {
    print_header "Configuring Network Persistence"
    
    # O comando que será injetado nos ficheiros de configuração
    local CMD='[[ -f ~/.anoypc/anoyon.sh ]] && ~/.anoypc/anoyon.sh > /dev/null 2>&1'
    local FILES=("$HOME/.zshenv" "$HOME/.zshrc" "$HOME/.zprofile")

    for file in "${FILES[@]}"; do
        # Cria o ficheiro se não existir
        touch "$file"
        
        # Verifica se o comando já lá está para não duplicar
        if ! grep -q "anoyon.sh" "$file"; then
            echo -e "\n# AnoyPC Persistence Hack\n$CMD" >> "$file"
            print_success "Persistence added to $file"
        else
            print_info "Persistence already present in $file"
        fi
    done
}

# ==============================================================================
# SYMLINKS FOR CONVENIENCE
# ==============================================================================

# Create convenience symlinks in ~/.local/bin
create_symlinks() {
    print_header "Creating Convenience Symlinks"
    
    # Add ~/.local/bin to PATH in bashrc if not already there
    if ! grep -q '/.local/bin' "$HOME/.bashrc" 2>/dev/null; then
        echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$HOME/.bashrc"
        print_info "Added ~/.local/bin to PATH in ~/.bashrc"
    fi
    
    # Create symlink for features menu
    ln -sf "$ANOYPC_DIR/features.sh" "$BIN_DIR/anoypc-features" 2>/dev/null
    print_success "Symlink: anoypc-features"
    
    # Create symlink for test menu
    ln -sf "$ANOYPC_DIR/test.sh" "$BIN_DIR/anoypc-test" 2>/dev/null
    print_success "Symlink: anoypc-test"
}

# ==============================================================================
# SUMMARY AND COMPLETION
# ==============================================================================

# Display installation summary
show_summary() {
    print_header "Installation Complete!"
    echo ""
    echo "AnoyPC has been successfully installed on your system."
    echo ""
    echo "📁 Installation Location:"
    echo "   $ANOYPC_DIR"
    echo ""
    echo "🎭 20 Features Enabled:"
    for feature in "${FEATURES[@]}"; do
        echo "   • $feature"
    done
    echo ""
    echo "⚙️  Control Commands:"
    echo "   • $ANOYPC_DIR/features.sh   - Toggle features on/off"
    echo "   • $ANOYPC_DIR/test.sh       - Test individual effects"
    echo "   • $ANOYPC_DIR/anoyon.sh     - Enable automation"
    echo "   • $ANOYPC_DIR/anoyoff.sh    - Disable automation temporarily"
    echo "   • $ANOYPC_DIR/anoydel.sh    - Remove everything"
    echo ""
    echo "📊 Convenience Commands (add ~/.local/bin to PATH):"
    echo "   • anoypc-features           - Toggle features"
    echo "   • anoypc-test               - Test effects"
    echo ""
    echo "📝 Logging:"
    echo "   • $ANOYPC_DIR/anoypc.log"
    echo ""
    echo "⏰ Schedule: Active (Stealth Mode)"
    echo ""
    echo "💡 Quick Start:"
    echo "   • View effects in action: ~/.anoypc/test.sh"
    echo "   • Toggle features: ~/.anoypc/features.sh"
    echo "   • View logs: tail -f $ANOYPC_DIR/anoypc.log"
    echo ""
}

# ==============================================================================
# MAIN EXECUTION
# ==============================================================================

main() {
    # Pre-flight checks
    check_user_context
    check_build_tools
    
    # Compilation
    compile_program
    
    # Installation
    create_directories
    copy_binary
    enable_features
    create_log
    
    # Create control scripts
    create_run_script
    create_features_script
    create_test_script
    create_on_script
    create_off_script
    create_del_script
    
    # Setup scheduling and persistence
    setup_cron
    setup_persistence
    create_symlinks
    
    # Summary
    show_summary
    
    echo -e "${GREEN}${BOLD}System armed! 🎉${NC}"
    echo ""
}

# Run main function
main
