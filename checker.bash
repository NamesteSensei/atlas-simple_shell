#!/bin/bash

# Set the necessary variables
SHELL_BINARY="$1"
TEST_SCRIPT="$2"

# Check if both arguments are provided
if [ -z "$SHELL_BINARY" ] || [ -z "$TEST_SCRIPT" ]; then
    echo "Usage: $0 <shell_binary> <test_script>"
    exit 1
fi

# Check if the shell binary exists
if [ ! -x "$SHELL_BINARY" ]; then
    echo "Error: Shell binary '$SHELL_BINARY' not found or not executable"
    exit 1
fi

# Check if the test script exists
if [ ! -f "$TEST_SCRIPT" ]; then
    echo "Error: Test script '$TEST_SCRIPT' not found"
    exit 1
fi

# Run the shell binary with the test script using valgrind and ltrace
echo "Running valgrind..."
valgrind --leak-check=full --track-origins=yes --error-exitcode=1 ./"$SHELL_BINARY" < "$TEST_SCRIPT" > output.txt 2> valgrind.log
VALGRIND_EXIT_CODE=$?

echo "Running ltrace..."
if command -v ltrace &> /dev/null; then
    ltrace ./"$SHELL_BINARY" < "$TEST_SCRIPT" > ltrace.log 2>&1
else
    echo "ltrace command not found. Skipping ltrace step."
    LTRACE_EXIT_CODE=0
fi

# Check valgrind output for memory errors
if [ $VALGRIND_EXIT_CODE -ne 0 ]; then
    echo "Memory errors detected by valgrind:"
    cat valgrind.log
else
    echo "No memory errors detected by valgrind."
fi

# Display ltrace output if available
if [ -f ltrace.log ]; then
    echo "ltrace output:"
    cat ltrace.log
fi

# Expected output for /bin/ls /var
EXPECTED_OUTPUT=$(ls /var)

# Filter the actual output to remove extra prompt lines
ACTUAL_OUTPUT=$(grep -v '#cisfun$' output.txt)

if [ "$EXPECTED_OUTPUT" = "$ACTUAL_OUTPUT" ]; then
    echo "Output is correct."
else
    echo "Output is incorrect."
    echo "Expected:"
    echo "$EXPECTED_OUTPUT"
    echo "Actual:"
    echo "$ACTUAL_OUTPUT"
fi

# Exit with the appropriate status
if [ $VALGRIND_EXIT_CODE -ne 0 ] || [ "$EXPECTED_OUTPUT" != "$ACTUAL_OUTPUT" ]; then
    exit 1
else
    exit 0
fi

