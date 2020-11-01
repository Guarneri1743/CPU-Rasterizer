local solution_dir = _ACTION
local lib_dir = "lib"
local src_dir = "src"
local third_party_dir = "third_party"
local unit_test_dir = "unit_test"
local sample_dir = "sample"

function setupIncludeDirs()
   includedirs {
      src_dir,
      src_dir .. "/utility",
      src_dir .. "/math",
      src_dir .. "/primitives",
      third_party_dir,
      third_party_dir .. "/assimp",
      third_party_dir .. "/ply",
      third_party_dir .. "/stb_image",
   }
end

function setupSlotion()
   location(solution_dir)
   solution "SoftRasterizer"
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
         defines { "NDEBUG" }
         optimize "On"
         targetdir ( solution_dir .. "lib/Release"  )

      filter { "platforms:Win*", "configurations:Debug*" }
         defines { "WIN32", "_WINDOWS", "_CRT_SECURE_NO_WARNINGS" }
         system "Windows"
         architecture "x86_64"
         staticruntime "On"
         libdirs { lib_dir .. "/assimp/Debug" }

      filter { "platforms:Win*", "configurations:Release*" }
         defines { "WIN32", "_WINDOWS", "_CRT_SECURE_NO_WARNINGS" }
         system "Windows"
         architecture "x86_64"
         staticruntime "On"
         libdirs { lib_dir .. "/assimp/Release" }

      filter { "platforms:Win*", "configurations:Debug*" }
         links { "assimpd" }

      filter { "platforms:Win*", "configurations:Release*" }
         links { "assimp" }
end

function setupSoftRasterizerProj()
   project "SoftRasterizer"
   kind "ConsoleApp"
   language "C++"

   files { 
      src_dir .. "/*.*", 
      src_dir .. "/utility/*.*",
      src_dir .. "/math/*.*",
      src_dir .. "/primitives/*.*",
      third_party_dir .. "/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      sample_dir .. "/simple_scene/*.*"
   }

   filter { "configurations:Debug*" }
      targetdir "build/debug"

   filter { "configurations:Release*" }
      targetdir "build/release"
end

function setupPLYProj()
   project "StanfordPLY"
      kind "ConsoleApp"
      language "C++"

      files { 
         src_dir .. "/*.*", 
         src_dir .. "/utility/*.*",
         src_dir .. "/math/*.*",
         src_dir .. "/primitives/*.*",
         third_party_dir .. "/*.*",
         third_party_dir .. "/assimp/*.*",
         third_party_dir .. "/stb_image/*.*",
         third_party_dir .. "/ply/*.*",
         sample_dir .. "/ply/*.*"
      }

      filter { "configurations:Debug*" }
         targetdir "build/debug"

      filter { "configurations:Release*" }
         targetdir "build/release" 
end

function setupUnitTestProj()
   project "UnitTest"
   kind "ConsoleApp"
   language "C++"

   files { 
      src_dir, "*.*", 
      src_dir, "utility/*.*",
      src_dir, "math/*.*",
      src_dir, "primitives/*.*",
      third_party_dir .. "/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      sample_dir .. "/unit_test/*.*"
   }

   filter { "configurations:Debug*" }
      targetdir "build/debug"

   filter { "configurations:Release*" }
      targetdir "build/release"
end

setupIncludeDirs()
setupSlotion()
setupSoftRasterizerProj()
setupPLYProj()
setupUnitTestProj()