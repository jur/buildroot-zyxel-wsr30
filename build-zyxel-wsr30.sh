#!/bin/bash
BASEDIR="$(pwd)"
cd "$(dirname "${0}")" || exit $?
SCRIPTDIR="$(pwd)"
cd "${BASEDIR}" || exit $?

VERSION="2024.02.7"
PACKAGE="buildroot-${VERSION}"
ARCHIVE="${PACKAGE}.tar.gz"

SERVER="https://buildroot.org/downloads"

cd "${BASEDIR}" || exit $?
if [ ! -e "${BASEDIR}/${ARCHIVE}" ]; then
	wget -O "${BASEDIR}/${ARCHIVE}" "${SERVER}/${ARCHIVE}" || exit $?
fi
if [ ! -e "${BASEDIR}/${PACKAGE}" ]; then
	tar -xzf "${BASEDIR}/${ARCHIVE}" || exit $?
fi

cd "${BASEDIR}/${PACKAGE}" || exit $?
if [ ! -e ".config" ]; then
	cp "${SCRIPTDIR}/configs/buildroot-config-zyxel-wsr30.txt" ".config" || exit $?
	make oldconfig || exit $?
fi
make || exit $?
