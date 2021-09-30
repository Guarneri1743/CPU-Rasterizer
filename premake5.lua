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
      include_dir .. "/core",
      include_dir .. "/editor",
      include_dir .. "/graphics",
      include_dir .. "/math",
      include_dir .. "/primitives",
      include_dir .. "/shader",
      include_dir .. "/utility",
      third_party_dir,
      third_party_dir .. "/assimp",
      third_party_dir .. "/ply",
      third_party_dir .. "/stb_image",
      third_party_dir .. "/rapidjson",
      third_party_dir .. "/imgui",
      third_party_dir .. "/glfw",
      third_party_dir .. "/gl3w"
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
end

function setupProject()
   project "CPURasterizer"
   kind "ConsoleApp"
   language "C++"

   files { 
      src_dir .. "/*.*", 
      src_dir .. "/utility/*.*",
      src_dir .. "/math/*.*",
      src_dir .. "/primitives/*.*",
      src_dir .. "/core/*.*",
      src_dir .. "/graphics/*.*",
      src_dir .. "/shader/*.*",
      src_dir .. "/editor/*.*",
      include_dir .. "/*.*", 
      include_dir .. "/detail/*.*", 
      include_dir .. "/utility/*.*",
      include_dir .. "/utility/detail/*.*",
      include_dir .. "/math/*.*",
      include_dir .. "/primitives/*.*",
      include_dir .. "/core/*.*",
      include_dir .. "/graphics/*.*",
      include_dir .. "/core/detail/*.*",
      include_dir .. "/shader/*.*",
      include_dir .. "/editor/*.*",
      third_party_dir .. "/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      third_party_dir .. "/rapidjson/*.*",
      third_party_dir .. "/imgui/*.*",
      third_party_dir .. "/imgui/backends/*.*",
      third_party_dir .. "/gl3w/GL/*.*",
      third_party_dir .. "/glfw/GLFW/*.*",
      sample_dir .. "/Editor.cpp"
   }

   filter { "configurations:Debug*" }
      targetdir (solution_dir .. "/bin/Debug")

   filter { "configurations:Release*" }
      targetdir (solution_dir .. "/bin/release")
end

setupIncludeDirs()
setupSlotion()
setupProject()