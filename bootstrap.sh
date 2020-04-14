#!/bin/sh

CXX="g++ -std=c++17"

OUTDIR=bin
SRCDIR=src

mkdir -p ${OUTDIR}
${CXX} -o ${OUTDIR}/fabr -I${SRCDIR} ${SRCDIR}/*/*.cpp
