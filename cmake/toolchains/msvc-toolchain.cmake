set(CMAKE_SYSTEM_NAME Windows)

set(CMAKE_C_STANDARD_LIBRARIES "kernel32.lib user32.lib gdi32.lib winspool.lib winmm.lib imm32.lib comctl32.lib version.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib dbghelp.lib wsock32.lib ws2_32.lib iphlpapi.lib rpcrt4.lib wininet.lib")
set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_C_STANDARD_LIBRARIES}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP /TC /errorReport:queue /DWIN32 /DNOMINMAX /D_CRT_SECURE_NO_WARNINGS /wd4244 /wd4100 /wd4267")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Zi /Od /Oy- /MTd /D_DEBUG /DDEBUG=1")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Ox /MT /DNDEBUG")

# 4456: declaration of 'q' hides previous local declaration
# 4244: possible loss of data (e.g. float/double)
# 4201: nonstandard extension used
# 4245: signed/unsigned mismatch
# 4100: unreferenced formal parameter
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /MP /TP /DWIN32 /DNOMINMAX /D_CRT_SECURE_NO_WARNINGS /wd4244 /wd4245 /wd4201 /wd4100 /wd4456 /wd4267")
# Visual Studio 2018 15.8 implemented conformant support for std::aligned_storage, but the conformant
# support is only enabled when the following flag is passed, to avoid
# breaking backwards compatibility with code that relied on the non-conformant behavior
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_ENABLE_EXTENDED_ALIGNED_STORAGE")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /Od /Oy- /MTd /D_DEBUG /DDEBUG=1")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox /MT /DNDEBUG")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO /STACK:5000000")
if (CMAKE_CL_64)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /machine:x64")
else()
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /machine:x86")
endif()
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")
