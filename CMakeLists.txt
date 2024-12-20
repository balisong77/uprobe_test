cmake_minimum_required(VERSION 3.16)
project(utest)

# 生成 compile_commands.json 文件
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# utest exec
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -ggdb -rdynamic")  # 带符号
# utest exec
add_executable(utest utest.c)

# uprobe exec
add_executable(uprobe uprobe.c)
target_link_libraries(uprobe bpf)

# 查找所有 .bpf.c 文件
file(GLOB BPF_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.bpf.c")

# 为每个 BPF 程序创建编译和生成 skeleton 的命令
foreach(BPF_SOURCE ${BPF_SOURCES})
    # 获取文件名(不含扩展名)
    get_filename_component(BPF_NAME ${BPF_SOURCE} NAME_WE)
    
    # 编译 eBPF 程序
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${BPF_NAME}.bpf.o
        COMMAND clang -g -O2 -target bpf -D__TARGET_ARCH_x86 
                -I/usr/include/x86_64-linux-gnu -I${CMAKE_CURRENT_SOURCE_DIR}
                -c ${BPF_SOURCE} -o ${CMAKE_CURRENT_BINARY_DIR}/${BPF_NAME}.bpf.o
        DEPENDS ${BPF_SOURCE}
        VERBATIM
    )

    # 创建 skel 目录
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/skel)
    
    # 生成 skeleton 头文件到 skel 目录
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/skel/${BPF_NAME}.skel.h
        COMMAND bpftool gen skeleton ${CMAKE_CURRENT_BINARY_DIR}/${BPF_NAME}.bpf.o 
                > ${CMAKE_CURRENT_SOURCE_DIR}/skel/${BPF_NAME}.skel.h
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${BPF_NAME}.bpf.o
        VERBATIM
    )

    # 将生成的文件添加到列表中
    list(APPEND BPF_SKELS ${CMAKE_CURRENT_SOURCE_DIR}/skel/${BPF_NAME}.skel.h)
endforeach()

# 添加自定义目标,确保在构建时生成所有 skeleton 文件
add_custom_target(bpf_skel ALL DEPENDS ${BPF_SKELS})

# 确保 uprobe 目标依赖于 bpf_skel
add_dependencies(uprobe bpf_skel)