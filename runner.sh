#!/bin/zsh
tput setaf 2; echo "Recompiling source code and library:\n"; tput sgr0 \
&& clang++ cmake-build-debug/SimpleCodeGen.ll src/lualib.cpp -o cmake-build-debug/program \
&& tput setaf 2; echo "\nSuccessfully recompiled. Running:\n"; tput sgr0 \
&& ./cmake-build-debug/program