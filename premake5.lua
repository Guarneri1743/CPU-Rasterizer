local solution_dir = "build/" .. _ACTION
local lib_dir = "lib"
local res_dir = "res"
local src_dir = "src"
local include_dir = "include"
local third_party_dir = "third_party"
local shader_dir = "shader"
local sample_dir = "samples"

function setupIncludeDirs()
   includedirs {
      include_dir,
      include_dir .. "/core",
      include_dir .. "/graphics",
      include_dir .. "/editor",
      include_dir .. "/shader",
      include_dir .. "/util",
      shader_dir,
      third_party_dir,
      third_party_dir .. "/assimp",
      third_party_dir .. "/ply",
      third_party_dir .. "/stb_image",
      third_party_dir .. "/rapidjson",
      third_party_dir .. "/imgui",
      third_party_dir .. "/glfw",
      third_party_dir .. "/gl3w",
      third_party_dir .. "/tinymath"
   }
end

function setupSlotion()
   location(solution_dir)
   solution "CPURasterizer"
      configurations {
         "Debug", 
         "Release"
      }

      platforms { "Win64" }
      warnings "Extra"
      floatingpoint "Fast"
      symbols "On"
      cppdialect "C++17"
      rtti "On"
      characterset ("MBCS")

      configuration "Debug*"
         defines { "DEBUG", "_DEBUG" }
         targetdir ( solution_dir .. "lib/Debug" )

      configuration "Release*"
         defines { "NDEBUG"}
         optimize "Speed"
         targetdir ( solution_dir .. "lib/Release"  )

      filter { "platforms:Win*", "configurations:Debug*" }
         defines { "WIN32", "_WINDOWS", "_CRT_SECURE_NO_WARNINGS", "NOMINMAX" }
         system "Windows"
         architecture "x86_64"
         staticruntime "Off"
         libdirs 
         { 
            lib_dir .. "/assimp/Debug",
            lib_dir .. "/glfw/Debug"
         }
         local createResFolder = "mkdir \"$(OutDir)res\""
         local copyAssimp = "xcopy /y /d \"../../" .. lib_dir .. "/assimp/Debug\" \"$(OutDir)\""
         local copyGlfw = "xcopy /y /d \"../../" .. lib_dir .. "/glfw/Debug\" \"$(OutDir)\""
         local copyRes = "xcopy /y /d /s \"../../" .. res_dir .. "\" \"$(OutDir)res\""
         postbuildcommands {
            createResFolder,
            copyAssimp,
            copyGlfw,
            copyRes
         }

      filter { "platforms:Win*", "configurations:Release*" }
         defines { "WIN32", "_WINDOWS", "_CRT_SECURE_NO_WARNINGS", "NOMINMAX" }
         system "Windows"
         architecture "x86_64"
         staticruntime "Off"
         libdirs 
         { 
            lib_dir .. "/assimp/Release",
            lib_dir .. "/glfw/Release"
         }
         local createResFolder = "mkdir \"$(OutDir)res\""
         local copyAssimp = "xcopy /y /d \"../../" .. lib_dir .. "/assimp/Release\" \"$(OutDir)\""
         local copyGlfw = "xcopy /y /d \"../../" .. lib_dir .. "/glfw/Release\" \"$(OutDir)\""
         local copyRes = "xcopy /y /d /s \"../../" .. res_dir .. "\" \"$(OutDir)res\""
         postbuildcommands {
            createResFolder,
            copyAssimp,
            copyGlfw,
            copyRes
         }

      filter { "platforms:Win*", "configurations:Debug*" }
         links { "assimpd", "opengl32", "glfw3" }

      filter { "platforms:Win*", "configurations:Release*" }
         links { "assimp", "opengl32", "glfw3"  }

      filter { "system:windows", "action:vs*"}
    	 flags { "MultiProcessorCompile" }
end

