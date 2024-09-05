add_rules("mode.debug", "mode.release", "mode.profile")
set_languages("c++20")
add_requires("glfw", "glm", "glad", "gnu-gsl", "spdlog", "tracy", "stb", "freetype")
add_requires("imgui", {configs = {glfw = true, opengl3 = true}})

target("ST2DCore")
    set_kind("shared")
    add_defines("ST_BUILD_DLL")

    if is_mode("debug") then 
        add_defines("ST_DEBUG")
        set_symbols("debug")
        set_optimize("none")
    end 

    if is_mode("release", "profile") then 
        add_defines("ST_RELEASE")
        set_symbols("hidden")
        set_optimize("fastest")
    end
    
    add_packages("spdlog", "tracy")
    add_headerfiles("Core/Source/**.h")
    add_files("Core/Source/**.cpp")
    add_includedirs("Core/Source")

target("ST2DEditor")
    set_kind("binary")
    add_defines("ST_BUILD_EXE")

    if is_mode("debug") then 
        add_defines("ST_DEBUG")
        set_symbols("debug")
        set_optimize("none")
    end 

    if is_mode("release", "profile") then 
        add_defines("ST_RELEASE")
        set_symbols("hidden")
        set_optimize("fastest")
    end

    add_packages("spdlog", "tracy", "glfw", "glm", "glad", "gnu-gsl", "stb", "freetype", "imgui")
    add_deps("ST2DCore")
    add_files("Editor/Source/**.cpp")
    add_headerfiles("Editor/Source/**.h")
    add_includedirs("Core/Source", "Editor/Source")

    after_build(function (target)
        local targetdir = target:targetdir()
        local targetfile = path.join(targetdir, target:basename())

        local font_src_dir = path.join(os.projectdir(), "Editor/Resource")

        local font_dst_dir = path.join(targetdir, "Resource")

        if os.isdir(font_src_dir) then
        
            os.cp(font_src_dir, font_dst_dir)
            print("font folder has been copied to: " .. font_dst_dir)
        else
            print("font folder not found in project directory.")
        end
    end)