cmake_minimum_required(VERSION 3.0)

include(FetchContent)
include(CheckFunctionExists)

option(BKPAD_DUMP_SYMS "Builds tool for producing cross-platform .sym files" ON)
# dump_syms - Tool for producing cross-platform .sym files from a binary with debug info

option(BKPAD_MINIDUMP "Builds tool for producing stacktrace" ON)
# minidump_stackwalk - A tool for producing a stacktrace # from a minidump using debug symbol
# files produced by dump_syms

set(BREAKPAD_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${BREAKPAD_SRC_DIR}
    ${BREAKPAD_SRC_DIR}/common)

set(BREAKPADCOMMON_LIB_SRCS
    ${BREAKPAD_SRC_DIR}/common/convert_UTF.cc
    ${BREAKPAD_SRC_DIR}/common/md5.cc
    ${BREAKPAD_SRC_DIR}/common/string_conversion.cc)
set(BREAKPADCOMMON_LIB_HEADERS
    ${BREAKPAD_SRC_DIR}/common/convert_UTF.h
    ${BREAKPAD_SRC_DIR}/common/md5.h
    ${BREAKPAD_SRC_DIR}/common/path_helper.h
    ${BREAKPAD_SRC_DIR}/common/string_conversion.h)

if(WIN32)
    add_definitions(-D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
                    -DUNICODE
                    -D_UNICODE)

    list(APPEND BREAKPADCOMMON_LIB_SRCS
        ${BREAKPAD_SRC_DIR}/common/windows/guid_string.cc
        ${BREAKPAD_SRC_DIR}/common/windows/path_helper.cc
        ${BREAKPAD_SRC_DIR}/common/windows/pe_source_line_writer.cc
        ${BREAKPAD_SRC_DIR}/common/windows/pe_util.cc
        ${BREAKPAD_SRC_DIR}/common/windows/string_utils.cc)
    list(APPEND BREAKPADCOMMON_LIB_HEADERS
        ${BREAKPAD_SRC_DIR}/common/windows/guid_string.h
        ${BREAKPAD_SRC_DIR}/common/windows/pe_source_line_writer.h
        ${BREAKPAD_SRC_DIR}/common/windows/pe_util.h
        ${BREAKPAD_SRC_DIR}/common/windows/string_utils-inl.h)

    set(CLIENT_SRC_DIR ${BREAKPAD_SRC_DIR}/client/windows)

    list(APPEND BREAKPAD_LIB_SRCS
        ${CLIENT_SRC_DIR}/handler/exception_handler.cc
        ${CLIENT_SRC_DIR}/crash_generation/client_info.cc
        ${CLIENT_SRC_DIR}/crash_generation/crash_generation_client.cc
        ${CLIENT_SRC_DIR}/crash_generation/crash_generation_server.cc
        ${CLIENT_SRC_DIR}/crash_generation/minidump_generator.cc)
    list(APPEND BREAKPAD_LIB_HEADERS
        ${CLIENT_SRC_DIR}/handler/exception_handler.h
        ${CLIENT_SRC_DIR}/crash_generation/client_info.h
        ${CLIENT_SRC_DIR}/crash_generation/crash_generation_client.h
        ${CLIENT_SRC_DIR}/crash_generation/crash_generation_server.h
        ${CLIENT_SRC_DIR}/crash_generation/minidump_generator.h)
