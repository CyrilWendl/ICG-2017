get_filename_component(EXERCISENAME ${CMAKE_CURRENT_LIST_DIR} NAME)
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

# these files are copied only once
file(GLOB TEXTURES
    grid/grid_texture.tga
    grid/ground.tga
    water/water_texture.tga
    quad/quad_texture.tga
    skybox/cottoncandy_rt.tga
    skybox/cottoncandy_lf.tga
    skybox/cottoncandy_up.tga
    skybox/cottoncandy_dn.tga
    skybox/cottoncandy_bk.tga
    skybox/cottoncandy_ft.tga
    skybox/right.jpg
    skybox/left.jpg
    skybox/top.jpg
    skybox/bottom.jpg
    skybox/back.jpg
    skybox/front.jpg)
copy_files_once(${TEXTURES})

# list all the files you want to copy everytime
# you build (ie, you want the shaders there)
file(GLOB SHADERS
  grid/*.glsl
  water/*.glsl
  quad/*.glsl
  skybox/*.glsl)
deploy_shaders_to_build_dir(${SHADERS})

add_executable(${EXERCISENAME} ${SOURCES} ${HEADERS} ${SHADERS})
target_link_libraries(${EXERCISENAME} ${COMMON_LIBS})
