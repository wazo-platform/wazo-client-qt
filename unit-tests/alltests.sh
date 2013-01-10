#!/usr/bin/env bash

# Launch all unit-tests

TESTS_BIN_PATH=bin/tests
testbins=$(ls $TESTS_BIN_PATH)
for testbin in $testbins ; do
    "$TESTS_BIN_PATH/$testbin"
    "$TESTS_BIN_PATH/$testbin" -xunitxml -o "unit-tests/$testbin.xml"
done
