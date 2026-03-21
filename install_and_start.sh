#!/bin/bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SETUP_SCRIPT="$SCRIPT_DIR/setup.sh"
ANOYPC_DIR="$HOME/.anoypc"

print_usage() {
    cat << 'EOF'
Usage: ./install_and_start.sh [--no-now]

What it does in one command:
  1) Runs setup.sh (compile + install + cron configuration)
  2) Ensures cron is enabled via ~/.anoypc/anoyon.sh
  3) Optionally triggers one prank immediately (default behavior)

Options:
  --no-now   Install and enable scheduler, but do not trigger immediate prank
  -h, --help Show this help
EOF
}

RUN_NOW=1
case "${1:-}" in
    --no-now)
        RUN_NOW=0
        ;;
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

echo "[1/3] Running setup..."
"$SETUP_SCRIPT"

if [ -x "$ANOYPC_DIR/anoyon.sh" ]; then
    echo "[2/3] Ensuring scheduler is enabled..."
    "$ANOYPC_DIR/anoyon.sh" >/dev/null 2>&1 || true
fi

if [ "$RUN_NOW" -eq 1 ] && [ -x "$ANOYPC_DIR/run.sh" ]; then
    echo "[3/3] Triggering one prank now..."
    "$ANOYPC_DIR/run.sh" || true
else
    echo "[3/3] Immediate prank skipped (--no-now)."
fi

echo
echo "Done. AnoyPC is installed and running."
echo "- Scheduler: every 6 minutes"
echo "- Log file: $ANOYPC_DIR/anoypc.log"
echo "- Test menu: $ANOYPC_DIR/test.sh"
echo "- Feature menu: $ANOYPC_DIR/features.sh"
