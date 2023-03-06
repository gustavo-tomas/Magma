-- Workspace -----------------------------------------------------------------------------------------------------------
workspace "magma"
    architecture "x86_64"
    toolset "clang"
    language "C"
    cppdialect "gnu++17"
    configurations { "Debug", "Release" }

-- Magma (Engine) ------------------------------------------------------------------------------------------------------
project "magma"
    location "magma"
    kind "SharedLib"

    targetdir ("bin/%{prj.name}")
    objdir ("bin/build/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.c"
    }

    filter "system:linux"
        pic "On"

    filter "configurations:Debug"
        buildoptions { "-Wall", "-g", "-fsanitize=address", "-fno-omit-frame-pointer" }
        linkoptions { "-fsanitize=address", "-lX11", "-lX11-xcb", "-lxcb" }
        defines "_DEBUG"
        symbols "On"
        runtime "Debug"


-- Sandbox (Aplicação) -------------------------------------------------------------------------------------------------
project "sandbox"
    location "sandbox"
    kind "ConsoleApp"

    targetdir ("bin/%{prj.name}")
    objdir ("bin/build/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.c"
    }

    includedirs { "magma/src" }

    links { "magma" }

    filter "system:linux"
        pic "On"

    filter "configurations:Debug"
        buildoptions { "-Wall", "-g", "-fsanitize=address", "-fno-omit-frame-pointer" }
        linkoptions { "-fsanitize=address" }
        defines "_DEBUG"
        runtime "Debug"
        symbols "On"
