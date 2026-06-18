execute_process(
    COMMAND codesign --force --deep --sign
        "Developer ID Application: jeff elkins (9Q77WK7W3R)"
        --timestamp --options=runtime
        --entitlements "${ENTITLEMENTS_PATH}"
        "${CPACK_TEMPORARY_DIRECTORY}/adblink.app"
    RESULT_VARIABLE result
)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "codesign failed: ${result}")
endif()
message(STATUS "Signed adblink.app in CPack staging directory")