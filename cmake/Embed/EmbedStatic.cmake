
set(_EMBED_STATIC_SCRIPT_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "Path to EmbedStatic script directory")
set(_EMBED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/embed/include" CACHE INTERNAL "Path to EmbedStatic include directory")

add_custom_command(
    COMMAND ${CMAKE_COMMAND}
        -E copy_if_different "${_EMBED_STATIC_SCRIPT_DIR}/embed.hpp" "${_EMBED_INCLUDE_DIR}/embed/embed.hpp"
    OUTPUT "${_EMBED_INCLUDE_DIR}/embed/embed.hpp"
    DEPENDS "${_EMBED_STATIC_SCRIPT_DIR}/embed.hpp"
)

add_library(embed-base INTERFACE)
target_include_directories(embed-base INTERFACE ${_EMBED_INCLUDE_DIR})
target_sources(embed-base PUBLIC "${_EMBED_INCLUDE_DIR}/embed/embed.hpp")

function(embed_static name)
    cmake_parse_arguments(
        ARGS
        ""
        ""
        ""
        ${ARGN}
    )

    add_library(${name} STATIC)

    foreach(static_file IN LISTS ARGS_UNPARSED_ARGUMENTS)
        get_filename_component(absolute_path ${static_file} ABSOLUTE)

        _embed_static_add_source(${name} ${absolute_path})
    endforeach()

    _embed_static_gen_source(${name})
    _embed_static_gen_header(${name})

    target_include_directories(${name} PUBLIC ${_EMBED_INCLUDE_DIR})
    target_link_libraries(${name} embed-base)
endfunction()


function(_embed_static_add_source name source)
    get_filename_component(file_name ${source} NAME)

    set(destination "${CMAKE_CURRENT_BINARY_DIR}/Intermediate/${name}/${file_name}.embed.cpp")

    string(MAKE_C_IDENTIFIER ${file_name} symbol)

    add_custom_command(
        COMMAND ${CMAKE_COMMAND}
            -D SOURCE=${source}
            -D DESTINATION=${destination}
            -D NAMESPACE=${name}
            -D SYMBOL=${symbol}
            -P "${_EMBED_STATIC_SCRIPT_DIR}/EmbedStaticAddSource.cmake"
        OUTPUT ${destination}
        DEPENDS ${source}
    )

    set_property(
        TARGET ${name}
        APPEND PROPERTY EMBED_EXTERNS
        "extern const unsigned char* const ${symbol}_begin\;"
        "extern const unsigned char* const ${symbol}_end\;"
    )

    set_property(
        TARGET ${name}
        APPEND PROPERTY EMBED_FILES
        ${destination}
    )

    set_property(
        TARGET ${name}
        APPEND PROPERTY EMBED_FILE_DATA
        "{ \"${file_name}\", file_data{${symbol}_begin, ${symbol}_end} },"
    )

    target_sources(${name} PUBLIC ${destination})
endfunction()

function(_embed_static_gen_source name)
    set(destination "${CMAKE_CURRENT_BINARY_DIR}/Intermediate/${name}/embed.gen.cpp")
    
    string(CONFIGURE [[
        // Auto generated file.

        #include <embed/embed.hpp>

        namespace embed {

            namespace ${name} {

                $<JOIN:$<TARGET_PROPERTY:${name},EMBED_EXTERNS>,
                >

                std::unordered_map<std::string, file_data> index = {
                    $<JOIN:$<TARGET_PROPERTY:${name},EMBED_FILE_DATA>,
                    >
                }\;

                file get(const std::string& name)
                {
                    if (auto it = index.find(name)\; it != index.end())
                    {
                        return file{ it->second.begin, it->second.end }\;
                    }

                    return file{}\;
                }
            }
        }
    ]] embed_source)

    file(GENERATE OUTPUT "${destination}.tmp" CONTENT ${embed_source})

    add_custom_command(
        COMMAND ${CMAKE_COMMAND}
            -E copy_if_different "${destination}.tmp" ${destination}
        OUTPUT ${destination}
        DEPENDS "${destination}.tmp" $<TARGET_PROPERTY:${name},EMBED_FILES>
    )

    target_sources(${name} PRIVATE ${destination})
endfunction()

function(_embed_static_gen_header name)
    set(final_destination "${_EMBED_INCLUDE_DIR}/${name}/embed.gen.hpp")
    set(temp_destination "${CMAKE_CURRENT_BINARY_DIR}/Intermediate/${name}/embed.gen.hpp.tmp")
    
    string(CONFIGURE [[
        // Auto generated file.

        #pragma once

        #include <embed/embed.hpp>

        namespace embed {

            namespace ${name} {
                file get(const std::string& name)\;
            }
        }
    ]] embed_header)

    file(CONFIGURE OUTPUT ${temp_destination} CONTENT ${embed_header})

    add_custom_command(
        COMMAND ${CMAKE_COMMAND}
            -E copy_if_different ${temp_destination} ${final_destination}
        OUTPUT ${final_destination}
        DEPENDS ${temp_destination} $<TARGET_PROPERTY:${name},EMBED_FILES>
    )

    target_sources(${name} PUBLIC ${final_destination})
endfunction()