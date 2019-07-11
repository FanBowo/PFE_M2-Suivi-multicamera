EXT=tiff
for i in *.${EXT}; do
	fbname=${i%.${EXT}}
	if [[ ${fbname} =~ 201710181256-2.bin-(.+) ]]; then
		x="${BASH_REMATCH[1]}"
	fi
	undiname="undi_${fbname}.${EXT}"
	../../../calib-images/Undistorsion ${i} ../AfterCali/cam2/${undiname} ../../../calib-images/camera2.xml
	x=$((x-1))
	
	convert ../AfterCali/cam2/${undiname}  ../AfterCali/cam2/"image$x.tiff"
	rm ../AfterCali/cam2/${undiname}
done