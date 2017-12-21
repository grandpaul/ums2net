find_package(PkgConfig)

pkg_check_modules(CPPUNIT cppunit)
link_directories(${CPPUNIT_LIBRARY_DIRS})
include_directories(${CPPUNIT_LIBRARY_DIRS})

add_executable(testUMS2NET-ConfRecord testUMS2NET-ConfRecord.cc ../ums2netconfrecord.cc)
target_compile_options(testUMS2NET-ConfRecord PUBLIC ${CPPUNIT_CFLAGS})
target_link_libraries(testUMS2NET-ConfRecord ${CPPUNIT_LIBRARIES})

add_test(UMS2NET-ConfRecord testUMS2NET-ConfRecord)

add_executable(testUMS2NET-ConfigReader testUMS2NET-ConfigReader.cc ../ums2netconfrecord.cc ../configReader.cc)
target_compile_options(testUMS2NET-ConfigReader PUBLIC ${CPPUNIT_CFLAGS})
target_link_libraries(testUMS2NET-ConfigReader ${CPPUNIT_LIBRARIES})

add_test(UMS2NET-ConfigReader testUMS2NET-ConfigReader)
