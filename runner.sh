#!/bin/zsh

./cmake-build-debug/aux --src $1 \
&& mv a.out ./cmake-build-debug/a.out \
&& tput setaf 2; echo "Recompiling source code and library:\n"; tput sgr0 \
&& clang++ -O2 ./cmake-build-debug/a.out src/lualib.cpp -o ./cmake-build-debug/program \
&& tput setaf 2; echo "\nSuccessfully recompiled. Running:\n"; tput sgr0 \
&& ./cmake-build-debug/program