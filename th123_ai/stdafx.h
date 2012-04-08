
#define _WIN32_WINNT 0x0501

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include <string>
#include <vector>
#include <list>

#include <winsock2.h>
// XPÇ≈Ç‡ìÆÇ≠ÇÊÇ§êßå¿Ç∑ÇÈ
#define PSAPI_VERSION 1
#include <psapi.h>
#include <tlhelp32.h>
#include <windows.h>
#include <mmsystem.h>
#include <winhttp.h>

#include <boost/version.hpp>
#include <boost/static_assert.hpp>
#include <boost/assert.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/utility.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/format.hpp>

#include <org/click3/utility.h>
#include <org/click3/dll_hack_lib.h>

#include "../common/common.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "resource.h"
#include "version.h"
#include "consts.h"
#include "dik_list.h"
#include "keybd_event.h"

#include "network.h"
#include "lib.h"
#include "utility.h"
#include "address.h"
#include "scriptBase.h"
#include "scriptLua.h"
#include "script.h"
#include "obj_base.h"
#include "obj.h"
#include "character.h"
#include "key_manager.h"
#include "main.h"
