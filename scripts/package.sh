#!/bin/bash
rm ./submission.zip
mkdir submission
cp src/*.hpp submission 2>/dev/null
cp *.hpp submission 2>/dev/null
cp src/*.cpp submission 2>/dev/null
cp *.cpp submission 2>/dev/null
cp scripts/packageRun.sh submission/run.sh
cd submission
for f in *.*pp
do
    sed -i 's/#include "src\/\(.*\?\.[ch]pp\)"/#include "\1"/' $f
done
zip submission.zip ./*
mv submission.zip ..
cd ..
rm -rf ./submission