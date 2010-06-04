project(cells)
cmake_minimum_required(VERSION 2.6)

add_library(cellsworld
    common.cc
    world.cc
    minds.cc)

find_package(Qt4)
if(QT_FOUND)
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
endif(QT_FOUND)

