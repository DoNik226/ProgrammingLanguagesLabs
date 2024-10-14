file(GLOB_RECURSE sources CONFIGURE_DEPENDS
    src/*.c
    src/*.h
    include/*.h
)

add_executable(image-transformer ${sources}
        src/main.c
        include/image.h
        src/rotate.c
        include/rotate.h
        src/bmp.c
        include/bmp.h)
target_include_directories(image-transformer PRIVATE src include)
