@echo off

::cl /c dictionary.c ini.c atom.c fmap.c registry.c
cl /c /DMEM_COPY=memcpy /DMEM_MOVE=memmove /DMEM_COMPARE=memcmp /DMEM_SET=memset jszl.c
::cl /Fotest\test.obj /Fetest\test.exe test\test.c /link dictionary.obj ini.obj atom.obj fmap.obj registry.obj
cl /Fotest\test.obj /FI"..\..\core.h" /Fe"test\test.exe" test\test.c /link /MACHINE:x86 jszl.obj
::del dict.obj ini.obj atom.obj fmap.obj test.obj registry.obj json.obj
::del dilly.obj
echo.-------------------------------------
call test\test
