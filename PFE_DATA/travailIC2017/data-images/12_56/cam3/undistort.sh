EXT=tiff
for i in *.${EXT}; do
	fbname=${i%.${EXT}}
	if [[ ${fbname} =~ 201710181256-3.bin-(.+) ]]; then
		x="${BASH_REMATCH[1]}"
	fi
	undiname="undi_${fbname}.${EXT}"
	../../../calib-images/Undistorsion ${i} ../AfterCali/cam3/${undiname} ../../../calib-images/camera3.xml
	x=$((x-1))
	
	convert ../AfterCali/cam3/${undiname}  ../AfterCali/cam3/"image$x.tiff"
	rm ../AfterCali/cam3/${undiname}
done