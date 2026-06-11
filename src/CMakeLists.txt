add_subdirectory(libltfs)

# CRC routines compiled with SSE4.2 where available (replaces the autotools
# per-backend symlink + CRC_OPTIMIZE compile rule). One object library, linked
# into every tape backend.
add_library(ltfs_crc OBJECT
    tape_drivers/reed_solomon_crc.c
    tape_drivers/crc32c_crc.c)
target_include_directories(ltfs_crc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/tape_drivers)
if(LTFS_CRC_OPTIONS)
    target_compile_options(ltfs_crc PRIVATE ${LTFS_CRC_OPTIONS})
endif()
set_target_properties(ltfs_crc PROPERTIES POSITION_INDEPENDENT_CODE ON)

# I/O scheduler and key-manager plugins (always built).
add_subdirectory(iosched)
add_subdirectory(kmi)

# Tape backends: the file simulator and the image driver are portable; the
# real device backends are platform-specific.
add_subdirectory(tape_drivers/generic/file)
add_subdirectory(tape_drivers/generic/itdtimg)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_subdirectory(tape_drivers/linux/sg)
    if(LTFS_ENABLE_LINTAPE)
        add_subdirectory(tape_drivers/linux/lin_tape)
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_subdirectory(tape_drivers/osx/iokit)
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    add_subdirectory(tape_drivers/freebsd/cam)
elseif(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
    add_subdirectory(tape_drivers/netbsd/scsipi-ibmtape)
endif()

add_subdirectory(utils)

# The ltfs mount executable.
add_executable(ltfs_bin main.c ltfs_fuse.c)
set_target_properties(ltfs_bin PROPERTIES OUTPUT_NAME ltfs)
set_target_properties(ltfs_bin PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(ltfs_bin PRIVATE ltfs)
ltfs_link_message(ltfs_bin bin_ltfs)
install(TARGETS ltfs_bin RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
