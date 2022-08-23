@echo off

set code=%cd%
set application_name=Microsmos
set warnings= /W4 /WX /wd4201 /wd4100 /wd4127 /wd4505 /wd4533
set compile_flags= -FC -GR- -EHa- -nologo -Zi %warnings% /I "%code%/code/"
set common_link_flags= opengl32.lib -incremental:no /LIBPATH:"%code%/code/third_party/bin"
set platform_link_flags= Shell32.lib gdi32.lib user32.lib Kernel32.lib winmm.lib %common_link_flags%
set app_link_flags= %common_link_flags%


if not exist build mkdir build
pushd build
del %application_name%_*.pdb > NUL 2> NUL
set time_stamp=%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%

start /b /wait "" "cl.exe"  %build_options% %compile_flags% ../code/game/game_main.c /LD /link %app_link_flags% /out:%application_name%_code.dll  -PDB:%application_name%_%time_stamp: =%.pdb

start /b /wait "" "cl.exe" %compile_flags% ../code/os/win32/win32_main.c /link %platform_link_flags% /out:%application_name%.exe
popd

