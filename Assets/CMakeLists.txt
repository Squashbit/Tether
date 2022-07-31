cmake_minimum_required (VERSION 3.8)

# Definitions
set(ASSETS_BIN_DIR Bin)
set(COMPILED_ASSETS_DIR ${ASSETS_BIN_DIR}/CompiledAssets)
set(VK_SHADER_BINARY_DIR ${ASSETS_BIN_DIR}/Shaders)
set(VK_SHADER_SOURCE_DIR Shaders)

# Valid definitions are:
# VULKAN_API
# OPENGL_API
add_definitions(-DVULKAN_API)

# Compile vulkan shaders
# Only if VULKAN_API is defined above
get_directory_property(COMPDEFS COMPILE_DEFINITIONS)
if (COMPDEFS MATCHES "^VULKAN_API")
	
	# Compile shaders

	# GLSLC is required for Vulkan shader compilation
	find_program(glslc_executable NAMES glslc)
	find_package(Vulkan REQUIRED)

	file(GLOB_RECURSE SHADERS
		${VK_SHADER_SOURCE_DIR}/*.vert
		${VK_SHADER_SOURCE_DIR}/*.frag
	)
	
	# Make the shader directory
	set (SHADER_BINARY_DIR 
		"\"${CMAKE_CURRENT_LIST_DIR}/${VK_SHADER_BINARY_DIR}\""
	)

	if (WIN32)
		add_custom_command(
			COMMAND
				IF not exist ${SHADER_BINARY_DIR} ( mkdir ${SHADER_BINARY_DIR} )
			OUTPUT shaderdir
			COMMENT "Creating compiled shaders directory"
		)
	elseif(UNIX)
		add_custom_command(
			COMMAND
				mkdir -p ${SHADER_BINARY_DIR}
			OUTPUT shaderdir
			COMMENT "Creating compiled shaders directory"
		)
	endif()

	foreach(source IN LISTS SHADERS)
		# Get the name of the file without path or extention
		get_filename_component(FILENAME ${source} NAME)

		# Run glslc for each shader
		add_custom_command(
			COMMAND
				${glslc_executable}
				# Output file
				-o ${CMAKE_CURRENT_LIST_DIR}/${VK_SHADER_BINARY_DIR}/${FILENAME}.spv
				# Source file
				${source}
			OUTPUT ${FILENAME}.spv
			COMMENT "Compiling shader: ${FILENAME}"
		)

		list(APPEND SPV_SHADERS ${FILENAME}.spv)
	endforeach()

	add_custom_target(vkshaders ALL DEPENDS ${SPV_SHADERS} shaderdir)
endif()
