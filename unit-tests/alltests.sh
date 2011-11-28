#!/usr/bin/env bash

# Launch all unit-tests

testbins=$(find bin/tests -type f -print)
for testbin in "$testbins" ; do
    "$testbin"
    "$testbin" -xunitxml -o unit-tests/unit-baselib.xml
done
