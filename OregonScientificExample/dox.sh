#! /bin/bash

doxygen
cd latex/
make
evince refman.pdf
make clean
cd ..