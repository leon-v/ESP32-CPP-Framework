file(GLOB src_files "*.c" "*.cpp")
file(GLOB txt_files "*.bin.txt")

idf_component_register(
	SRCS ${src_files}
	INCLUDE_DIRS "."
	REQUIRES json
	EMBED_TXTFILES ${txt_files}
)