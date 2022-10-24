project "LEntity"
    language "C++"
    cppdialect "C++17"
		
    targetdir 	("%{wks.location}/bin/%{prj.name}/" .. outputDir)
    objdir 		("%{wks.location}/obj/%{prj.name}/" .. outputDir)
	
	pchheader "lepch.h"
	pchsource "src/lepch.cpp"

    files 
    { 
        "src/**.h", 
        "src/**.cpp",
    }
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

    includedirs
    {
        "%{IncludeDir.LEntity}",
    }

	links
	{
	}
	
    filter "system:windows"
        kind "StaticLib"
        staticruntime "off"
        systemversion "latest"
		
	filter "system:linux"
        kind "SharedLib"
        staticruntime "off"
        defines "LE_SHARED"
        pic "On"
        systemversion "latest"

    filter "configurations:Debug"
        defines { "LE_DEBUG" }
		runtime "Debug"
        symbols "on"
    filter "configurations:Release"
        defines { "LE_RELEASE" }
		runtime "Release"
        optimize "on"