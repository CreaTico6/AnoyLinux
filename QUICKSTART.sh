#!/bin/bash
# 
# Quick Start Guide for AnoyingPC Linux
#
# This file shows the essential commands to get started
#

cat << 'EOF'

╔════════════════════════════════════════════════════════════════════╗
║                    AnoyingPC Linux - Quick Start                   ║
╚════════════════════════════════════════════════════════════════════╝

STEP 1: INSTALL BUILD TOOLS
════════════════════════════════════════════════════════════════════

Ubuntu/Debian:
    apt-get update
    apt-get install build-essential

Fedora/RHEL/CentOS:
    dnf groupinstall "Development Tools"

Arch:
    pacman -S base-devel

STEP 2: COMPILE
════════════════════════════════════════════════════════════════════

    cd AnoyLinux
    make

STEP 3: INSTALL
════════════════════════════════════════════════════════════════════

    ./setup.sh

This will:
    ✓ Compile the program
    ✓ Create ~/.anoypc/ directory
    ✓ Enable all 7 features
    ✓ Setup cron job (every 5 minutes)
    ✓ Create control scripts

STEP 4: ENJOY!
════════════════════════════════════════════════════════════════════

Test a prank:
    ~/.anoypc/test.sh

View logs:
    tail -f ~/.anoypc/anoypc.log

Toggle features:
    ~/.anoypc/features.sh

Disable temporarily:
    ~/.anoypc/anoyoff.sh

Remove completely:
    ~/.anoypc/anoydel.sh

════════════════════════════════════════════════════════════════════

Full documentation: README.md
Technical details:  STRUCTURE.md

Made with ❤️ by CreaTico6 - Linux Edition 🎭

EOF
