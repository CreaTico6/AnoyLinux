##
##	AnoyPC
##
##	by: creatico6@gmail.com
##
##					march 2026
##


#!/bin/bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SETUP_SCRIPT="$SCRIPT_DIR/setup.sh"
ANOYPC_DIR="$HOME/.anoypc"

print_usage() {
	cat << 'EOF'
Usage: ./install.sh

What it does:
	1) Runs setup.sh (compile + install + cron configuration)
	2) Ensures cron is enabled via ~/.anoypc/anoyon.sh
	3) Does NOT trigger any prank immediately
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

echo "[1/3] Running setup..."
echo "[3/3] No prank triggered."
echo
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


echo "[3/3] No prank triggered."

## Remove a pasta do programa após instalar, mantendo ~/.anoypc
#if [ -d "$SCRIPT_DIR" ] && [ "$SCRIPT_DIR" != "$ANOYPC_DIR" ]; then
#	echo "[CLEANUP] Removing source directory: $SCRIPT_DIR"
#	cd ~
#	rm -rf "$SCRIPT_DIR"
#	echo "[CLEANUP] Source directory removed. Only ~/.anoypc remains."
#fi


echo
echo "Done. AnoyPC is installed and running."
echo "- Scheduler: every 30 minutes"
echo "- Log file: $ANOYPC_DIR/anoypc.log"
echo "- Test menu: $ANOYPC_DIR/test.sh"
echo "- Feature menu: $ANOYPC_DIR/features.sh"