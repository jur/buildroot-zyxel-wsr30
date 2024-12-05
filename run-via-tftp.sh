#!/bin/bash
BASEDIR="$(pwd)"
cd "$(dirname "$0")" || exit $?
SCRIPTDIR="$(pwd)"
cd "${BASEDIR}" || exit $?

CONFIGDIR="${SCRIPTDIR}"
SCRIPTDIR="${SCRIPTDIR}/scripts"

source "${CONFIGDIR}/config.sh"
source "${SCRIPTDIR}/tools.sh"

cd "${CONFIGDIR}/linuxstarter" || exit $?

KERNELBIN="vmlinux.bin"
if [ -h "vmlinux" ]; then
	rm vmlinux || exit $?
fi
ln -s "${CONFIGDIR}/buildroot-2024.02.7/output/build/linux-6.6.51/vmlinux" . || exit $?
make "${KERNELBIN}" || exit $?

LOADADDR="$(printf "0x%08x" $[ ${STARTADDR} & 0x3FFFFFFF ])"
BOOTADDR="$(printf "0x%08x" $[ ${LOADADDR} | ${KSEG} ])"
KERNELSIZE="$(stat --format="%s" "${KERNELBIN}")"
KERNELSIZE="$(printf "0x%08x" "${KERNELSIZE}")"

make LOADERADDR="${LOADERADDR}" all || exit $?

DADDR="$(printf "0x%08x\n" $(grep "linuxstarter_${LOADERADDR}.map" -e ' linux_addr$' | grep -oE "0x[A-Fa-f0-9]+"))"
DSIZE="$(printf "0x%08x\n" $(grep "linuxstarter_${LOADERADDR}.map" -e ' linux_size$' | grep -oE "0x[A-Fa-f0-9]+"))"

cat >startscript.txt <<EOF || exit $?
eth
loadaddr ${BOOTADDR}
tftp ${IP}
binary
put ${KERNELBIN}
quit
dw ${BOOTADDR} 32
EOF

export LANG=C
STARTADDRESS="$(mips-linux-gnu-objdump -f "linuxstarter_${LOADERADDR}.elf" | grep -e "start address" | grep -oe "0x[0-9a-fA-F]*")"

cat >>startscript.txt <<EOF || exit $?
loadaddr ${LOADERADDR}
tftp ${IP}
binary
put linuxstarter_${LOADERADDR}.bin
quit
ew ${DADDR} ${BOOTADDR}
ew ${DSIZE} ${KERNELSIZE}
dw ${LOADERADDR} 32
j ${STARTADDRESS:2}
EOF

target_power_on || exit $?

"${SCRIPTDIR}/sendscript.exp" "${SERIALDEVICE}" startscript.txt

target_power_off || exit $?
