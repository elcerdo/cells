project(cells)
cmake_minimum_required(VERSION 2.6)

# find packages
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

# simualtion library
add_library(cellsworld
    common.cc
    world.cc
    minds.cc)
target_link_libraries(cellsworld
    ${PYTHON_LIBRARIES})

# basic benchmark tests
add_executable(cellbenchmark_cc
    benchmark.cc
    cellbenchmark_cc.cc)
target_link_libraries(cellbenchmark_cc
    cellsworld)
add_executable(cellbenchmark_python
    benchmark.cc
    cellbenchmark_python.cc)
target_link_libraries(cellbenchmark_python
    cellsworld)

# qt gui of simulator
qt4_wrap_cpp(moc_srcs
    mainwindow.h
    mindsmodel.h
    createworldwidget.h
    worldwidget.h)
add_executable(qcells
    qcells.cc
    mainwindow.cc
    mindsmodel.cc
    createworldwidget.cc
    worldwidget.cc
    ${moc_srcs})
target_link_libraries(qcells
    cellsworld
    ${QT_LIBRARIES})
