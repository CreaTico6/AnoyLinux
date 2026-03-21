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

STEP 2: ONE COMMAND INSTALL (RECOMMENDED)
════════════════════════════════════════════════════════════════════

    cd AnoyLinux
    ./install_and_start.sh

This will:
    ✓ Compile and install everything
    ✓ Create ~/.anoypc/ directory
    ✓ Enable all 9 features
    ✓ Setup cron job (every 6 minutes)
    ✓ Start one prank immediately

STEP 3: MANUAL INSTALL (ALTERNATIVE)
════════════════════════════════════════════════════════════════════

    cd AnoyLinux
    make
    ./setup.sh

This will:
    ✓ Compile the program
    ✓ Create ~/.anoypc/ directory
    ✓ Enable all 9 features
    ✓ Setup cron job (every 6 minutes)
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
