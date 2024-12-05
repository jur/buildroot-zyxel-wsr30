#!/bin/bash
sudo apt-get update || exit $?
sudo apt-get install -y gcc-mips-linux-gnu sispmctl picocom make || exit $?
