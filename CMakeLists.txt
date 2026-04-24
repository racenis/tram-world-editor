# Build file for the Tramway SDK Level Editor.
#
# tram-sdk resolution (first match wins):
#   1. -DTRAM_SDK_DIR=/abs/path — explicit checkout (used by CI/CD).
#   2. ../tram-sdk next to this repo — local-dev convention.
#   3. Otherwise: cloned from https://github.com/racenis/tram-sdk at configure
#      time. Override the URL / ref with -DTRAM_SDK_GIT_REPOSITORY=... and
#      -DTRAM_SDK_GIT_TAG=... if you need a fork or a pinned commit.
#
# Fedora deps:  sudo dnf install wxGTK-devel cmake gcc-c++ mesa-libGL-devel
# Debian deps:  sudo apt install libwxgtk3.2-dev cmake g++ libgl1-mesa-dev
#
# Typical native build:
#   cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
#   cmake --build build -j
#   (cd build && ln -sf ../data && ./trameditor)   # editor expects data/ in CWD
#
# Cross-compile to Windows from Linux (MinGW):
#   cmake -S . -B build-win \
#       -DCMAKE_TOOLCHAIN_FILE=../tram-ci-cd/cmake/toolchain-mingw-w64.cmake
#   cmake --build build-win -j
# The toolchain file lives in the tram-ci-cd repo; tram-ci-cd/editor-dev/
# has a standalone Docker image that bundles both native + MinGW toolchains.

