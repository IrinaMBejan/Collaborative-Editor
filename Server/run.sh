#! /usr/bin/env bash

make clean
if make; then
    make run
fi
