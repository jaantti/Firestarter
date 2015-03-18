while [ : ]
do
	sudo ./usbreset /dev/bus/usb/001/001
	sudo ./usbreset /dev/bus/usb/003/001
	sudo ./usbreset /dev/bus/usb/002/001
	sleep 1
	python Client.py
done
