#!/bin/bash

red=$(tput setaf 1)
green=$(tput bold; tput setaf 2)
blue=$(tput bold; tput setaf 4)
reset=$(tput sgr0)

function include_poglib {
    git clone https://github.com/gimploo/poglib
    mv poglib/ lib/
}

function setup_envirnoment {
    rm -rf core
    ulimit -c unlimited
}

function cleanup_envirnoment {
    ulimit -c 0
}

function compile {

    local CC="gcc"
    local FLAGS="-W -Wall -Wextra -pedantic -g -Wno-missing-braces -Wmissing-field-initializers"
    local LINKERS="-lSDL2 -lGLEW -lGLU -lGL -lm"

    local EXE_PATH="bin/pong"
    local FILE_PATH="$1"

    $CC $FILE_PATH $FLAGS $LINKERS -o $EXE_PATH

}

function gdb_debug {

    local TMP=gdb_debug_txt_file
    local EXE_PATH=$1

    if [ -f "core" ] 
    then
        echo -e "[*] ${blue}Core dump found, running with core dump ... ${reset}"
        echo q | gdb --core=core --silent "$EXE_PATH" > $TMP
    else 
        echo -e "[*] ${blue}Core Dump not found, running without core dump ... ${reset}"
        echo run | gdb --silent "$EXE_PATH" > $TMP
    fi

    echo "${red}"
        sed "5p;6p; d" $TMP
    echo "${reset}"

    rm $TMP

}

function run_profiler {

    local EXE_PATH=$1

    time $EXE_PATH
}

function main {

    local SRC_PATH="src/main.c"
    local EXE_PATH="bin/pong"
    local BIN_DIR="bin/"
    local LIB_DIR="lib/"


    # Cleaning bin directory
    if [ "$1" == "clean" ]
    then
        echo -e "[!] ${green}Cleaning bin/ directory${reset}"
        rm -rf bin/
        echo -e "[!] ${green}Removing coredumps${reset}\n"
        rm -f core
        exit 0
    fi


    # Set environment
    echo -e "[!] ${green}Setting up environment${reset}"
    setup_envirnoment

    if [ ! -d "$LIB_DIR" ]
    then
        include_poglib
    fi

    # Checking if bin directory is made
    if [ ! -d "$BIN_DIR" ] 
    then
        echo -e "[!] ${green}Creating directory ${reset}\`$BIN_DIR\`"
        mkdir bin/
    else 
        echo -e "[!] ${green}Found directory ${reset}\`$BIN_DIR\`" 
    fi


    # Compiling source files
    echo -e "[*] ${blue}Compiling source file ...${reset}\n"

    if ! compile $SRC_PATH;
    then 
        echo -e "[!] ${red}Compilation Failed ${reset}"
        exit $LINENO
    else 
        echo -e "\n[!] ${green}Compilation Successfull ${reset}"
    fi


    # Running executable
    echo -e "[*] ${blue}Running executable ...\n${reset}"
    run_profiler $EXE_PATH


    # If seg faults run it throught debugger
    if [ $? -eq 139 ]
    then
        echo -e "\n[!] ${red} Segmentation Fault Occurred ${reset}"
        echo -e "\n[*] ${blue}Running executable through debugger ...${reset}"
        gdb_debug $EXE_PATH
        echo -e "[!] ${green}Exiting debugger ${reset}"
    fi


    # Reseting environment
    echo -e "[!] ${green}Cleaning up environment ${reset}"
    cleanup_envirnoment


    echo -e ""
    exit 0
}

main "$1"
