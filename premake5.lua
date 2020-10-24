local proj_folder = _ACTION

solution "SoftRasterizer"
   configurations 
   {
      "Debug", 
      "Release"
   }
   platforms { "Win64", "linux64", "Mac64" }

   location (proj_folder)

   warnings "Extra"

   floatingpoint "Fast"

   symbols "On"
   cppdialect "C++17"

   rtti "On"

   characterset ("MBCS")

   configuration "Debug*"
      defines { "DEBUG", "_DEBUG" }
      targetdir ( proj_folder .. "/lib/Debug" )

   configuration "Release*"
      defines { "NDEBUG" }
      optimize "On"
      targetdir ( proj_folder .. "/lib/Release" )

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

   filter { "platforms:linux64" }
      architecture "x86_64"
      buildoptions {"-lstdc++fs"}
      links { }
      libdirs { }
      defines { "LINUX" }
   filter { "platforms:linux64", "configurations:Debug*" }
      links { "stdc++fs" }
   filter { "platforms:linux64", "configurations:Release*" }
      links { "stdc++fs" }

   filter {"platforms:Mac64", "configurations:Debug*"}
      architecture "x86_64"
      system "macosx"
      buildoptions {"-glldb"}
      libdirs { }
      links { }

   filter {"platforms:Mac64", "configurations:Release*"}
      architecture "x86_64"
      system "macosx"
      buildoptions {"-glldb"}
      libdirs { }
      links { }


   filter { }

   includedirs {
      "includes"
   }

project "SoftRasterizer"
   kind "ConsoleApp"
   language "C++"

   files 
   { 
      "includes/*.*",
      "src/*.*",
      "samples/*.*"
   }

   filter { "configurations:Debug*" }
      targetdir "build/debug"

   filter { "configurations:Release*" }
      targetdir "build/release"

project "UnitTest"
   kind "ConsoleApp"
   language "C++"

   files 
   { 
      "includes/*.*",
      "src/*.*",
      "unit_test/*.*"
   }

   filter { "configurations:Debug*" }
      targetdir "build/debug"

   filter { "configurations:Release*" }
      targetdir "build/release"