else()
    list(APPEND BREAKPADCOMMON_LIB_SRCS
        ${BREAKPAD_SRC_DIR}/common/path_helper.cc)

    set(BREAKPAD_LIB_SRCS
        ${BREAKPAD_SRC_DIR}/client/minidump_file_writer.cc)
    set(BREAKPAD_LIB_HEADERS
        ${BREAKPAD_SRC_DIR}/client/minidump_file_writer.h)

    if(APPLE)
        add_definitions(-DHAVE_MACH_O_NLIST_H)

        list(APPEND BREAKPADCOMMON_LIB_SRCS
            ${BREAKPAD_SRC_DIR}/common/mac/MachIPC.mm
            ${BREAKPAD_SRC_DIR}/common/mac/bootstrap_compat.cc
            ${BREAKPAD_SRC_DIR}/common/mac/file_id.cc
            ${BREAKPAD_SRC_DIR}/common/mac/macho_id.cc
            ${BREAKPAD_SRC_DIR}/common/mac/macho_utilities.cc
            ${BREAKPAD_SRC_DIR}/common/mac/macho_walker.cc
            ${BREAKPAD_SRC_DIR}/common/mac/string_utilities.cc)
        list(APPEND BREAKPADCOMMON_LIB_HEADERS
            ${BREAKPAD_SRC_DIR}/common/mac/MachIPC.h
            ${BREAKPAD_SRC_DIR}/common/mac/bootstrap_compat.h
            ${BREAKPAD_SRC_DIR}/common/mac/file_id.h
            ${BREAKPAD_SRC_DIR}/common/mac/macho_id.h
            ${BREAKPAD_SRC_DIR}/common/mac/macho_utilities.h
            ${BREAKPAD_SRC_DIR}/common/mac/macho_walker.h
            ${BREAKPAD_SRC_DIR}/common/mac/string_utilities.h)

        set(CLIENT_SRC_DIR ${BREAKPAD_SRC_DIR}/client/mac)

        list(APPEND BREAKPAD_LIB_SRCS
            ${CLIENT_SRC_DIR}/crash_generation/crash_generation_client.cc
            ${CLIENT_SRC_DIR}/crash_generation/crash_generation_server.cc
            ${CLIENT_SRC_DIR}/handler/breakpad_nlist_64.cc
            ${CLIENT_SRC_DIR}/handler/dynamic_images.cc
            ${CLIENT_SRC_DIR}/handler/exception_handler.cc
            ${CLIENT_SRC_DIR}/handler/minidump_generator.cc)
        list(APPEND BREAKPAD_LIB_HEADERS
            ${CLIENT_SRC_DIR}/crash_generation/crash_generation_client.h
            ${CLIENT_SRC_DIR}/crash_generation/crash_generation_server.h
            ${CLIENT_SRC_DIR}/handler/breakpad_nlist_64.h
            ${CLIENT_SRC_DIR}/handler/dynamic_images.h
            ${CLIENT_SRC_DIR}/handler/exception_handler.h
            ${CLIENT_SRC_DIR}/handler/minidump_generator.h)
    elseif(UNIX)
        add_definitions(-DHAVE_A_OUT_H)

        set(BREAKPADCOMMON_LIB_GETCONTEXT_SRCS
            ${BREAKPAD_SRC_DIR}/common/linux/breakpad_getcontext.S)
        set(BREAKPADCOMMON_LIB_GETCONTEXT_HEADERS
            ${BREAKPAD_SRC_DIR}/common/linux/breakpad_getcontext.h)

        list(APPEND BREAKPADCOMMON_LIB_SRCS
            ${BREAKPAD_SRC_DIR}/common/linux/crc32.cc
            ${BREAKPAD_SRC_DIR}/common/linux/elfutils.cc
            ${BREAKPAD_SRC_DIR}/common/linux/file_id.cc
            ${BREAKPAD_SRC_DIR}/common/linux/guid_creator.cc
            ${BREAKPAD_SRC_DIR}/common/linux/linux_libc_support.cc
            ${BREAKPAD_SRC_DIR}/common/linux/memory_mapped_file.cc
            ${BREAKPAD_SRC_DIR}/common/linux/safe_readlink.cc)

        list(APPEND BREAKPADCOMMON_LIB_HEADERS
            ${BREAKPAD_SRC_DIR}/common/linux/crc32.h
            ${BREAKPAD_SRC_DIR}/common/linux/elfutils.h
            ${BREAKPAD_SRC_DIR}/common/linux/elfutils-inl.h
            ${BREAKPAD_SRC_DIR}/common/linux/file_id.h
            ${BREAKPAD_SRC_DIR}/common/linux/guid_creator.h
            ${BREAKPAD_SRC_DIR}/common/linux/linux_libc_support.h
            ${BREAKPAD_SRC_DIR}/common/linux/memory_mapped_file.h
            ${BREAKPAD_SRC_DIR}/common/linux/safe_readlink.h)

        set(CLIENT_SRC_DIR ${BREAKPAD_SRC_DIR}/client/linux)

        list(APPEND BREAKPAD_LIB_SRCS
            ${CLIENT_SRC_DIR}/crash_generation/crash_generation_client.cc
            ${CLIENT_SRC_DIR}/dump_writer_common/thread_info.cc
            ${CLIENT_SRC_DIR}/dump_writer_common/ucontext_reader.cc
            ${CLIENT_SRC_DIR}/handler/exception_handler.cc
            ${CLIENT_SRC_DIR}/handler/minidump_descriptor.cc
            ${CLIENT_SRC_DIR}/log/log.cc
            ${CLIENT_SRC_DIR}/microdump_writer/microdump_writer.cc
            ${CLIENT_SRC_DIR}/minidump_writer/linux_dumper.cc
            ${CLIENT_SRC_DIR}/minidump_writer/linux_ptrace_dumper.cc
            ${CLIENT_SRC_DIR}/minidump_writer/minidump_writer.cc
            ${CLIENT_SRC_DIR}/minidump_writer/pe_file.cc)
        list(APPEND BREAKPAD_LIB_HEADERS
            ${CLIENT_SRC_DIR}/crash_generation/crash_generation_client.h
            ${CLIENT_SRC_DIR}/dump_writer_common/thread_info.h
            ${CLIENT_SRC_DIR}/dump_writer_common/ucontext_reader.h
            ${CLIENT_SRC_DIR}/handler/exception_handler.h
            ${CLIENT_SRC_DIR}/handler/minidump_descriptor.h
            ${CLIENT_SRC_DIR}/log/log.h
            ${CLIENT_SRC_DIR}/microdump_writer/microdump_writer.h
            ${CLIENT_SRC_DIR}/minidump_writer/linux_dumper.h
            ${CLIENT_SRC_DIR}/minidump_writer/linux_ptrace_dumper.h
            ${CLIENT_SRC_DIR}/minidump_writer/minidump_writer.h
            ${CLIENT_SRC_DIR}/minidump_writer/pe_file.h)
    endif()
