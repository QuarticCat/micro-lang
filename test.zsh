mipselDir="${1:-$HOME/.local/mipsel-linux-musl-cross}"

buildDir="./cmake-build-test"
outputDir="$buildDir/output"
testDir="./tests"

mkdir -p $buildDir $outputDir
(cd $buildDir && cmake .. -DCMAKE_BUILD_TYPE=Release && make)
echo

for file in ./examples/*; do
    name="${$(basename "$file")%.*}"
    echo "Testing: $name"
    $buildDir/micro_lang "./examples/$name.m" "$outputDir/$name.S"
    "$mipselDir/bin/mipsel-linux-musl-cc" -static "$outputDir/$name.S" -o "$outputDir/$name"
    output="$(qemu-mipsel "$outputDir/$name" < "$testDir/$name.in")"
    if [[ $output == $(<"$testDir/$name.out") ]]; then
        echo "... Success!"
    else
        echo "... Fail!"
    fi
    echo
done