cmake_minimum_required(VERSION 3.21)  # TARGET_RUNTIME_DLLS, GIT_SUBMODULES_RECURSE
project(tram-world-editor VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------

set(TRAM_SDK_DIR "" CACHE PATH
    "Path to an existing tram-sdk checkout (empty = auto-detect or fetch)")
set(TRAM_SDK_GIT_REPOSITORY "https://github.com/racenis/tram-sdk.git"
    CACHE STRING "Upstream tram-sdk git URL (used only when TRAM_SDK_DIR is empty)")
set(TRAM_SDK_GIT_TAG "main" CACHE STRING
    "Git ref to check out from TRAM_SDK_GIT_REPOSITORY")

# `has_sdk_sources(<dir>)` — true if <dir> looks like a tram-sdk checkout.
macro(_has_sdk_sources dir out)
    if(EXISTS "${dir}/src/framework/core.cpp")
        set(${out} TRUE)
    else()
        set(${out} FALSE)
    endif()
endmacro()

if(TRAM_SDK_DIR)
    _has_sdk_sources("${TRAM_SDK_DIR}" _valid)
    if(NOT _valid)
        message(FATAL_ERROR
            "TRAM_SDK_DIR=${TRAM_SDK_DIR} does not look like a tram-sdk "
            "checkout (no src/framework/core.cpp).")
    endif()
    message(STATUS "tram-sdk: explicit TRAM_SDK_DIR=${TRAM_SDK_DIR}")
else()
    _has_sdk_sources("${CMAKE_CURRENT_SOURCE_DIR}/../tram-sdk" _sibling)
    if(_sibling)
        set(TRAM_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../tram-sdk")
        message(STATUS "tram-sdk: using sibling ${TRAM_SDK_DIR}")
    else()
        message(STATUS
            "tram-sdk: not found locally, fetching from "
            "${TRAM_SDK_GIT_REPOSITORY} (${TRAM_SDK_GIT_TAG})")
        include(FetchContent)
        # Source-only fetch: point SOURCE_SUBDIR at a dir that doesn't have a
        # CMakeLists.txt so FetchContent_MakeAvailable populates the tree but
        # skips add_subdirectory. The editor builds the SDK bits it needs via
        # src/tramsdk.cpp — we never want the SDK's own library target here.
        FetchContent_Declare(tramsdk_source
            GIT_REPOSITORY "${TRAM_SDK_GIT_REPOSITORY}"
            GIT_TAG        "${TRAM_SDK_GIT_TAG}"
            SOURCE_SUBDIR  _do_not_build_)
        FetchContent_MakeAvailable(tramsdk_source)
        set(TRAM_SDK_DIR "${tramsdk_source_SOURCE_DIR}")
    endif()
endif()

# ---------------------------------------------------------------------------
# wxWidgets
# ---------------------------------------------------------------------------
# Built from source by default so the shipped artifacts are consistent across
# hosts: a Fedora MinGW cross-build and a Windows-native MSYS2 build emit the
# same wx version and ABI, and there's no dependency on a distro packaging
# wxWidgets at the right version (Fedora's mingw64-wxWidgets-static came and
# went across releases, and the prior manual build used `_gcc_custom` DLLs
# that drifted from anything upstream).
#
# Toggle off (-DTRAMWAY_USE_SYSTEM_WXWIDGETS=ON) if you already have wx
# installed and want a faster dev-loop build — wxWidgets itself takes several
# minutes to compile.

option(TRAMWAY_USE_SYSTEM_WXWIDGETS
    "Link against the host's installed wxWidgets instead of building from source"
    OFF)
set(TRAMWAY_WXWIDGETS_GIT_TAG "v3.2.10" CACHE STRING
    "wxWidgets git tag to build when not using system wx")

# Components the editor actually uses (grep for wx class names: wxAuiManager,
# wxPropertyGrid, wxGLCanvas, wxAboutBox, wxTextCtrl, wxTreeCtrl, ...).
# The legacy Windows .mk linked html/xrc/webview — none of them appear in
# source, so they're off here to save wx build time.
set(_tramway_wx_components gl propgrid aui adv core base)

if(TRAMWAY_USE_SYSTEM_WXWIDGETS)
    find_package(wxWidgets 3.0 REQUIRED COMPONENTS ${_tramway_wx_components})
    include(${wxWidgets_USE_FILE})
    set(TRAMWAY_WX_LINK ${wxWidgets_LIBRARIES})
else()
    message(STATUS
        "wxWidgets: building ${TRAMWAY_WXWIDGETS_GIT_TAG} from source "
        "(-DTRAMWAY_USE_SYSTEM_WXWIDGETS=ON to skip and use the host's wx)")

    # All wxBUILD_* / wxUSE_* options must be set BEFORE FetchContent_MakeAvailable,
    # because MakeAvailable triggers wxWidgets's own CMakeLists which reads them.
    set(wxBUILD_SHARED      ON  CACHE BOOL "" FORCE)
    set(wxBUILD_MONOLITHIC  OFF CACHE BOOL "" FORCE)
    set(wxBUILD_SAMPLES     OFF CACHE BOOL "" FORCE)
    set(wxBUILD_TESTS       OFF CACHE BOOL "" FORCE)
    set(wxBUILD_DEMOS       OFF CACHE BOOL "" FORCE)
    set(wxBUILD_INSTALL     OFF CACHE BOOL "" FORCE)
    # Trim modules the editor doesn't use — cuts several minutes off the wx
    # build AND sidesteps wxUSE_XRC's xh_bmpcbox.cpp compile failure under
    # GCC 15 in wx 3.2.6 (still broken in some of the wx 3.2.x tags).
    set(wxUSE_WEBVIEW       OFF CACHE BOOL "" FORCE)
    set(wxUSE_STC           OFF CACHE BOOL "" FORCE)
    set(wxUSE_RICHTEXT      OFF CACHE BOOL "" FORCE)
    set(wxUSE_RIBBON        OFF CACHE BOOL "" FORCE)
    set(wxUSE_MEDIACTRL     OFF CACHE BOOL "" FORCE)
    set(wxUSE_HTML          OFF CACHE BOOL "" FORCE)
    set(wxUSE_XRC           OFF CACHE BOOL "" FORCE)
    # wxUSE_WXHTML_HELP pulls in wxUSE_HTML; force both off so chkconf.h's
    # "Built in help controller can't be compiled" guard doesn't fire.
    set(wxUSE_HELP          OFF CACHE BOOL "" FORCE)
    set(wxUSE_WXHTML_HELP   OFF CACHE BOOL "" FORCE)

    include(FetchContent)
    FetchContent_Declare(wxwidgets_source
        GIT_REPOSITORY         https://github.com/wxWidgets/wxWidgets.git
        GIT_TAG                "${TRAMWAY_WXWIDGETS_GIT_TAG}"
        GIT_SHALLOW            TRUE
        GIT_SUBMODULES_RECURSE TRUE)
    FetchContent_MakeAvailable(wxwidgets_source)

    # Modular wx builds expose targets under the wx:: namespace.
    set(TRAMWAY_WX_LINK
        wx::core wx::gl wx::propgrid wx::aui
        wx::adv wx::base)
endif()

find_package(OpenGL REQUIRED)

# ---------------------------------------------------------------------------
# Editor target
# ---------------------------------------------------------------------------

# src/init.cpp is empty on disk; excluded until it gains contents.
set(EDITOR_SOURCES
    src/main.cpp
    src/tramsdk.cpp
    src/editor/editor.cpp
    src/editor/language.cpp
    src/editor/settings.cpp
    src/editor/objects/entity.cpp
    src/editor/objects/lightgraph.cpp
    src/editor/objects/navmesh.cpp
    src/editor/objects/path.cpp
    src/editor/objects/soundgraph.cpp
    src/editor/objects/transition.cpp
    src/editor/objects/world.cpp
    src/editor/objects/worldcell.cpp
    src/editor/objects/worldcellmanager.cpp
    src/widgets/dialogs.cpp
    src/widgets/mainframe.cpp
    src/widgets/objectlist.cpp
    src/widgets/objectmenu.cpp
    src/widgets/propertypanel.cpp
    src/widgets/signaleditor.cpp
    src/widgets/viewport.cpp
    src/widgets/worldtree.cpp
)

# On Windows we embed the common-controls manifest via a generated .rc so the
# editor picks up modern Win32 theming. On Linux the .rc and manifest are ignored.
set(EDITOR_WIN32_RESOURCES)
if(WIN32)
    set(_rc "${CMAKE_CURRENT_BINARY_DIR}/trameditor.rc")
    file(WRITE "${_rc}"
        "// Generated by CMake — do not edit.\n"
        "1 24 \"${CMAKE_CURRENT_SOURCE_DIR}/tram-editor.exe.manifest\"\n")
    list(APPEND EDITOR_WIN32_RESOURCES "${_rc}")
endif()

add_executable(trameditor ${EDITOR_SOURCES} ${EDITOR_WIN32_RESOURCES})

# The installer (tram-ci-cd/installer/tram-full.iss) expects this binary as
# `tedit`/`tedit.exe`. The CMake target stays `trameditor` for clarity.
set_target_properties(trameditor PROPERTIES OUTPUT_NAME tedit)

if(WIN32)
    # WIN32_EXECUTABLE links with -mwindows so no console appears behind the GUI.
    set_target_properties(trameditor PROPERTIES WIN32_EXECUTABLE TRUE)
    # wxWidgets on MinGW expects UNICODE + __WXMSW__; wx-config already emits
    # __WXMSW__ via its cxxflags, we add the UNICODE pair.
    target_compile_definitions(trameditor PRIVATE UNICODE _UNICODE)
endif()

target_include_directories(trameditor PRIVATE
    src
    "${TRAM_SDK_DIR}/src"
    "${TRAM_SDK_DIR}/libraries"
    "${TRAM_SDK_DIR}/libraries/glad"    # viewport.cpp: #include <glad.h>
)

# ENGINE_EDITOR_MODE gates out physics/audio/scripting in the SDK unity build
# performed by src/tramsdk.cpp.
target_compile_definitions(trameditor PRIVATE ENGINE_EDITOR_MODE)

target_link_libraries(trameditor PRIVATE
    ${TRAMWAY_WX_LINK}
    OpenGL::GL
)

if(UNIX AND NOT WIN32)
    target_link_libraries(trameditor PRIVATE ${CMAKE_DL_LIBS} pthread)
endif()

if(WIN32 AND TRAMWAY_USE_SYSTEM_WXWIDGETS)
    # When linking against a statically-built system wx, pull in the Win32
    # system libs wx depends on that find_package doesn't surface.
    # (Source-built shared wx lists these as PUBLIC link deps on its targets,
    # so the fetch-and-build path handles them transitively.)
    target_link_libraries(trameditor PRIVATE comctl32 shlwapi uxtheme winmm)
endif()

# Bundle shared-lib runtime deps next to the binary so tedit runs straight
# out of the build tree AND the CI output can just `cp build/*` to ship it.
if(WIN32 AND NOT TRAMWAY_USE_SYSTEM_WXWIDGETS)
    # On Windows, $<TARGET_RUNTIME_DLLS> expands to every DLL the target
    # loads at runtime — wx, and anything wx itself pulls in.
    add_custom_command(TARGET trameditor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:trameditor>
            $<TARGET_FILE_DIR:trameditor>
        COMMAND_EXPAND_LISTS
        VERBATIM)
endif()

# On Linux, rpath to the wx build tree is set automatically for build-tree
# use, so `./build/tedit` works out of the box. For CI artifact builds we
# additionally set INSTALL_RPATH to $ORIGIN so a packaged directory of
# `tedit + libwx_*.so*` is portable.
if(UNIX AND NOT WIN32 AND NOT TRAMWAY_USE_SYSTEM_WXWIDGETS)
    set_target_properties(trameditor PROPERTIES
        INSTALL_RPATH "$ORIGIN"
        BUILD_WITH_INSTALL_RPATH FALSE)   # keep BUILD_RPATH auto for dev
endif()

# tramsdk.cpp does a unity-include of ~40 SDK files + glad.c. The generated
# glad code trips a couple of unused-function warnings that aren't actionable
# from the editor side — silence them only on that one TU.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set_source_files_properties(src/tramsdk.cpp PROPERTIES
        COMPILE_OPTIONS "-Wno-unused-function;-Wno-unused-variable")
endif()

# ---------------------------------------------------------------------------
# Convenience: stage data/ next to the binary for in-build testing
# ---------------------------------------------------------------------------
# The editor opens "data/worldcells/", "data/paths/", etc. relative to CWD.
# Opt-in because most users run it from the project root directly.

option(TRAM_EDITOR_STAGE_DATA
    "Symlink data/ into the build directory next to the binary" OFF)

if(TRAM_EDITOR_STAGE_DATA AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/data")
    add_custom_command(TARGET trameditor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${CMAKE_CURRENT_SOURCE_DIR}/data"
            "$<TARGET_FILE_DIR:trameditor>/data"
        VERBATIM)
endif()

message(STATUS "tram-world-editor:")
message(STATUS "  TRAM_SDK_DIR    = ${TRAM_SDK_DIR}")
if(TRAMWAY_USE_SYSTEM_WXWIDGETS)
    message(STATUS "  wxWidgets       = system ${wxWidgets_VERSION_STRING}")
else()
    message(STATUS "  wxWidgets       = from-source ${TRAMWAY_WXWIDGETS_GIT_TAG}")
endif()
