j=0
for f in *.tiff; do 

[[ $f =~ 201710181658-2(.+).tiff ]]
x="${BASH_REMATCH[1]}"
#y=$((x-6145))
z="image$j.tiff"
mv "$f" "$z"
convert "$z" "image$j.tiff"
j=$((j+1))
done
