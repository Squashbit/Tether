cmake_minimum_required (VERSION 3.8)

# Relative to the build directory
set(SHADER_BINARY_DIR Shaders)

# Relative to the list directory
set(SHADER_SOURCE_DIR Shaders)
set(COMPILED_ASSETS_DIR Compiled/Assets)
set(BIN2CPP_UNIX_DIR Compiled/bin2cpp)
set(BIN2CPP_WINDOWS_DIR Compiled/bin2cpp.exe)

# The absolute path to the compiled assets directory
# Build/-Configuration-/Tether/Assets/-COMPILED_ASSETS_DIR-
set (ABSOLUTE_COMPILED_DIR
	"\"${CMAKE_CURRENT_BINARY_DIR}/${COMPILED_ASSETS_DIR}\""
)

# The absolute path to the shader binary directory
# Build/-Configuration-/Tether/Assets/-SHADER_BINARY_DIR-
set (ABSOLUTE_SHADER_DIR
	"\"${CMAKE_CURRENT_BINARY_DIR}/${SHADER_BINARY_DIR}\""
)

if (TETHER_VULKAN_API)
	
	# Compile shaders

	# GLSLC is required for Vulkan shader compilation
	find_program(glslc_executable NAMES glslc)
	find_package(Vulkan REQUIRED)

	# Find all shader files
	file(GLOB_RECURSE SHADERS
		${SHADER_SOURCE_DIR}/*.vert
		${SHADER_SOURCE_DIR}/*.frag
	)

	# Make the shader bin directory (if it doesn't exist)
	if (WIN32)
		add_custom_command(
			COMMAND
				IF not exist ${ABSOLUTE_SHADER_DIR} ( mkdir ${ABSOLUTE_SHADER_DIR} )
			OUTPUT tetherShaderdir
			COMMENT "Creating compiled shaders directory"
		)
	elseif(UNIX)
		add_custom_command(
			COMMAND
				mkdir -p ${ABSOLUTE_SHADER_DIR}
			OUTPUT tetherShaderdir
			COMMENT "Creating compiled shaders directory"
		)
	endif()

	# Compile all shaders with glslc
	foreach(source IN LISTS SHADERS)
		# Get the name of the file without path (with extension)
		get_filename_component(FILENAME ${source} NAME)

		# Run glslc for each shader
		add_custom_command(
			COMMAND
				${glslc_executable}
				# Output file
				-o ${ABSOLUTE_SHADER_DIR}/${FILENAME}.spv
				# Source file
				${source}
			OUTPUT ${FILENAME}.spv
			COMMENT "Compiling shader: ${FILENAME}"
		)

		list(APPEND SPV_SHADERS ${FILENAME}.spv)
	endforeach()

	add_custom_target(compileTetherVKshaders ALL DEPENDS ${SPV_SHADERS} 
		tetherShaderdir)
endif()

# ----- END SHADERS COMPILATION  -----
# ----- BEGIN ASSETS COMPILATION -----

# Find all assets
file(GLOB_RECURSE ASSETS
	*.*
)

# Remove the CMake file
list(REMOVE_ITEM ASSETS CMakeLists.txt)
# TODO: Remove the compiled assets directory

add_custom_target(compileTetherAssets)
# Add vulkan shaders dependency
if (COMPDEFS MATCHES "^VULKAN_API")
	add_dependencies(compileTetherAssets compileTetherVKshaders)
endif()

message(STATUS "Compiler for assets is \"${CMAKE_CXX_COMPILER_ID}\"")

# Compile assets
foreach(source IN LISTS ASSETS)
	# Get the name of the file without path (with extension)
	get_filename_component(FILENAME ${source} NAME)

	string(REPLACE "." "_" ARRAY_NAME "${FILENAME}")
	
	# Potential issue: There may be multiple assets with the same name so they
	# should be stored in their folders relative to the Assets folder
	set(OUTPUT_FILE "\"${ABSOLUTE_COMPILED_DIR}/${FILENAME}.o\"")

	#add_custom_command(
	#	TARGET compileTetherAssets 
	#	COMMAND
	#		# TODO: Assets command
	#	COMMENT "Compiling asset: ${OUTPUT_FILE}"
	#)
endforeach()

