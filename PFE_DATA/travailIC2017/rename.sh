j=0
for f in *.jpg; do 

[[ $f =~ IMG_20180116_(.+).jpg ]]
x="${BASH_REMATCH[1]}"
#y=$((x-6145))
z="image$j.jpg"
mv "$f" "$z"
convert "$z" "image$j.tiff"
j=$((j+1))
done
