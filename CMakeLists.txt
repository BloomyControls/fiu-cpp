cmake_minimum_required(VERSION 3.14)

project(fiu-cpp
  VERSION 1.0.0
  DESCRIPTION "FIU Serial Library"
  HOMEPAGE_URL "https://github.com/BloomyControls/fiu-cpp"
)

option(FIU_INSTALL "Enable install" ON)

if(FIU_STATIC)
  set(FIU_LIB_TYPE STATIC)
elseif(BUILD_SHARED_LIBS)
  set(FIU_LIB_TYPE SHARED)
else()
  set(FIU_LIB_TYPE STATIC)
endif()

include(FetchContent)

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

set(Boost_USE_STATIC_LIBS ON)
set(BOOST_INCLUDE_LIBRARIES system asio)
set(BOOST_ENABLE_CMAKE ON)
# use CONFIG to avoid CMP0167 warnings
find_package(Boost 1.88.0 CONFIG COMPONENTS system QUIET)
if (NOT Boost_FOUND)
  set(SAVED_BUILD_SHARED ${BUILD_SHARED_LIBS})
  set(BUILD_SHARED_LIBS OFF)
  FetchContent_Declare(Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.88.0/boost-1.88.0-cmake.tar.gz
    URL_HASH "SHA1=7667d5edc4e753965db02595c912f78ad64725fe"
  )
  FetchContent_GetProperties(Boost)
  if(NOT Boost_POPULATED)
    message(STATUS "Fetch Boost (may take a while)")
    FetchContent_MakeAvailable(Boost)
  endif()
  # workaround an issue with fetched boost asio
  set(BOOST_LIBS ${Boost_LIBRARIES} Boost::asio)
  set(BUILD_SHARED_LIBS ${SAVED_BUILD_SHARED})
else()
  set(BOOST_LIBS ${Boost_LIBRARIES})
endif()

find_package(Doxygen 1.9.8 OPTIONAL_COMPONENTS dot)

add_library(fiu ${FIU_LIB_TYPE}
  src/CInterface.cpp
  src/Errors.cpp
  src/FiuClient.cpp
  src/SerialPort.cpp
)
add_library(bci::fiu ALIAS fiu)

set_target_properties(fiu PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  POSITION_INDEPENDENT_CODE ON
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN TRUE
)

include(GenerateExportHeader)
generate_export_header(fiu
  EXPORT_MACRO_NAME FIU_API
)
if(${FIU_LIB_TYPE} STREQUAL STATIC)
  target_compile_definitions(fiu PRIVATE FIU_STATIC_DEFINE)
endif()

target_compile_features(fiu PUBLIC cxx_std_20)

target_compile_definitions(fiu PRIVATE
  "FIU_VERSION=((${PROJECT_VERSION_MAJOR}*10000)+(${PROJECT_VERSION_MINOR}*100)+(${PROJECT_VERSION_PATCH}))"
)

if(MSVC)
  # disable C5030, unknown attributes
  target_compile_options(fiu PRIVATE /Zc:__cplusplus /W3 /wd5030)
else()
  target_compile_options(fiu PRIVATE -Wall -Wextra)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(fiu PRIVATE -Wno-psabi -Wno-attributes=msvc::)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(fiu PRIVATE -Wno-unknown-attributes)
  endif()
endif()

if(FIU_INSTALL AND BUILD_SHARED_LIBS)
  include(GNUInstallDirs)
endif()

target_include_directories(fiu PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(fiu PRIVATE
  ${BOOST_LIBS}
)

if(DOXYGEN_FOUND)
  include(cmake/add_doxygen_target.cmake)
endif()

if(FIU_INSTALL AND BUILD_SHARED_LIBS)
  include(CMakePackageConfigHelpers)

  set(targets_export_name "fiu-targets")
  set(version_config "${PROJECT_BINARY_DIR}/fiu-cpp-config-version.cmake")
  set(project_config "${PROJECT_BINARY_DIR}/fiu-cpp-config.cmake")
  set(fiu_cmake_dir "${CMAKE_INSTALL_LIBDIR}/cmake/fiu-cpp")

  install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/bci"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT devel)

  install(FILES "${PROJECT_BINARY_DIR}/fiu_export.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bci/fiu
    COMPONENT devel)

  install(TARGETS fiu
    EXPORT ${targets_export_name}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT libs)

  write_basic_package_version_file(
    ${version_config}
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion)

  configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/fiu-cpp-config.cmake.in
    ${project_config}
    INSTALL_DESTINATION ${fiu_cmake_dir})

  install(FILES ${project_config} ${version_config}
    DESTINATION ${fiu_cmake_dir}
    COMPONENT devel)
  install(EXPORT ${targets_export_name} DESTINATION ${fiu_cmake_dir}
    NAMESPACE bci::
    COMPONENT devel)

  # TODO: generate DEB packages
  set(CPACK_PACKAGE_NAME "fiu-cpp")
  set(CPACK_PACKAGE_VENDOR "Bloomy Controls, Inc.")
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
  set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
  set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
  install(FILES
    ${CPACK_RESOURCE_FILE_LICENSE}
    ${CPACK_RESOURCE_FILE_README}
    DESTINATION .
    COMPONENT Metadata
    EXCLUDE_FROM_ALL)
  set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
  set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)

  set(CPACK_PACKAGE_INSTALL_DIRECTORY "Bloomy Controls/fiu-cpp")
  set(CPACK_WIX_VERSION 4)
  set(CPACK_WIX_UPGRADE_GUID "C86E7A85-9979-48E9-A1CC-C33F55B84D77")
  set(CPACK_WIX_ROOT_FEATURE_TITLE "FIU Serial Driver")
  set(CPACK_WIX_ROOT_FEATURE_DESCRIPTION
    "FIU Serial driver C/C++ library and development files")
  set(CPACK_WIX_PROPERTY_ARPCOMMENTS "FIU Serial Driver C/C++ Library")
  set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "${PROJECT_HOMEPAGE_URL}")
  set(CPACK_WIX_CMAKE_PACKAGE_REGISTRY "fiu-cpp")
  set(CPACK_WIX_UI_BANNER "${CMAKE_CURRENT_SOURCE_DIR}/img/installer_banner.jpg")
  set(CPACK_WIX_UI_DIALOG "${CMAKE_CURRENT_SOURCE_DIR}/img/installer_dialog.jpg")

  if(WIN32)
    # this will create a "package" target on Windows so that we don't have to
    # invoke CPack directly
    set(CPACK_GENERATOR "WIX")
  endif()

  include(CPack)

  cpack_add_component(Metadata HIDDEN)
  cpack_add_component(libs REQUIRED
    DISPLAY_NAME "BS120x driver"
    DESCRIPTION "C/C++ BS120x driver library")
  cpack_add_component(devel
    DISPLAY_NAME "Development files"
    DESCRIPTION "Headers and CMake support files for developing software using the libraries")
endif()
