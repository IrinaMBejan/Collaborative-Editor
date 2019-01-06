#! /bin/bash

make
if make; then
    make run
fi
