#!/bin/sh

pushd ../bin

gcc ../src/main.c -std=c11 -o ash -lreadline

popd
