local solution_folder = _ACTION

solution "SoftRasterizer"
   configurations {
      "Debug", 
      "Release"
   }
   platforms { "Win64" }

   location (solution_folder)

   warnings "Extra"

   floatingpoint "Fast"

   symbols "On"
   cppdialect "C++17"

   rtti "On"

   characterset ("MBCS")

   configuration "Debug*"
      defines { "DEBUG", "_DEBUG" }
      targetdir ( solution_folder .. "/lib/Debug" )

   configuration "Release*"
      defines { "NDEBUG" }
      optimize "On"
      targetdir ( solution_folder .. "/lib/Release" )

   filter { "platforms:Win*" }
      defines { "WIN32", "_WINDOWS", "_CRT_SECURE_NO_WARNINGS" }
      system "Windows"
      architecture "x86_64"
      staticruntime "On"
      libdirs {  }
   filter { "platforms:Win*", "configurations:Debug*" }
      links { }
   filter { "platforms:Win*", "configurations:Release*" }
      links { }

   filter { }

includedirs {
   "src",
   "src/third_party",
   "src/utility",
   "src/math",
   "src/primitives"
}

project "SoftRasterizer"
   kind "ConsoleApp"
   language "C++"

   files { 
      "src/*.*", 
      "src/third_party/*.*",
      "src/utility/*.*",
      "src/math/*.*",
      "src/primitives/*.*",
      "samples/simple_scene/*.*"
   }

   filter { "configurations:Debug*" }
      targetdir "build/debug"

   filter { "configurations:Release*" }
      targetdir "build/release"


project "StanfordPLY"
   kind "ConsoleApp"
   language "C++"

   files { 
      "src/*.*", 
      "src/third_party/*.*",
      "src/utility/*.*",
      "src/math/*.*",
      "src/primitives/*.*",
      "samples/ply/*.*"
   }

   filter { "configurations:Debug*" }
      targetdir "build/debug"

   filter { "configurations:Release*" }
      targetdir "build/release" 


project "UnitTest"
   kind "ConsoleApp"
   language "C++"

   files { 
      "src/*.*",
      "src/third_party/*.*",
      "src/utility/*.*",
      "src/math/*.*",
      "src/primitives/*.*",
      "unit_test/*.*"
   }

   filter { "configurations:Debug*" }
      targetdir "build/debug"

   filter { "configurations:Release*" }
      targetdir "build/release"