#!/bin/sh

FILES="wordle.c"


build() {
    local target="$1"
    local suffix="$2"
    local prefix
    local bin="bin/$target"
    if [ "$target" = `uname -m` ]
    then
        prefix="/usr"
        cc="gcc"
    else
        prefix="/usr/$target"
        cc="$target-gcc"
    fi

    mkdir -p "$bin"

    "$cc" -s -static -DNDEBUG -O3 -lz $FILES "$prefix/lib/libz.a" -o "$bin/wordle"
    cp "bin/words.db.gz" "$bin/words.db.gz"
    tar -C "$bin" --zstd --numeric-owner -cf "bin/wordle-$target.tar.zst" "wordle$suffix" "words.db.gz"
}
rm -rf "bin"
mkdir -p "bin"


tr --delete '\n' < "words.txt" > "bin/words.db"
gzip -c "bin/words.db" > "bin/words.db.gz"


# (x86_64-w64-mingw32-gcc -s -static -DNDEBUG -O3 -lz wordle.c /usr/x86_64-w64-mingw32/lib/libz.a -o bin/wordle.exe && tar --zst --numeric-owner -cf bin/wordle-windows-x86_64.tar.zst bin/wordle.exe bin/words.db.gz) &
# (gcc -s -static -DNDEBUG -O3 -lz wordle.c /usr/lib/libz.a -o bin/wordle && tar --zst --numeric-owner -cf bin/wordle-linux-x86_64.tar.zst bin/wordle bin/words.db.gz) &

build "x86_64" &
build "x86_64-w64-mingw32" ".exe" &
wait $(jobs -rp)


