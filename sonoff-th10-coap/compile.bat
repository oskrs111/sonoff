@echo off

@set comport=COM4
@set curdir=%cd%
@set elf_out=app.out
@set out_dir=build
@set irom0text_addr="0x40000"
@set devkit_dir=C:\Espressif
@set mingw_dir=C:\MinGW

rem SPI_SPEED = 40, 26, 20, 80
@set SPI_SPEED="40"

rem SPI_MODE: qio, qout, dio, dout
@set SPI_MODE="qio"

rem SPI_SIZE: 512K, 256K, 1M, 2M, 4M
@set SPI_SIZE="512K"

PATH=%devkit_dir%\xtensa-lx106-elf\bin;%mingw_dir%\bin;%mingw_dir%\msys\1.0\bin;%PATH%

if exist %mingw_dir%\bin\mingw32-make.exe (
  echo Clean project directory...
  make -f Makefile clean
  echo Build project...
  make -f Makefile
) else (
    echo ERROR! MinGW not found
)
