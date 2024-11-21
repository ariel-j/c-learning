#!/bin/bash

# Compilation
echo "Compiling encoder..."
make encoder || { echo "Compilation failed!"; exit 1; }

# Test cases
echo "Running tests..."

# 1. No encoding (default behavior)
echo "ABCDEZ" | ./encoder > output_default.txt
if diff <(echo "ABCDEZ") output_default.txt; then
    echo "Test 1 (No Encoding): PASSED"
else
    echo "Test 1 (No Encoding): FAILED"
fi

# 2. Encoding with +E12345
echo "ABCDEZ" | ./encoder +E12345 > output_encode1.txt
if diff <(echo "BDFHJA") output_encode1.txt; then
    echo "Test 2 (+E12345 Encoding): PASSED"
else
    echo "Test 2 (+E12345 Encoding): FAILED"
fi

# 3. Encoding with -E12345
echo "ABCDEZ" | ./encoder -E12345 > output_encode2.txt
if diff <(echo "ZAXEYZ") output_encode2.txt; then
    echo "Test 3 (-E12345 Encoding): PASSED"
else
    echo "Test 3 (-E12345 Encoding): FAILED"
fi

# 4. Wrap Around
echo "XYZ" | ./encoder +E3 > output_wrap.txt
if diff <(echo "ABC") output_wrap.txt; then
    echo "Test 4 (Wrap Around): PASSED"
else
    echo "Test 4 (Wrap Around): FAILED"
fi

# 5. File input and output
./encoder +E123 -i input1.txt -o output1.txt
if diff <(echo "BDFGAB") output1.txt; then
    echo "Test 5 (File I/O): PASSED"
else
    echo "Test 5 (File I/O): FAILED"
fi

# Cleanup
rm -f output_default.txt output_encode1.txt output_encode2.txt output_wrap.txt output1.txt

echo "All tests completed."
# chmod +x test_encoder.sh
#./test_encoder.sh
