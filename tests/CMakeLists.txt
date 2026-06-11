# Integration tests: the same shell scripts the autotools `make check` runs,
# driven here by ctest. They need a Linux host with /dev/fuse and are skipped
# (exit 77) elsewhere.
add_executable(fsops_helper helpers/fsops_helper.c)
set_target_properties(fsops_helper PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests/helpers")

set(_tests
    00-mount-umount
    01-io-roundtrip
    02-truncate
    03-rename
    04-dirs-unlink
    05-xattr
    06-symlink
    07-large-dir
    08-tar-roundtrip
    09-rename-flags-ftruncate
    10-request-size
    11-direct-io
    12-readdirplus
    13-large-write-integrity)

foreach(_t ${_tests})
    add_test(NAME ${_t}
             COMMAND sh "${CMAKE_CURRENT_SOURCE_DIR}/t/${_t}.sh")
    set_tests_properties(${_t} PROPERTIES
        SKIP_RETURN_CODE 77
        ENVIRONMENT "top_srcdir=${CMAKE_SOURCE_DIR};top_builddir=${CMAKE_BINARY_DIR}")
endforeach()
