project(cells)
cmake_minimum_required(VERSION 2.6)

# simualtion library
add_library(cellsworld
    common.cc
    world.cc
    minds.cc)

# basic benchmark test
add_executable(cellbenchmark
    cellbenchmark.cc)
target_link_libraries(cellbenchmark
    cellsworld)

# python mind test
find_package(PythonLibs)
if(PYTHONLIBS_FOUND)
    include_directories(${PYTHON_INCLUDE_DIRS})
    
    add_executable(cellpython
        cellpython.cc)

    target_link_libraries(cellpython
        cellsworld
        ${PYTHON_LIBRARIES})
endif(PYTHONLIBS_FOUND)

# qt gui of simulator
find_package(Qt4)
if(QT4_FOUND)
    include(${QT_USE_FILE})

    qt4_wrap_cpp(moc_srcs
        mainwindow.h
        worldwidget.h
        )

    add_executable(qcells
        qcells.cc
        mainwindow.cc
        worldwidget.cc
        ${moc_srcs})
    target_link_libraries(qcells
        cellsworld
        ${QT_LIBRARIES})
endif(QT4_FOUND)

