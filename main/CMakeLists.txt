file(GLOB src_files "*.c" "*.cpp")
file(GLOB txt_files "*.bin.txt" "*.bin.html")

idf_component_register(
	SRCS ${src_files}
	INCLUDE_DIRS "."
	REQUIRES Exceptions Nvs Wifi HttpServer Gpio Spi
	EMBED_TXTFILES ${txt_files}
)

# Build static library, do not build test executables
option(BUILD_SHARED_LIBS OFF)
option(BUILD_TESTING OFF)

# Unfortunately the library performs install and export. Would 
# have been nice if devs made that an option like BUILD_SHARED_LIBS
# and BUILD_TESTING. Override install() and export() to do nothing
# instead.
function(install)
endfunction()

function(export)
endfunction()
