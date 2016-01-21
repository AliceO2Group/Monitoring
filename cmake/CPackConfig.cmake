# @author  Barthélémy von Haller
# @author  Vasco Barroso

# General CPack configuration 
# Info: http://www.itk.org/Wiki/CMake:Component_Install_With_CPack
# _____________________________________________________________________________

set(CPACK_PACKAGE_NAME "Monitoring-DataCollector${VERSION_MAJOR}")

if(APPLE)
  set(CPACK_PACKAGE_VENDOR "ALICE") # PackageMaker doesn't like http://
else()
  set(CPACK_PACKAGE_VENDOR "http://aliceinfo.cern.ch") # deb lintian insists on URL
endif()

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Monitoring")
set(CPACK_PACKAGE_CONTACT "Vasco Barroso <Vasco.Chibante.Barroso@cern.ch>")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_RPM_PACKAGE_DEBUG 0)

# Select package generator
if(MSVC)
  set(CPACK_GENERATOR "NSIS")
endif(MSVC)

if(APPLE)
  set(CPACK_GENERATOR "PackageMaker")
  set(CPACK_SET_DESTDIR ON)
endif(APPLE)

if (${CMAKE_SYSTEM_NAME} MATCHES Linux)
  find_program(RPM_EXE rpmbuild)
  if(${RPM_EXE} MATCHES RPM_EXE-NOTFOUND)
    set(CPACK_GENERATOR "TGZ;DEB")
  else()
    set(CPACK_GENERATOR "TGZ;DEB;RPM")
  endif()
endif(${CMAKE_SYSTEM_NAME} MATCHES Linux)

# Components
# _____________________________________________________________________________
set(CPACK_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL libs dev apps docs)

set(CPACK_COMPONENT_LIBS_DISPLAY_NAME "Libraries")
set(CPACK_COMPONENT_LIBS_DESCRIPTION "Runtime Libraries")
set(CPACK_COMPONENT_LIBS_GROUP "Runtime")
# express component dependencies this way, it will translate into package dependencies where applicable
set(CPACK_COMPONENT_LIBS_DEPENDS docs) # dependency on another group

set(CPACK_COMPONENT_APPS_DISPLAY_NAME "Applications")
set(CPACK_COMPONENT_APPS_DESCRIPTION "Example Programs")
set(CPACK_COMPONENT_APPS_GROUP "Runtime")
set(CPACK_COMPONENT_APPS_DEPENDS libs docs) # dependency within the same group (lib) and another (docs), but second time from the same group

set(CPACK_COMPONENT_DEV_DISPLAY_NAME "Development files")
set(CPACK_COMPONENT_DEV_DESCRIPTION "Development header files and libraries")
set(CPACK_COMPONENT_DEV_GROUP "Development")
set(CPACK_COMPONENT_DEV_DEPENDS libs) # dependency on another group

set(CPACK_COMPONENT_DOCS_DISPLAY_NAME "Documents")
set(CPACK_COMPONENT_DOCS_DESCRIPTION "User Documentation")
#set(CPACK_COMPONENT_DOCS_GROUP "Documentation")

# see www.cmake.org/Wiki/CMake:Component_Install_With_CPack#Controlling_Differents_Ways_of_packaging_components
#set(CPACK_COMPONENTS_GROUPING "ALL_COMPONENTS_IN_ONE") # IGNORE, ONE_PER_GROUP (default), ALL_COMPONENTS_IN_ONE

# Debian specific configuration (minimum)
# _____________________________________________________________________________

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.1-6), libboost-test-dev")
SET(CPACK_DEBIAN_PACKAGE_CONFLICTS "Monitoring-DataCollector0-apps")

# RPM specific configuration (minimum)
# _____________________________________________________________________________
set(CPACK_RPM_PACKAGE_LICENSE "Proprietary")
set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
set(CPACK_RPM_PACKAGE_VERSION ${VERSION})
set(CPACK_RPM_COMPONENT_INSTALL ON)   # necessary even if CPACK_COMPONENT_INSTALL set to ON. A bug in my opinion.


# OS X PackageMaker
# _____________________________________________________________________________
set(CPACK_OSX_PACKAGE_VERSION "10.5")

include (InstallRequiredSystemLibraries)
include (CPack)
