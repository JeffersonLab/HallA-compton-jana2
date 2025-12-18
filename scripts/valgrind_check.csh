#!/usr/bin/env csh

# Exit on Ctrl+C
onintr cleanup

# Where to store Valgrind logs and JANA output
set LOGDIR = "build/scripts/valgrind_check"
set LOGFILE = "${LOGDIR}/valgrind_out.log"
set JANAOUTPUT = "${LOGDIR}/jana_output.txt"

# Ensure log directory exists; abort immediately if it cannot be created
if ( ! -d "$LOGDIR" ) then
    mkdir -p "$LOGDIR"
    if ( $status != 0 ) then
        echo "❌ Failed to create log directory: $LOGDIR"
        exit 1
    endif
endif

# -------------------------------
# Get ROOT_DIR from CMakeCache.txt
# -------------------------------
set ROOT_DIR = `grep '^ROOT_DIR:' build/CMakeCache.txt | cut -d= -f2`

if ( "$ROOT_DIR" == "" ) then
    echo "❌ ROOT_DIR not found in build/CMakeCache.txt"
    exit 1
endif

# ROOT_PREFIX = dirname(dirname(ROOT_DIR))
set ROOT_PARENT = `dirname "$ROOT_DIR"`
set ROOT_PREFIX = `dirname "$ROOT_PARENT"`

set ROOT_SUPP = "${ROOT_PREFIX}/etc/valgrind-root.supp"

if ( ! -f "$ROOT_SUPP" ) then
    echo "❌ ROOT Valgrind suppression file not found:"
    echo "   $ROOT_SUPP"
    exit 1
endif

# -------------------------------
# Run valgrind
# -------------------------------
valgrind \
  --leak-check=full \
  --show-leak-kinds=definite,indirect \
  --track-origins=yes \
  --num-callers=40 \
  --error-limit=no \
  --suppressions="$ROOT_SUPP" \
  --log-file="$LOGFILE" \
  ./build/compton fadcV2_64.evio > "$JANAOUTPUT"

# If Valgrind or the program failed, abort immediately
if ( $status != 0 ) then
    echo "❌ Valgrind or compton execution failed (status $status)"
    exit 1
endif

# -------------------------------
# Check for real leaks
# -------------------------------
grep -E "definitely lost: [1-9]|indirectly lost: [1-9]" "$LOGFILE" >& /dev/null
if ( $status == 0 ) then
    echo "❌ Valgrind detected memory leaks:"
    grep -E "definitely lost|indirectly lost" "$LOGFILE"
    echo "Commit aborted."
    exit 1
endif

echo "✅ Valgrind leak check passed"
exit 0

cleanup:
    exit 1
