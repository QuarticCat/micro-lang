# Micro Lang

A compiler of a super simple toy language written with
[qc-parser-comb](https://github.com/QuarticCat/qc-parser-comb).

It generates valid MIPS code that can be compiled by MUSL-GCC and run on QEMU.

```text
begin
  a := 1 + 1 + 1;
  b := (a + 5) + ( a + ( a + ( a + (-1) ) ) ) + ( +5 ) + ( -1 );
  c := ((((1))));
  read ( d );
  write ( a, b, c, d, -123, +255, +0, -0 );
end
```

## Requirements

- GCC >= 10.2 or Clang >= 11.1
- MUSL-MIPS(el) (download from [musl.cc](https://musl.cc/))
- QEMU-MIPS(el)

## Build & Run

```console
$ cd micro-lang
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release
$ make

$ ./micro_lang sample.m sample.S
$ mipsel-linux-musl-cc -static sample.S -o sample
$ qemu-mipsel sample
```

## License

MIT
