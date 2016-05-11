
#-------------------------------------------------------------------------------
# Qt project for building Aspect Oriented GUI Behavior library
# as well as the test/example project.
#-------------------------------------------------------------------------------

# main target
PROJECT_NAME = sibapfw_test
TEMPLATE = subdirs
BUILD_DIR = build
DESTDIR = $${BUILD_DIR}
MOC_DIR = $${BUILD_DIR}/mocs
OBJECTS_DIR = $${BUILD_DIR}/objs

# config
QT       += core gui widgets

# testsuite targets
SUBDIRS += sibapfw/sibapfw.pro
SUBDIRS += test/simusaes.pro

