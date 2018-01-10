add_executable(ums2net main.cc ums2netconfrecord.cc configReader.cc servantThread.cc)

install(TARGETS ums2net DESTINATION sbin)
find_library(PTHREAD_LIBRARIES NAMES pthread)
target_link_libraries(ums2net ${PTHREAD_LIBRARIES})
include_directories(${PROJECT_BINARY_DIR})

subdirs(test)
