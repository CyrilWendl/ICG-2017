get_filename_component(EXERCISENAME ${CMAKE_CURRENT_LIST_DIR} NAME)
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

# these files are copied only once
file(GLOB TEXTURES
    grid/grid_texture.tga
    quad/quad_texture.tga
    grid/back.jpg
    grid/bottom.jpg
    grid/front.jpg
    grid/left.jpg
    grid/right.jpg
    grid/top.jpg)
copy_files_once(${TEXTURES})

# list all the files you want to copy everytime
# you build (ie, you want the shaders there)
file(GLOB SHADERS
  grid/*.glsl
  quad/*.glsl)
deploy_shaders_to_build_dir(${SHADERS})

add_executable(${EXERCISENAME} ${SOURCES} ${HEADERS} ${SHADERS})
target_link_libraries(${EXERCISENAME} ${COMMON_LIBS})