endif()

macro(breakpad_set_project_properties project)
    set_target_properties(${project} PROPERTIES
        C_STANDARD 99)
    if(APPLE AND CMAKE_GENERATOR MATCHES "Xcode")
        set_target_properties(${project} PROPERTIES
            XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11"
            XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    else()
        set_target_properties(${project} PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
            LINKER_LANGUAGE CXX)
    endif()
    if(WIN32)
        target_compile_definitions(${project} PRIVATE
            _WIN32
            _CRT_SECURE_NO_WARNINGS)
    endif()
endmacro()

project(breakpad CXX)

add_library(breakpad_common STATIC ${BREAKPADCOMMON_LIB_SRCS} ${BREAKPADCOMMON_LIB_HEADERS})
breakpad_set_project_properties(breakpad_common)

if(WIN32)
    set(BREAKPADCOMMON_LIBS Imagehlp)
elseif(APPLE)
    find_library(FOUNDATION_LIBRARY Foundation REQUIRED)
    set(BREAKPADCOMMON_LIBS ${FOUNDATION_LIBRARY})
    find_library(APPLICATIONSERVICES_LIBRARY ApplicationServices REQUIRED)
    list(APPEND BREAKPADCOMMON_LIBS ${APPLICATIONSERVICES_LIBRARY})
    if(CMAKE_GENERATOR MATCHES "Xcode")
        # Xcode 10 or higher uses libc++
        list(APPEND BREAKPADCOMMON_LIBS "c++")
    endif()
elseif(UNIX)
    find_package(Threads)
    set(BREAKPADCOMMON_LIBS ${CMAKE_THREAD_LIBS_INIT})
endif()

target_link_libraries(breakpad_common ${BREAKPADCOMMON_LIBS})

add_library(breakpad ${BREAKPAD_LIB_SRCS} ${BREAKPAD_LIB_HEADERS})
breakpad_set_project_properties(breakpad)
target_include_directories(breakpad PUBLIC $<BUILD_INTERFACE:${BREAKPAD_SRC_DIR}>)
if(APPLE)
    if(BUILD_SHARED_LIBS)
        set_target_properties(breakpad PROPERTIES
            MACOSX_RPATH TRUE)
    endif()
endif()
target_link_libraries(breakpad breakpad_common)

if(BKPAD_DUMP_SYMS)
    if(WIN32)
        # Use vswhere tool to determine the install directory of Visual Studio
        set(PROGRAMFILES86 "ProgramFiles(x86)")
        find_program(VSWHERE_TOOL
            NAMES vswhere
            PATHS "$ENV{${PROGRAMFILES86}}/Microsoft Visual Studio/Installer")

        if(VSWHERE_TOOL-NOTFOUND)
            message(WARNING "Unable to find Visual Studio where tool")
        else()
            execute_process(COMMAND "${VSWHERE_TOOL}" -latest -property installationPath
                OUTPUT_VARIABLE VSINSTALLDIR
                RESULT_VARIABLE VSWHERE_EXITCODE
                OUTPUT_STRIP_TRAILING_WHITESPACE)
        endif()

        # DIA SDK is not found in build tools distribution
        set(DEBUG_ACCESS_SDK_DIR "${VSINSTALLDIR}/DIA SDK")

        # Look for DIA SDK using known environment variables
        if (NOT EXISTS "${DEBUG_ACCESS_SDK_DIR}")
            set(DEBUG_ACCESS_SDK_DIR "$ENV{VSINSTALLDIR}/DIA SDK")
        endif()
        if (NOT EXISTS "${DEBUG_ACCESS_SDK_DIR}")
            set(DEBUG_ACCESS_SDK_DIR "$ENV{VS160COMNTOOLS}/../../DIA SDK")
        endif()
        if (NOT EXISTS "${DEBUG_ACCESS_SDK_DIR}")
            set(DEBUG_ACCESS_SDK_DIR "$ENV{VS150COMNTOOLS}/../../DIA SDK")
        endif()
        if (NOT EXISTS "${DEBUG_ACCESS_SDK_DIR}")
            set(DEBUG_ACCESS_SDK_DIR "$ENV{VS140COMNTOOLS}/../../DIA SDK")
        endif()
        if (NOT EXISTS "${DEBUG_ACCESS_SDK_DIR}")
            set(DEBUG_ACCESS_SDK_DIR "$ENV{VS120COMNTOOLS}/../../DIA SDK")
        endif()
        if (NOT EXISTS "${DEBUG_ACCESS_SDK_DIR}")
            # The Debug Interface Access SDK is required to build the dump_syms tool.
            # It ships with the certain versions/editions of Visual Studio or can be
            # downloaded separately as part of the Windows SDK.
            message(FATAL_ERROR "DIA SDK not found in ${DEBUG_ACCESS_SDK_DIR}.")
        else()
            message(STATUS "DIA SDK directory: ${DEBUG_ACCESS_SDK_DIR}")
        endif()

        include_directories(${DEBUG_ACCESS_SDK_DIR}/include)

        set(DUMPSYMS_SRCS
            ${BREAKPAD_SRC_DIR}/common/windows/dia_util.cc
            ${BREAKPAD_SRC_DIR}/common/windows/omap.cc
            ${BREAKPAD_SRC_DIR}/common/windows/pdb_source_line_writer.cc
            ${BREAKPAD_SRC_DIR}/tools/windows/dump_syms/dump_syms.cc)
        set(DUMPSYMS_HEADERS
            ${BREAKPAD_SRC_DIR}/common/windows/pdb_source_line_writer.h)

        if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
            set(DEBUG_ACCESS_SDK_LIB_DIR ${DEBUG_ACCESS_SDK_DIR}/lib/amd64)
            set(DEBUG_ACCESS_SDK_BIN_DIR ${DEBUG_ACCESS_SDK_DIR}/bin/amd64)
        else()
            set(DEBUG_ACCESS_SDK_LIB_DIR ${DEBUG_ACCESS_SDK_DIR}/lib)
            set(DEBUG_ACCESS_SDK_BIN_DIR ${DEBUG_ACCESS_SDK_DIR}/bin)
        endif()

        message(STATUS "DIA SDK library directory: ${DEBUG_ACCESS_SDK_LIB_DIR}")
        message(STATUS "DIA SDK binary directory: ${DEBUG_ACCESS_SDK_BIN_DIR}")

        find_library(DIA_SDK_GUID_LIB diaguids PATHS ${DEBUG_ACCESS_SDK_LIB_DIR})
        message(STATUS "DIA SDK guid library: ${DIA_SDK_GUID_LIB}")

        find_file(DIA_SDK_140_DLL msdia140.dll PATHS ${DEBUG_ACCESS_SDK_BIN_DIR})
        message(STATUS "DIA SDK library: ${DIA_SDK_140_DLL}")

        set(DUMPSYMS_LIBS ${DIA_SDK_GUID_LIB} dbghelp)
    else()
        set(DUMPSYMS_SRCS
            ${BREAKPAD_SRC_DIR}/common/dwarf/bytereader.cc
            ${BREAKPAD_SRC_DIR}/common/dwarf/dwarf2diehandler.cc
            ${BREAKPAD_SRC_DIR}/common/dwarf/dwarf2reader.cc
            ${BREAKPAD_SRC_DIR}/common/dwarf/elf_reader.cc
            ${BREAKPAD_SRC_DIR}/common/dwarf_cfi_to_module.cc
            ${BREAKPAD_SRC_DIR}/common/dwarf_cu_to_module.cc
            ${BREAKPAD_SRC_DIR}/common/dwarf_line_to_module.cc
            ${BREAKPAD_SRC_DIR}/common/dwarf_range_list_handler.cc
            ${BREAKPAD_SRC_DIR}/common/language.cc
            ${BREAKPAD_SRC_DIR}/common/module.cc
            ${BREAKPAD_SRC_DIR}/common/stabs_reader.cc
            ${BREAKPAD_SRC_DIR}/common/stabs_to_module.cc)
        set(DUMPSYMS_HEADERS
            ${BREAKPAD_SRC_DIR}/common/dwarf/bytereader.h
            ${BREAKPAD_SRC_DIR}/common/dwarf/dwarf2diehandler.h
            ${BREAKPAD_SRC_DIR}/common/dwarf/dwarf2reader.h
            ${BREAKPAD_SRC_DIR}/common/dwarf/elf_reader.h
            ${BREAKPAD_SRC_DIR}/common/dwarf_cfi_to_module.h
            ${BREAKPAD_SRC_DIR}/common/dwarf_cu_to_module.h
            ${BREAKPAD_SRC_DIR}/common/dwarf_line_to_module.h
            ${BREAKPAD_SRC_DIR}/common/dwarf_range_list_handler.h
            ${BREAKPAD_SRC_DIR}/common/language.h
            ${BREAKPAD_SRC_DIR}/common/module.h
            ${BREAKPAD_SRC_DIR}/common/stabs_reader.h
            ${BREAKPAD_SRC_DIR}/common/stabs_to_module.h)

        if(APPLE)
            list(APPEND DUMPSYMS_SRCS
                ${BREAKPAD_SRC_DIR}/common/mac/arch_utilities.cc
                ${BREAKPAD_SRC_DIR}/common/mac/dump_syms.cc
                ${BREAKPAD_SRC_DIR}/common/mac/macho_reader.cc
                ${BREAKPAD_SRC_DIR}/tools/mac/dump_syms/dump_syms_tool.cc)
            list(APPEND DUMPSYMS_HEADERS
                ${BREAKPAD_SRC_DIR}/common/mac/dump_syms.h
                ${BREAKPAD_SRC_DIR}/common/mac/macho_reader.h)
        elseif(UNIX)
            list(APPEND DUMPSYMS_SRCS
                ${BREAKPAD_SRC_DIR}/common/linux/dump_symbols.cc
                ${BREAKPAD_SRC_DIR}/common/linux/elf_symbols_to_module.cc
                ${BREAKPAD_SRC_DIR}/tools/linux/dump_syms/dump_syms.cc)
            list(APPEND DUMPSYMS_HEADERS
                ${BREAKPAD_SRC_DIR}/common/linux/dump_symbols.h
                ${BREAKPAD_SRC_DIR}/common/linux/elf_symbols_to_module.h)
        endif()
    endif()

    add_executable(dump_syms ${DUMPSYMS_SRCS} ${DUMPSYMS_HEADERS})
    target_link_libraries(dump_syms breakpad_common ${DUMPSYMS_LIBS})

    if(UNIX AND NOT APPLE)
        # Link against zlib for compressed section header support
        if(NOT TARGET ZLIB::ZLIB)
            if(CMAKE_VERSION VERSION_LESS "3.24")
                set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
            else()
                set(ZLIB_USE_STATIC_LIBS ON)
            endif()

            find_package(ZLIB REQUIRED)
        endif()
        target_link_libraries(dump_syms ZLIB::ZLIB)
    endif()

    breakpad_set_project_properties(dump_syms)
    if(WIN32 AND DIA_SDK_140_DLL)
        set(TARGET_DIA_SDK_140_DLL $<TARGET_FILE_DIR:dump_syms>/msdia140.dll)
        add_custom_command(TARGET dump_syms POST_BUILD
            COMMAND "${CMAKE_COMMAND}"
                -E copy "${DIA_SDK_140_DLL}" "${TARGET_DIA_SDK_140_DLL}"
            COMMENT "Copying msdia140.dll to output directory")
    endif()
endif()

if(UNIX)
    check_function_exists(getcontext HAVE_GETCONTEXT)

    if(HAVE_GETCONTEXT)
        target_compile_definitions(breakpad PRIVATE HAVE_GETCONTEXT)
        target_compile_definitions(breakpad_common PRIVATE HAVE_GETCONTEXT)
    else()
        target_sources(breakpad_common PRIVATE
            ${BREAKPADCOMMON_LIB_GETCONTEXT_SRCS}
            ${BREAKPADCOMMON_LIB_GETCONTEXT_HEADERS})
    endif()

    message(STATUS "Retrieving linux-syscall-support repository")

    FetchContent_Declare(lss
        GIT_REPOSITORY https://chromium.googlesource.com/linux-syscall-support
        SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/lss)
    FetchContent_MakeAvailable(lss)

    target_include_directories(breakpad_common PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    target_include_directories(breakpad PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

    if(BKPAD_DUMP_SYMS)
        target_include_directories(dump_syms PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    endif()
endif()

if(BKPAD_MINIDUMP)
    if(WIN32)
        include_directories(${BREAKPAD_SRC_DIR}/third_party/windows/include)
    elseif(APPLE)
        include_directories(${BREAKPAD_SRC_DIR}/third_party/mac/include)
    elseif(UNIX)
        include_directories(${BREAKPAD_SRC_DIR}/third_party/linux/include)
    endif()

    set(LIBDISASM_DIR ${BREAKPAD_SRC_DIR}/third_party/libdisasm)
    set(LIBDISASM_SRCS
        ${LIBDISASM_DIR}/ia32_implicit.c
        ${LIBDISASM_DIR}/ia32_insn.c
        ${LIBDISASM_DIR}/ia32_invariant.c
        ${LIBDISASM_DIR}/ia32_modrm.c
        ${LIBDISASM_DIR}/ia32_invariant.c
        ${LIBDISASM_DIR}/ia32_opcode_tables.c
        ${LIBDISASM_DIR}/ia32_operand.c
        ${LIBDISASM_DIR}/ia32_reg.c
        ${LIBDISASM_DIR}/ia32_settings.c
        ${LIBDISASM_DIR}/x86_disasm.c
        ${LIBDISASM_DIR}/x86_format.c
        ${LIBDISASM_DIR}/x86_imm.c
        ${LIBDISASM_DIR}/x86_insn.c
        ${LIBDISASM_DIR}/x86_misc.c
        ${LIBDISASM_DIR}/x86_operand_list.c)

    enable_language(C)
    add_library(disasm STATIC ${LIBDISASM_SRCS})
    breakpad_set_project_properties(disasm)

    set(PROCESSOR_SRC_DIR ${BREAKPAD_SRC_DIR}/processor)
    set(PROCESSOR_COMMON_SRCS
        ${PROCESSOR_SRC_DIR}/basic_code_modules.cc
        ${PROCESSOR_SRC_DIR}/basic_source_line_resolver.cc
        ${PROCESSOR_SRC_DIR}/call_stack.cc
        ${PROCESSOR_SRC_DIR}/cfi_frame_info.cc
        ${PROCESSOR_SRC_DIR}/convert_old_arm64_context.cc
        ${PROCESSOR_SRC_DIR}/disassembler_objdump.cc
        ${PROCESSOR_SRC_DIR}/disassembler_x86.cc
        ${PROCESSOR_SRC_DIR}/dump_context.cc
        ${PROCESSOR_SRC_DIR}/dump_object.cc
        ${PROCESSOR_SRC_DIR}/exploitability.cc
        ${PROCESSOR_SRC_DIR}/exploitability_win.cc
        ${PROCESSOR_SRC_DIR}/exploitability_linux.cc
        ${PROCESSOR_SRC_DIR}/logging.cc
        ${PROCESSOR_SRC_DIR}/minidump.cc
        ${PROCESSOR_SRC_DIR}/minidump_processor.cc
        ${PROCESSOR_SRC_DIR}/pathname_stripper.cc
        ${PROCESSOR_SRC_DIR}/process_state.cc
        ${PROCESSOR_SRC_DIR}/proc_maps_linux.cc
        ${PROCESSOR_SRC_DIR}/simple_symbol_supplier.cc
        ${PROCESSOR_SRC_DIR}/source_line_resolver_base.cc
        ${PROCESSOR_SRC_DIR}/stack_frame_symbolizer.cc
        ${PROCESSOR_SRC_DIR}/stackwalk_common.cc
        ${PROCESSOR_SRC_DIR}/stackwalker.cc
        ${PROCESSOR_SRC_DIR}/stackwalker_amd64.cc
        ${PROCESSOR_SRC_DIR}/stackwalker_arm.cc
        ${PROCESSOR_SRC_DIR}/stackwalker_arm64.cc
        ${PROCESSOR_SRC_DIR}/stackwalker_mips.cc
        ${PROCESSOR_SRC_DIR}/stackwalker_ppc.cc
        ${PROCESSOR_SRC_DIR}/stackwalker_ppc64.cc
        ${PROCESSOR_SRC_DIR}/stackwalker_riscv.cc
        ${PROCESSOR_SRC_DIR}/stackwalker_riscv64.cc
        ${PROCESSOR_SRC_DIR}/stackwalker_sparc.cc
        ${PROCESSOR_SRC_DIR}/stackwalker_x86.cc
        ${PROCESSOR_SRC_DIR}/source_line_resolver_base.cc
        ${PROCESSOR_SRC_DIR}/symbolic_constants_win.cc
        ${PROCESSOR_SRC_DIR}/tokenize.cc)
    set(PROCESSOR_COMMON_HEADERS
        ${PROCESSOR_SRC_DIR}/basic_code_modules.h
        ${PROCESSOR_SRC_DIR}/cfi_frame_info.h
        ${PROCESSOR_SRC_DIR}/disassembler_objdump.h
        ${PROCESSOR_SRC_DIR}/disassembler_x86.h
        ${PROCESSOR_SRC_DIR}/exploitability_win.h
        ${PROCESSOR_SRC_DIR}/exploitability_linux.h
        ${PROCESSOR_SRC_DIR}/logging.h
        ${PROCESSOR_SRC_DIR}/pathname_stripper.h
        ${PROCESSOR_SRC_DIR}/simple_symbol_supplier.h
        ${PROCESSOR_SRC_DIR}/stackwalk_common.h
        ${PROCESSOR_SRC_DIR}/stackwalker_amd64.h
        ${PROCESSOR_SRC_DIR}/stackwalker_arm.h
        ${PROCESSOR_SRC_DIR}/stackwalker_arm64.h
        ${PROCESSOR_SRC_DIR}/stackwalker_mips.h
        ${PROCESSOR_SRC_DIR}/stackwalker_ppc.h
        ${PROCESSOR_SRC_DIR}/stackwalker_ppc64.h
        ${PROCESSOR_SRC_DIR}/stackwalker_riscv.h
        ${PROCESSOR_SRC_DIR}/stackwalker_riscv64.h
        ${PROCESSOR_SRC_DIR}/stackwalker_sparc.h
        ${PROCESSOR_SRC_DIR}/stackwalker_x86.h
        ${PROCESSOR_SRC_DIR}/tokenize.h)

    add_library(processor_common STATIC ${PROCESSOR_COMMON_SRCS} ${PROCESSOR_COMMON_HEADERS})
    breakpad_set_project_properties(processor_common)

    set(MINIDUMP_STACKWALK_SRCS
        ${PROCESSOR_SRC_DIR}/minidump.cc
        ${PROCESSOR_SRC_DIR}/minidump_stackwalk.cc)

    if(WIN32)
        include_directories(${BREAKPAD_SRC_DIR}/third_party/getopt)
        include_directories(${BREAKPAD_SRC_DIR}/third_party/windows)
        list(APPEND MINIDUMP_STACKWALK_SRCS
            ${BREAKPAD_SRC_DIR}/third_party/getopt/getopt.c)
        list(APPEND MINIDUMP_STACKWALK_HEADERS
            ${BREAKPAD_SRC_DIR}/third_party/getopt/getopt.h
            ${BREAKPAD_SRC_DIR}/third_party/windows/unistd.h)
    endif()

    add_executable(minidump_stackwalk ${MINIDUMP_STACKWALK_SRCS} ${MINIDUMP_STACKWALK_HEADERS})
    breakpad_set_project_properties(minidump_stackwalk)
    target_link_libraries(minidump_stackwalk breakpad_common processor_common disasm)
endif()
