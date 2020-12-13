local solution_dir = _ACTION
local lib_dir = "Guarneri/lib"
local res_dir = "Guarneri/res"
local src_dir = "Guarneri/src"
local third_party_dir = "Guarneri/third_party"
local sample_dir = "sample"

function setupIncludeDirs()
   includedirs {
      src_dir,
      src_dir .. "/utility",
      src_dir .. "/math",
      src_dir .. "/primitives",
      third_party_dir,
      third_party_dir .. "/threading",
      third_party_dir .. "/assimp",
      third_party_dir .. "/ply",
      third_party_dir .. "/stb_image",
      third_party_dir .. "/nlohmann"
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
         staticruntime "Off"
         libdirs { lib_dir .. "/assimp/Debug" }
         local createResFolder = "mkdir \"$(OutDir)res\""
         local copyAssimp = "xcopy /y /d \"../" .. lib_dir .. "/assimp/Debug\" \"$(OutDir)\""
         local copyRes = "xcopy /y /d /s \"../" .. res_dir .. "\" \"$(OutDir)res\""
         postbuildcommands {
            createResFolder,
            copyAssimp,
            copyRes
         }

      filter { "platforms:Win*", "configurations:Release*" }
         defines { "WIN32", "_WINDOWS", "_CRT_SECURE_NO_WARNINGS" }
         system "Windows"
         architecture "x86_64"
         staticruntime "Off"
         libdirs { lib_dir .. "/assimp/Release" }
         local createResFolder = "mkdir \"$(OutDir)res\""
         local copyAssimp = "xcopy /y /d \"../" .. lib_dir .. "/assimp/Release\" \"$(OutDir)\""
         local copyRes = "xcopy /y /d /s \"../" .. res_dir .. "\" \"$(OutDir)res\""
         postbuildcommands {
            createResFolder,
            copyAssimp,
            copyRes
         }

      filter { "platforms:Win*", "configurations:Debug*" }
         links { "assimpd" }

      filter { "platforms:Win*", "configurations:Release*" }
         links { "assimp" }
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
      third_party_dir .. "/*.*",
      third_party_dir .. "/threading/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      third_party_dir .. "/nlohmann/*.*",
      sample_dir .. "/blending.cpp"
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
      third_party_dir .. "/*.*",
      third_party_dir .. "/threading/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      third_party_dir .. "/nlohmann/*.*",
      sample_dir .. "/stencil_test.cpp"
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
      third_party_dir .. "/*.*",
      third_party_dir .. "/threading/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      third_party_dir .. "/nlohmann/*.*",
      sample_dir .. "/lighting.cpp"
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
      third_party_dir .. "/*.*",
      third_party_dir .. "/threading/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      third_party_dir .. "/nlohmann/*.*",
      sample_dir .. "/filtering.cpp"
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
      third_party_dir .. "/*.*",
      third_party_dir .. "/threading/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      third_party_dir .. "/nlohmann/*.*",
      sample_dir .. "/cubemap.cpp"
   }

   filter { "configurations:Debug*" }
      targetdir (solution_dir .. "/bin/Debug")

   filter { "configurations:Release*" }
      targetdir (solution_dir .. "/bin/release")
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
         third_party_dir .. "/threading/*.*",
         third_party_dir .. "/assimp/*.*",
         third_party_dir .. "/stb_image/*.*",
         third_party_dir .. "/nlohmann/*.*",
         third_party_dir .. "/ply/*.*",
         sample_dir .. "/plytest.c"
      }

      filter { "configurations:Debug*" }
         targetdir (solution_dir .. "/bin/debug")

      filter { "configurations:Release*" }
         targetdir (solution_dir .. "/bin/release")
end

function setupUnitTestProj()
   project "UnitTest"
   kind "ConsoleApp"
   language "C++"

   files { 
      src_dir .. "/*.*", 
      src_dir .. "/utility/*.*",
      src_dir .. "/math/*.*",
      src_dir .. "/primitives/*.*",
      third_party_dir .. "/*.*",
      third_party_dir .. "/threading/*.*",
      third_party_dir .. "/assimp/*.*",
      third_party_dir .. "/stb_image/*.*",
      third_party_dir .. "/nlohmann/*.*",
      sample_dir .. "/unit_test.cpp"
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
--setupPLYProj()
setupUnitTestProj()