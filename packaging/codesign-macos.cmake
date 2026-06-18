execute_process(
    COMMAND codesign --force --deep --sign -
        "${CPACK_TEMPORARY_DIRECTORY}/adblink.app"
    RESULT_VARIABLE result
)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "codesign failed: ${result}")
endif()
message(STATUS "Re-signed adblink.app after CPack modifications")