#!/bin/bash

# Function to run a single test
run_test() {
    input=$1
    key=$2
    expected=$3

    # Run the encoder with the provided key, using echo to pipe input
    output=$(echo -n "$input" | ./encoder $key)

    # Check if the output matches the expected result
    if [ "$output" == "$expected" ]; then
        echo "PASS: \"$input\" | \"$key\" -> \"$expected\""
    else
        echo "FAIL: \"$input\" | \"$key\" -> Expected: \"$expected\", Got: \"$output\""
    fi
}

# Test Cases
run_test "ABC123abc" "+E123" "BDF246bdf"
run_test "BDF246bdf" "-E123" "ABC123abc"
run_test "ABC123abc" "+E3" "DEF456def"
run_test "!@#$%^&*()_+" "+E123" "!@#$%^&*()_+"
run_test "" "+E123" ""

# Add more tests as needed
