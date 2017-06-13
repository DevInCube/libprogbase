#!/usr/bin/env bash

LIB_PATH=/usr/local/lib/libprogbase.a

VERSION=$(strings ${LIB_PATH} | grep Version -m 1 | cut -d " " -f 2)
printf "libprogbase: v${VERSION}\n"
