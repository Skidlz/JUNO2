lastdrive = z
subst L: C:\tc\lib
c:\tc\tcc -c chain.c
c:\tc\tcc -c patched.c
c:\tc\tcc -c juno2.c
c:\tc\tcc -c patchlod.c
c:\tc\tcc -c writscrn.c

c:\tc\tlink juno2 chain patched patchlod writscrn io401 bioscall C:\tc\lib\c0s,,, C:\tc\lib\emu C:\tc\lib\mathc C:\tc\lib\cs