cmake_minimum_required (VERSION 2.5)
project (ums2net)
set (ums2net_VERSION_MAJOR 0)
set (ums2net_VERSION_MINOR 1)
subdirs(src)
subdirs(conf)
subdirs(include)


set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${ums2net_VERSION_MAJOR}.${ums2net_VERSION_MINOR}")
set(CPACK_SOURCE_GENERATOR "TXZ")
set(CPACK_SOURCE_IGNORE_FILES "/build/;/.git/;~$;${CPACK_SOURCE_IGNORE_FILES};.*.cmake$;CMakeCache.txt;CMakeFiles;_CPack_Packages;Makefile;.*.tar.xz;install_manifest.txt")
include(CPack)

enable_testing()
