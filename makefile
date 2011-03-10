cc  = cl
!if $(RELEASE) == y
	opt = /errorReport:none /we4715 /W2 /MT /EHsc /Ox /Fpth123_ai.pch /IPythonInclude /DBOOST_DISABLE_ASSERTS
!else
	opt = /errorReport:none /we4715 /W2 /MT /EHsc /Ox /Fpth123_ai.pch /IPythonInclude
!endif
obj = lapi.obj lauxlib.obj lbaselib.obj lcode.obj ldblib.obj ldebug.obj ldo.obj ldump.obj lfunc.obj lgc.obj linit.obj liolib.obj llex.obj lmathlib.obj lmem.obj loadlib.obj lobject.obj lopcodes.obj loslib.obj lparser.obj lstate.obj lstring.obj lstrlib.obj ltable.obj ltablib.obj ltm.obj lundump.obj lvm.obj lzio.obj my_luacode.obj script.obj network.obj keyboard.obj lib.obj address.obj scriptBase.obj scriptLua.obj version.obj main.obj th105_ai.res
exe = th123_ai

.c.obj:
        $(cc) $(opt) -c /Tp $<
.cpp.obj:
        $(cc) $(opt) -c /Tp $<
$(exe).exe: $(obj)
	version_count.exe
        $(cc) $(opt) /Fe$* $** $(lib)

next_ver.exe: next_ver.obj
        $(cc) $(opt) -e$* $** $(lib2)

stdafx.h: resource.h consts.h version.h dik_list.h keyboard.h network.h lib.h address.h scriptBase.h scriptLua.h scriptPython.h script.h main.h
	touch stdafx.h
stdafx.obj: stdafx.h
        $(cc) $(opt) /c /Yc /Tp stdafx.cpp

lapi.obj: lua_src/lapi.c
lauxlib.obj: lua_src/lauxlib.c
lbaselib.obj: lua_src/lbaselib.c
lcode.obj: lua_src/lcode.c
ldblib.obj: lua_src/ldblib.c
ldebug.obj: lua_src/ldebug.c
ldo.obj: lua_src/ldo.c
ldump.obj: lua_src/ldump.c
lfunc.obj: lua_src/lfunc.c
lgc.obj: lua_src/lgc.c
linit.obj: lua_src/linit.c
liolib.obj: lua_src/liolib.c
llex.obj: lua_src/llex.c
lmathlib.obj: lua_src/lmathlib.c
lmem.obj: lua_src/lmem.c
loadlib.obj: lua_src/loadlib.c
lobject.obj: lua_src/lobject.c
lopcodes.obj: lua_src/lopcodes.c
loslib.obj: lua_src/loslib.c
lparser.obj: lua_src/lparser.c
lstate.obj: lua_src/lstate.c
lstring.obj: lua_src/lstring.c
lstrlib.obj: lua_src/lstrlib.c
ltable.obj: lua_src/ltable.c
ltablib.obj: lua_src/ltablib.c
ltm.obj: lua_src/ltm.c
lundump.obj: lua_src/lundump.c
lvm.obj: lua_src/lvm.c
lzio.obj: lua_src/lzio.c
my_luacode.obj: lua_src/my_luacode.c

main.obj: main.cpp stdafx.obj
script.obj: script.cpp stdafx.obj
keyboard.obj: keyboard.cpp stdafx.obj
network.obj: network.cpp stdafx.obj
lib.obj: lib.cpp stdafx.obj
address.obj: address.cpp stdafx.obj
scriptLua.obj: scriptLua.cpp stdafx.obj
scriptBase.obj: scriptBase.cpp stdafx.obj
version.obj: version.cpp version.h version_impl.h
th105_ai.res: th105_ai.rc th105_ai.ico th105_ai2.ico resource.h api.bin
	rc th105_ai.rc
api.bin:  api.ai
	luac.exe -s -o api.bin api.ai
