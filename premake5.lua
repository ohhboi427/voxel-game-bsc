workspace "voxel-game"
	architecture "x86_64"
	configurations { "Debug", "Release" }

project "voxel-game"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"

	files {
		"src/**.cpp",
		"src/**.h",
	}

	includedirs {
		"vendor/glad/include",
		"vendor/glfw/include",
		"vendor/imgui/include",
		"vendor/glm/include",
		"vendor/toml++/include",
		"vendor/fastnoiselite/include",
	}

	libdirs {
		"vendor/glad/lib",
		"vendor/glfw/lib",
		"vendor/imgui/lib",
	}

	links {
		"opengl32.lib",
		"glad.lib",
		"glfw3.lib",
		"imgui.lib",
	}

	targetdir "bin"
	objdir "obj/%{cfg.buildcfg}"

	filter "configurations:Debug"
		ignoredefaultlibraries { "MSVCRT" }
		targetname "%{prj.name}d"
		optimize "off"
		symbols "on"

	filter "configurations:Release"
		optimize "on"
		symbols "off"
