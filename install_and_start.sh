#!/bin/bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SETUP_SCRIPT="$SCRIPT_DIR/setup.sh"
ANOYPC_DIR="$HOME/.anoypc"

print_usage() {
    cat << 'EOF'
Usage: ./install_and_start.sh

What it does:
  1) Runs setup.sh (compile + install + cron configuration)
  2) Ensures cron is enabled via ~/.anoypc/anoyon.sh
  (no pranks are triggered during setup)

Options:
  -h, --help Show this help
EOF
}

case "${1:-}" in
    -h|--help)
        print_usage
        exit 0
        ;;
    "")
        ;;
    *)
        echo "Invalid option: $1"
        echo
        print_usage
        exit 1
        ;;
esac

if [ ! -f "$SETUP_SCRIPT" ]; then
    echo "Error: setup script not found at $SETUP_SCRIPT"
    exit 1
fi

chmod +x "$SETUP_SCRIPT"

echo "[1/2] Running setup..."
"$SETUP_SCRIPT"

if [ -x "$ANOYPC_DIR/anoyon.sh" ]; then
    echo "[2/2] Ensuring scheduler is enabled..."
    "$ANOYPC_DIR/anoyon.sh" >/dev/null 2>&1 || true
fi

clear || cls
echo
echo "tnuno-mo"
echo