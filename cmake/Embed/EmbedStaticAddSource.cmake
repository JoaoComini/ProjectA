
file(READ ${SOURCE} hex_content HEX)

string(REPEAT "[0-9a-f]" 32 column_pattern)
string(REGEX REPLACE "(${column_pattern})" "\\1\n" content "${hex_content}")
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " content "${content}")
string(REGEX REPLACE ", $" "" content "${content}")

string(CONFIGURE [[
    // Auto generated file.

    namespace embed {
        namespace ${NAMESPACE} {
            static constexpr const char ${SYMBOL}_array[] = {
                ${content}
            }\;

            extern const char* const ${SYMBOL}_begin = ${SYMBOL}_array\;
            extern const char* const ${SYMBOL}_end = ${SYMBOL}_array + sizeof(${SYMBOL}_array)\;
        }
    }
]] code)

file(WRITE ${DESTINATION} ${code})
    