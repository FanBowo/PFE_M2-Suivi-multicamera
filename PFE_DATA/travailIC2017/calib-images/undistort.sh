EXT=tiff
for i in *.${EXT}; do
	fbname=${i%.${EXT}}
	#re="201710181256-0.bin-(^[0-9]{1-3}$).tiff "
	#if [[ ${fbname} =~ 201710181256-0.bin-(.+) ]]; then
	if [[ ${fbname} =~ 201710181256-0.bin-(.+) ]]; then
		x="${BASH_REMATCH[1]}"
	fi
	#FileNum=~ ^*.bin-[0-9]{1-3}$  ${fbname}
	undiname=${x}.${EXT}
	./Undistorsion ${i} ${undiname} ./camera.xml
done
