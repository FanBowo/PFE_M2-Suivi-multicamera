EXT=tiff
for i in *.${EXT}; do
	fbname=${i%.${EXT}}
	if [[ ${fbname} =~ 201710181318-5.bin-(.+) ]]; then
		x="${BASH_REMATCH[1]}"
	fi
	undiname="undi_${fbname}.${EXT}"
	../../../calib-images/Undistorsion ${i} ../AfterCali/cam5/${undiname} ../../../calib-images/camera5.xml
	x=$((x-1))
	
	convert ../AfterCali/cam5/${undiname}  ../AfterCali/cam5/"image$x.tiff"
	rm ../AfterCali/cam5/${undiname}
done