aux_source_directory(./ast _AST_SRC_)
target_sources(${_EXE_NAME_} PUBLIC ${_AST_SRC_})
aux_source_directory(./ir _IR_SRC_)
target_sources(${_EXE_NAME_} PUBLIC ${_IR_SRC_})
aux_source_directory(./assembly _ASM_SRC_)
target_sources(${_EXE_NAME_} PUBLIC ${_ASM_SRC_})
aux_source_directory(./optimizer _OPT_SRC_)
target_sources(${_EXE_NAME_} PUBLIC ${_OPT_SRC_})