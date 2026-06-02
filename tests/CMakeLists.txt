find_package(Qt6 REQUIRED COMPONENTS Test Sql)

# Match the static CRT used by the main Qt 6.11.1-static build (/MT, not /MD)
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Shared sources compiled once into a convenience library for all tests.
add_library(testsupport STATIC
    "${CMAKE_SOURCE_DIR}/kodidatamanager.cpp"
    "${CMAKE_SOURCE_DIR}/logfile.cpp"
    "${CMAKE_SOURCE_DIR}/adbstring.cpp"
    "${CMAKE_SOURCE_DIR}/getlocaladb.cpp"
    "${CMAKE_SOURCE_DIR}/stringutils.cpp"
)
target_include_directories(testsupport PUBLIC "${CMAKE_SOURCE_DIR}")
target_link_libraries(testsupport PUBLIC Qt6::Sql Qt6::Core)
set_target_properties(testsupport PROPERTIES AUTOMOC ON)

# --- Test: adbString() ---
qt_add_executable(tst_adbstring tst_adbstring.cpp)
target_link_libraries(tst_adbstring PRIVATE testsupport Qt6::Test)
add_test(NAME adbstring COMMAND tst_adbstring)

# --- Test: KodiDataManager CRUD ---
qt_add_executable(tst_database tst_database.cpp)
target_link_libraries(tst_database PRIVATE testsupport Qt6::Test Qt6::Sql)
add_test(NAME database COMMAND tst_database)

# --- Test: JSON config read/write ---
qt_add_executable(tst_jsonconfig tst_jsonconfig.cpp)
target_link_libraries(tst_jsonconfig PRIVATE testsupport Qt6::Test Qt6::Sql)
add_test(NAME jsonconfig COMMAND tst_jsonconfig)

# --- Test: logfile rotation ---
qt_add_executable(tst_logfile tst_logfile.cpp)
target_link_libraries(tst_logfile PRIVATE testsupport Qt6::Test)
add_test(NAME logfile COMMAND tst_logfile)

# --- Test: getlocaladb path resolution ---
qt_add_executable(tst_getadbpath tst_getadbpath.cpp)
target_link_libraries(tst_getadbpath PRIVATE testsupport Qt6::Test)
add_test(NAME getadbpath COMMAND tst_getadbpath)

# --- Test: stringutils ---
qt_add_executable(tst_stringutils tst_stringutils.cpp)
target_link_libraries(tst_stringutils PRIVATE testsupport Qt6::Test)
add_test(NAME stringutils COMMAND tst_stringutils)
