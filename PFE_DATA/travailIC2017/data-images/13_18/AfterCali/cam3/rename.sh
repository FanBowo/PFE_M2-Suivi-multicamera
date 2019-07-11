j=0
for f in *.tiff; do 

[[ $f =~ temp(.+).tiff ]]
x="${BASH_REMATCH[1]}"
#y=$((x-6145))
#j=$((x+2))
z="image$x.tiff"
cp "$f" "$z"

#convert "$z" "image$j.tiff"
#j=$((j+1))
done