function setupViewerProject()
   project "Viewer"
   kind "ConsoleApp"
   language "C++"

   files { 
      src_dir .. "/*.*", 
      src_dir .. "/util/*.*",
      src_dir .. "/core/*.*",
      src_dir .. "/graphics/*.*",
      src_dir .. "/editor/*.*",
      include_dir .. "/*.*", 
      include_dir .. "/detail/*.*", 
      include_dir .. "/util/*.*",
      include_dir .. "/util/detail/*.*",
      include_dir .. "/core/*.*",
      include_dir .. "/graphics/*.*",
      include_dir .. "/core/detail/*.*",
      include_dir .. "/editor/*.*",
      shader_dir  .. "/*.*",
      third_party_dir .. "/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      third_party_dir .. "/rapidjson/*.*",
      third_party_dir .. "/imgui/*.*",
      third_party_dir .. "/imgui/backends/*.*",
      third_party_dir .. "/gl3w/GL/*.*",
      third_party_dir .. "/glfw/GLFW/*.*",
      third_party_dir .. "/tinymath/*.*",
      third_party_dir .. "/tinymath/detail/*.*",
      third_party_dir .. "/tinymath/primitives/*.*",
      third_party_dir .. "/tinymath/color/*.*",
      sample_dir .. "/Viewer/Viewer.cpp"
   }

   filter { "configurations:Debug*" }
      targetdir (solution_dir .. "/bin/Debug")

   filter { "configurations:Release*" }
      targetdir (solution_dir .. "/bin/release")
end

function setuoHelloTriangle()
   project "HelloTriangle"
   kind "ConsoleApp"
   language "C++"

   files { 
      src_dir .. "/*.*", 
      src_dir .. "/util/*.*",
      src_dir .. "/core/*.*",
      src_dir .. "/graphics/*.*",
      src_dir .. "/editor/*.*",
      include_dir .. "/*.*", 
      include_dir .. "/detail/*.*", 
      include_dir .. "/util/*.*",
      include_dir .. "/util/detail/*.*",
      include_dir .. "/core/*.*",
      include_dir .. "/graphics/*.*",
      include_dir .. "/core/detail/*.*",
      include_dir .. "/editor/*.*",
      shader_dir  .. "/*.*",
      third_party_dir .. "/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      third_party_dir .. "/rapidjson/*.*",
      third_party_dir .. "/imgui/*.*",
      third_party_dir .. "/imgui/backends/*.*",
      third_party_dir .. "/gl3w/GL/*.*",
      third_party_dir .. "/glfw/GLFW/*.*",
      third_party_dir .. "/tinymath/*.*",
      third_party_dir .. "/tinymath/detail/*.*",
      third_party_dir .. "/tinymath/primitives/*.*",
      third_party_dir .. "/tinymath/color/*.*",
      sample_dir .. "/HelloTriangle/HelloTriangle.cpp",
      sample_dir .. "/HelloTriangle/HelloTriangleShader.hpp"
   }

   filter { "configurations:Debug*" }
      targetdir (solution_dir .. "/bin/Debug")

   filter { "configurations:Release*" }
      targetdir (solution_dir .. "/bin/release")
end


function setupTextureProject()
   project "HelloTexture"
   kind "ConsoleApp"
   language "C++"

   files { 
      src_dir .. "/*.*", 
      src_dir .. "/util/*.*",
      src_dir .. "/core/*.*",
      src_dir .. "/graphics/*.*",
      src_dir .. "/editor/*.*",
      include_dir .. "/*.*", 
      include_dir .. "/detail/*.*", 
      include_dir .. "/util/*.*",
      include_dir .. "/util/detail/*.*",
      include_dir .. "/core/*.*",
      include_dir .. "/graphics/*.*",
      include_dir .. "/core/detail/*.*",
      include_dir .. "/editor/*.*",
      shader_dir  .. "/*.*",
      third_party_dir .. "/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      third_party_dir .. "/rapidjson/*.*",
      third_party_dir .. "/imgui/*.*",
      third_party_dir .. "/imgui/backends/*.*",
      third_party_dir .. "/gl3w/GL/*.*",
      third_party_dir .. "/glfw/GLFW/*.*",
      third_party_dir .. "/tinymath/*.*",
      third_party_dir .. "/tinymath/detail/*.*",
      third_party_dir .. "/tinymath/primitives/*.*",
      third_party_dir .. "/tinymath/color/*.*",
      sample_dir .. "/HelloTexture/HelloTexture.cpp",
      sample_dir .. "/HelloTexture/HelloTextureShader.hpp"
   }

   filter { "configurations:Debug*" }
      targetdir (solution_dir .. "/bin/Debug")

   filter { "configurations:Release*" }
      targetdir (solution_dir .. "/bin/release")
end



function setupTexture3DProject()
   project "HelloTexture3D"
   kind "ConsoleApp"
   language "C++"

   files { 
      src_dir .. "/*.*", 
      src_dir .. "/util/*.*",
      src_dir .. "/core/*.*",
      src_dir .. "/graphics/*.*",
      src_dir .. "/editor/*.*",
      include_dir .. "/*.*", 
      include_dir .. "/detail/*.*", 
      include_dir .. "/util/*.*",
      include_dir .. "/util/detail/*.*",
      include_dir .. "/core/*.*",
      include_dir .. "/graphics/*.*",
      include_dir .. "/core/detail/*.*",
      include_dir .. "/editor/*.*",
      shader_dir  .. "/*.*",
      third_party_dir .. "/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      third_party_dir .. "/rapidjson/*.*",
      third_party_dir .. "/imgui/*.*",
      third_party_dir .. "/imgui/backends/*.*",
      third_party_dir .. "/gl3w/GL/*.*",
      third_party_dir .. "/glfw/GLFW/*.*",
      third_party_dir .. "/tinymath/*.*",
      third_party_dir .. "/tinymath/detail/*.*",
      third_party_dir .. "/tinymath/primitives/*.*",
      third_party_dir .. "/tinymath/color/*.*",
      sample_dir .. "/HelloTexture3D/HelloTexture3D.cpp",
      sample_dir .. "/HelloTexture3D/HelloTexture3DShader.hpp"
   }

   filter { "configurations:Debug*" }
      targetdir (solution_dir .. "/bin/Debug")

   filter { "configurations:Release*" }
      targetdir (solution_dir .. "/bin/release")
end

function setupLightingProject()
   project "HelloLighting"
   kind "ConsoleApp"
   language "C++"

   files { 
      src_dir .. "/*.*", 
      src_dir .. "/util/*.*",
      src_dir .. "/core/*.*",
      src_dir .. "/graphics/*.*",
      src_dir .. "/editor/*.*",
      include_dir .. "/*.*", 
      include_dir .. "/detail/*.*", 
      include_dir .. "/util/*.*",
      include_dir .. "/util/detail/*.*",
      include_dir .. "/core/*.*",
      include_dir .. "/graphics/*.*",
      include_dir .. "/core/detail/*.*",
      include_dir .. "/editor/*.*",
      shader_dir  .. "/*.*",
      third_party_dir .. "/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      third_party_dir .. "/rapidjson/*.*",
      third_party_dir .. "/imgui/*.*",
      third_party_dir .. "/imgui/backends/*.*",
      third_party_dir .. "/gl3w/GL/*.*",
      third_party_dir .. "/glfw/GLFW/*.*",
      third_party_dir .. "/tinymath/*.*",
      third_party_dir .. "/tinymath/detail/*.*",
      third_party_dir .. "/tinymath/primitives/*.*",
      third_party_dir .. "/tinymath/color/*.*",
      sample_dir .. "/HelloLighting/HelloLighting.cpp",
      sample_dir .. "/HelloLighting/HelloLightingShader.hpp"
   }

   filter { "configurations:Debug*" }
      targetdir (solution_dir .. "/bin/Debug")

   filter { "configurations:Release*" }
      targetdir (solution_dir .. "/bin/release")
end

setupIncludeDirs()
setupSlotion()
setupViewerProject()
setuoHelloTriangle()
setupTextureProject()
setupTexture3DProject()
setupLightingProject()