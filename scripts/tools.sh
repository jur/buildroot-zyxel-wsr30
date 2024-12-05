
target_power_on()
{
	sudo sispmctl -o "${POWERSOCKET}"
	RV=$?
	if [ ${RV} -ne 0 ]; then
		echo "Please connect Zyxel WSR30 MultyU AC2100 via serial line ${SERIALDEVICE} and power on and press return"
		read
	fi
}

target_power_off()
{
	sudo sispmctl -f "${POWERSOCKET}"
}

