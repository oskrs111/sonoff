@echo off
@set comport=COM4
@set curdir=%cd%
@set devkit_dir=C:\Espressif

rem SPI_SPEED = 40, 26, 20, 80
@set SPI_SPEED="40"

rem SPI_MODE: qio, qout, dio, dout
@set SPI_MODE="qio"

rem SPI_SIZE: 512K, 256K, 1M, 2M, 4M
@set SPI_SIZE="512K"

PATH=%devkit_dir%\xtensa-lx106-elf\bin;%mingw_dir%\bin;%mingw_dir%\msys\1.0\bin;%PATH%
PATH="C:\Program Files\PuTTY";%PATH%

cd %curdir%
if exist flash.%SPI_MODE%.%SPI_SPEED%.%SPI_SIZE%.bin (
	echo Burn firmware...
	%devkit_dir%\utils\esptool-ck.exe -cp %comport% -cd ck -cb 256000 -ca 0x00000 -cf flash.%SPI_MODE%.%SPI_SPEED%.%SPI_SIZE%.bin	
) 
else (
	echo ERROR! File flash.%SPI_MODE%.%SPI_SPEED%.%SPI_SIZE%.bin 
	exit 0
)
echo Launching Serial Monitor...
putty.exe -load "COM4.115200"
