local solution_dir = "build/" .. _ACTION
local lib_dir = "lib"
local res_dir = "res"
local src_dir = "src"
local include_dir = "include"
local third_party_dir = "third_party"
local sample_dir = "samples"

function setupIncludeDirs()
   includedirs {
      include_dir,
      include_dir .. "/utility",
      include_dir .. "/math",
      include_dir .. "/primitives",
      include_dir .. "/core",
      include_dir .. "/shader",
      include_dir .. "/editor",
      third_party_dir,
      third_party_dir .. "/threading",
      third_party_dir .. "/assimp",
      third_party_dir .. "/ply",
      third_party_dir .. "/stb_image",
      third_party_dir .. "/rapidjson",
      third_party_dir .. "/imgui"
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
         optimize "On"
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
end

function setupBlendingDemo()
   project "BlendingDemo"
   kind "ConsoleApp"
   language "C++"

   files { 
      src_dir .. "/*.*", 
      src_dir .. "/utility/*.*",
      src_dir .. "/math/*.*",
      src_dir .. "/primitives/*.*",
      src_dir .. "/core/*.*",
      src_dir .. "/shader/*.*",
      src_dir .. "/editor/*.*",
      include_dir .. "/*.*", 
      include_dir .. "/detail/*.*", 
      include_dir .. "/utility/*.*",
      include_dir .. "/math/*.*",
      include_dir .. "/primitives/*.*",
      include_dir .. "/core/*.*",
      include_dir .. "/core/detail/*.*",
      include_dir .. "/shader/*.*",
      include_dir .. "/editor/*.*",
      third_party_dir .. "/*.*",
      third_party_dir .. "/threading/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      third_party_dir .. "/rapidjson/*.*",
      third_party_dir .. "/imgui/*.*",
      third_party_dir .. "/imgui/backends/*.*",
      third_party_dir .. "/imgui/backends/GL/*.*",
      sample_dir .. "/BlendingSample.cpp"
   }

   filter { "configurations:Debug*" }
      targetdir (solution_dir .. "/bin/Debug")

   filter { "configurations:Release*" }
      targetdir (solution_dir .. "/bin/release")
end

function setupStencilDemo()
   project "StencilDemo"
   kind "ConsoleApp"
   language "C++"

   files { 
      src_dir .. "/*.*", 
      src_dir .. "/utility/*.*",
      src_dir .. "/math/*.*",
      src_dir .. "/primitives/*.*",
      src_dir .. "/core/*.*",
      src_dir .. "/shader/*.*",
      src_dir .. "/editor/*.*",
      include_dir .. "/*.*", 
      include_dir .. "/detail/*.*", 
      include_dir .. "/utility/*.*",
      include_dir .. "/math/*.*",
      include_dir .. "/primitives/*.*",
      include_dir .. "/core/*.*",
      include_dir .. "/core/detail/*.*",
      include_dir .. "/shader/*.*",
      include_dir .. "/editor/*.*",
      third_party_dir .. "/*.*",
      third_party_dir .. "/threading/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      third_party_dir .. "/rapidjson/*.*",
      third_party_dir .. "/imgui/*.*",
      third_party_dir .. "/imgui/backends/*.*",
      third_party_dir .. "/imgui/backends/GL/*.*",
      third_party_dir .. "/glfw/*.*",
      sample_dir .. "/StencilSample.cpp"
   }

   filter { "configurations:Debug*" }
      targetdir (solution_dir .. "/bin/Debug")

   filter { "configurations:Release*" }
      targetdir (solution_dir .. "/bin/release")
end

function setupLightingDemo()
   project "LightingDemo"
   kind "ConsoleApp"
   language "C++"

   files { 
      src_dir .. "/*.*", 
      src_dir .. "/utility/*.*",
      src_dir .. "/math/*.*",
      src_dir .. "/primitives/*.*",
      src_dir .. "/core/*.*",
      src_dir .. "/shader/*.*",
      src_dir .. "/editor/*.*",
      include_dir .. "/*.*", 
      include_dir .. "/detail/*.*", 
      include_dir .. "/utility/*.*",
      include_dir .. "/math/*.*",
      include_dir .. "/primitives/*.*",
      include_dir .. "/core/*.*",
      include_dir .. "/core/detail/*.*",
      include_dir .. "/shader/*.*",
      include_dir .. "/editor/*.*",
      third_party_dir .. "/*.*",
      third_party_dir .. "/threading/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      third_party_dir .. "/rapidjson/*.*",
      third_party_dir .. "/imgui/*.*",
      third_party_dir .. "/imgui/backends/*.*",
      third_party_dir .. "/imgui/backends/GL/*.*",
      third_party_dir .. "/glfw/*.*",
      sample_dir .. "/PBRSample.cpp"
   }

   filter { "configurations:Debug*" }
      targetdir (solution_dir .. "/bin/Debug")

   filter { "configurations:Release*" }
      targetdir (solution_dir .. "/bin/release")
end

function setupFilteringDemo()
   project "FilteringDemo"
   kind "ConsoleApp"
   language "C++"

   files { 
      src_dir .. "/*.*", 
      src_dir .. "/utility/*.*",
      src_dir .. "/math/*.*",
      src_dir .. "/primitives/*.*",
      src_dir .. "/core/*.*",
      src_dir .. "/shader/*.*",
      src_dir .. "/editor/*.*",
      include_dir .. "/*.*", 
      include_dir .. "/detail/*.*", 
      include_dir .. "/utility/*.*",
      include_dir .. "/math/*.*",
      include_dir .. "/primitives/*.*",
      include_dir .. "/core/*.*",
      include_dir .. "/core/detail/*.*",
      include_dir .. "/shader/*.*",
      include_dir .. "/editor/*.*",
      third_party_dir .. "/*.*",
      third_party_dir .. "/threading/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      third_party_dir .. "/rapidjson/*.*",
      third_party_dir .. "/imgui/*.*",
      third_party_dir .. "/imgui/backends/*.*",
      third_party_dir .. "/imgui/backends/GL/*.*",
      third_party_dir .. "/glfw/*.*",
      sample_dir .. "/FilteringSample.cpp"
   }

   filter { "configurations:Debug*" }
      targetdir (solution_dir .. "/bin/Debug")

   filter { "configurations:Release*" }
      targetdir (solution_dir .. "/bin/release")
end


function setupCubemapDemo()
   project "CubemapDemo"
   kind "ConsoleApp"
   language "C++"

   files { 
      src_dir .. "/*.*", 
      src_dir .. "/utility/*.*",
      src_dir .. "/math/*.*",
      src_dir .. "/primitives/*.*",
      src_dir .. "/core/*.*",
      src_dir .. "/shader/*.*",
      src_dir .. "/editor/*.*",
      include_dir .. "/*.*", 
      include_dir .. "/detail/*.*", 
      include_dir .. "/utility/*.*",
      include_dir .. "/math/*.*",
      include_dir .. "/primitives/*.*",
      include_dir .. "/core/*.*",
      include_dir .. "/core/detail/*.*",
      include_dir .. "/shader/*.*",
      include_dir .. "/editor/*.*",
      third_party_dir .. "/*.*",
      third_party_dir .. "/threading/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      third_party_dir .. "/rapidjson/*.*",
      third_party_dir .. "/imgui/*.*",
      third_party_dir .. "/imgui/backends/*.*",
      third_party_dir .. "/imgui/backends/GL/*.*",
      third_party_dir .. "/glfw/*.*",
      sample_dir .. "/CubemapSample.cpp"
   }

   filter { "configurations:Debug*" }
      targetdir (solution_dir .. "/bin/Debug")

   filter { "configurations:Release*" }
      targetdir (solution_dir .. "/bin/release")
end

function setupGenerateScenesProj()
   project "GenerateScenes"
   kind "ConsoleApp"
   language "C++"

   files { 
      src_dir .. "/*.*", 
      src_dir .. "/utility/*.*",
      src_dir .. "/math/*.*",
      src_dir .. "/primitives/*.*",
      src_dir .. "/core/*.*",
      src_dir .. "/shader/*.*",
      src_dir .. "/editor/*.*",
      include_dir .. "/*.*", 
      include_dir .. "/detail/*.*", 
      include_dir .. "/utility/*.*",
      include_dir .. "/math/*.*",
      include_dir .. "/primitives/*.*",
      include_dir .. "/core/*.*",
      include_dir .. "/core/detail/*.*",
      include_dir .. "/shader/*.*",
      include_dir .. "/editor/*.*",
      third_party_dir .. "/*.*",
      third_party_dir .. "/threading/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      third_party_dir .. "/rapidjson/*.*",
      third_party_dir .. "/imgui/*.*",
      third_party_dir .. "/imgui/backends/*.*",
      third_party_dir .. "/imgui/backends/GL/*.*",
      third_party_dir .. "/glfw/*.*",
      sample_dir .. "/GenerateScenes.cpp"
   }

   filter { "configurations:Debug*" }
      targetdir (solution_dir .. "/bin/debug")

   filter { "configurations:Release*" }
      targetdir (solution_dir .. "/bin/release")
end

setupIncludeDirs()
setupSlotion()
setupBlendingDemo()
setupStencilDemo()
setupLightingDemo()
setupFilteringDemo()
setupCubemapDemo()
setupGenerateScenesProj()