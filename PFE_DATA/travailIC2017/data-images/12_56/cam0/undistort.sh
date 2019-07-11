EXT=tiff
for i in *.${EXT}; do
	fbname=${i%.${EXT}}
	if [[ ${fbname} =~ 201710181256-0.bin-(.+) ]]; then
		x="${BASH_REMATCH[1]}"
	fi
	undiname="undi_${fbname}.${EXT}"
	../../../calib-images/Undistorsion ${i} ../AfterCali/cam0/${undiname} ../../../calib-images/camera0.xml
	x=$((x-1))
	
	convert ../AfterCali/cam0/${undiname}  ../AfterCali/cam0/"image$x.tiff"
	rm ../AfterCali/cam0/${undiname}
done