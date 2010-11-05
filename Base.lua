Helium = {}

Helium.Sleep = function( seconds )
	if os.get() == "windows" then
		os.execute("ping 127.0.0.1 -n " .. seconds + 1 .. " -w 1000 >:nul 2>&1")
	else
		os.execute("sleep " .. seconds)
	end
end

Helium.Publish = function( files )
	for i,v in pairs(files) do
		-- mkpath the target folder
		os.mkdir( v.target )
		
		local path = v.source .. "/" .. v.file			
		local exists = os.isfile( path )
		local destination = v.target .. "/" .. v.file
		
		-- cull existing files
		if os.isfile( destination ) then
			local delCommand = "del /q \"" .. string.gsub( destination, "/", "\\" ) .. "\""
			os.execute( delCommand )
		end

		-- do the file copy
		local linkCommand = "mklink /h \"" .. destination .. "\" \"" .. path .. "\""
		local result = os.execute( linkCommand )

		-- the files were copied, complete this entry
		if result == 0 then
			files[ i ] = nil
		else
			os.exit( 1 )
		end						
	end
end

Helium.DoDefaultSolutionSettings = function()

	location "Premake"

	platforms
	{
		"x32",
		"x64",
	}

	configurations
	{
		"Debug",
		"Intermediate",
		"Profile",
		"Release",
	}
	
	configuration {}
		defines
		{
			"XML_STATIC=1",
		}
		flags
		{
			"EnableSSE2",
			"NoMinimalRebuild",
		}
	
	configuration "not no-unicode"
		defines
		{
			"UNICODE=1",
			"LITESQL_UNICODE=1",
			"XML_UNICODE_WCHAR_T=1",
		}
		flags
		{
			"Unicode",
		}
	
	for i, platform in ipairs( platforms() ) do
		for j, config in ipairs( configurations() ) do
			configuration( { config, platform } )
				objdir( "Intermediate" ) -- premake seems to automatically add the platform and config name
			configuration( { config, platform } )
				targetdir( "Bin/" .. platform .. "/" .. config )
		end
	end

	configuration "windows"
		defines
		{
			"_WIN32",
			"WIN32",
			"_CRT_SECURE_NO_DEPRECATE",
		}

	configuration "Debug"
		defines
		{
			"_DEBUG",
		}
		flags
		{
			"Symbols",
		}
		
	configuration "Intermediate"
		flags
		{
			"OptimizeSpeed",
			"Symbols",
		}
		
	configuration "Profile"
		defines
		{
			"NDEBUG",
		}
		flags
		{
			"NoFramePointer",
			"OptimizeSpeed",
		}

	configuration "Release"
		defines
		{
			"NDEBUG",
		}
		flags
		{
			"NoFramePointer",
			"OptimizeSpeed",
		}

	configuration { "windows", "Debug" }
		buildoptions
		{
			"/Ob0",
		}

	configuration { "windows", "not Debug" }
		buildoptions
		{
			"/Ob2",
			"/Oi",
		}

end

-- Common settings for projects linking with Lunar libraries.
Helium.DoDefaultLunarProjectSettings = function()

	language "C++"

	flags
	{
		"ExtraWarnings",
		"FatalWarnings",
		"FloatFast",  -- Should be used in all configurations to ensure data consistency.
		"NoRTTI",
	}

	includedirs
	{
		"Dependencies/boost",
		"Dependencies/tbb/include",
	}

	configuration "no-unicode"
		defines
		{
			"L_UNICODE=0",
		}

	configuration "not no-unicode"
		defines
		{
			"L_UNICODE=1",
		}

	configuration "Debug"
		defines
		{
			"L_DEBUG=1",
			"L_EDITOR=1",
			"L_SHARED=1",
			"TBB_USE_DEBUG=1",
		}

	configuration "Intermediate"
		defines
		{
			"L_INTERMEDIATE=1",
			"L_EDITOR=1",
			"L_STATIC=1",
		}

	configuration "Profile"
		defines
		{
			"L_PROFILE=1",
			"L_STATIC=1",
		}

	configuration "Release"
		defines
		{
			"L_RELEASE=1",
			"L_STATIC=1",
		}

	configuration { "windows", "Debug" }
		links
		{
			"dbghelp",
		}

	configuration { "windows", "x32", "Debug" }
		libdirs
		{
			"Dependencies/tbb/build/windows_ia32_cl_vc9_debug",
		}

	configuration { "windows", "x32", "not Debug" }
		libdirs
		{
			"Dependencies/tbb/build/windows_ia32_cl_vc9_release",
		}

	configuration { "windows", "x64", "Debug" }
		libdirs
		{
			"Dependencies/tbb/build/windows_intel64_cl_vc9_debug",
		}

	configuration { "windows", "x64", "not Debug" }
		libdirs
		{
			"Dependencies/tbb/build/windows_intel64_cl_vc9_release",
		}

end

-- Common settings for Lunar modules.
Helium.DoLunarModuleProjectSettings = function( tokenPrefix, moduleName, moduleNameUpper )

	defines
	{
		"L_MODULE_HEAP_FUNCTION=Get" .. moduleName .. "DefaultHeap"
	}

	files
	{
		moduleName .. "/*",
	}

	pchheader( moduleName .. "Pch.h" )
	pchsource( moduleName .. "/" .. moduleName .. "Pch.cpp" )

	Helium.DoDefaultLunarProjectSettings()

	configuration "Debug"
		kind "SharedLib"
		defines
		{
			tokenPrefix .. "_" .. moduleNameUpper .. "_EXPORTS",
		}

	configuration "not Debug"
		kind "StaticLib"

end