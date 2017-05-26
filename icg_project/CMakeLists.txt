get_filename_component(EXERCISENAME ${CMAKE_CURRENT_LIST_DIR} NAME)
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

# these files are copied only once
file(GLOB TEXTURES
    grid/grid_texture.tga
    grid/ground.tga
    grid/rock.tga
    grid/grass.tga
    grid/snow.tga
    water/water_texture.tga
    water/clear_water.jpg
    quad/quad_texture.tga
    tree/tree_texture.tga
    tree/sapin_texture.tga
    tree/pin_texture.tga
    skybox/cottoncandy_rt.tga
    skybox/cottoncandy_lf.tga
    skybox/cottoncandy_up.tga
    skybox/cottoncandy_dn.tga
    skybox/cottoncandy_bk.tga
    skybox/cottoncandy_ft.tga
    skybox/violentdays_rt.tga
    skybox/violentdays_lf.tga
    skybox/violentdays_up.tga
    skybox/violentdays_dn.tga
    skybox/violentdays_bk.tga
    skybox/violentdays_ft.tga
    skybox/cloudtop_rt.tga
    skybox/cloudtop_lf.tga
    skybox/cloudtop_up.tga
    skybox/cloudtop_dn.tga
    skybox/cloudtop_bk.tga
    skybox/cloudtop_ft.tga
    skybox/right.jpg
    skybox/left.jpg
    skybox/top.jpg
    skybox/bottom.jpg
    skybox/back.jpg
    skybox/front.jpg
    skybox/miramar_rt.tga
    skybox/miramar_lf.tga
    skybox/miramar_up.tga
    skybox/miramar_dn.tga
    skybox/miramar_bk.tga
    skybox/miramar_ft.tga
    skybox/grimmnightbk.tga
    skybox/grimmnightft.tga
    skybox/grimmnightup.tga
    skybox/grimmnightdn.tga
    skybox/grimmnightrt.tga
    skybox/grimmnightlf.tga)
copy_files_once(${TEXTURES})

# list all the files you want to copy everytime
# you build (ie, you want the shaders there)
file(GLOB SHADERS
  grid/*.glsl
  water/*.glsl
  quad/*.glsl
  skybox/*.glsl
  tree/*.glsl)
deploy_shaders_to_build_dir(${SHADERS})

add_executable(${EXERCISENAME} ${SOURCES} ${HEADERS} ${SHADERS})
target_link_libraries(${EXERCISENAME} ${COMMON_LIBS})
