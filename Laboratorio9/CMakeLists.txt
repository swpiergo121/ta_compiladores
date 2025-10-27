cmake_minimum_required(VERSION 3.29)
project(Laboratorio9)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Laboratorio9
    ast.cpp
    ast.h
    environment.h
    main.cpp
    parser.cpp
    parser.h
    scanner.cpp
    scanner.h
    semantic_types.h
    token.cpp
    token.h
    TypeChecker.cpp
    TypeChecker.h
    visitor.cpp
    visitor.h)
