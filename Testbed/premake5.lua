project "Testbed"
    language "C++"
    cppdialect "C++17"
    kind "ConsoleApp"
		
    targetdir 	("%{wks.location}/bin/%{prj.name}/" .. outputDir)
    objdir 		("%{wks.location}/obj/%{prj.name}/" .. outputDir)

    files 
    { 
        "**.h", 
        "**.cpp",
    }
    
    includedirs
    {
        "%{IncludeDir.LEntity}",
    }
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	links
	{
        "LEntity",
	}

    filter "configurations:Debug"
		runtime "Debug"
        symbols "on"
    filter "configurations:Release"
		runtime "Release"
        optimize "on"