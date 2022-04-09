#include "HashTable.hpp"


uint64_t xorHash(const char* str)
{
    uint64_t output = 0;
    size_t index = 0;

    while (str[index] != '\0')
    {
        output = CIRCULAR_SHIFT_RIGHT(output, index % 64) ^ str[index];
        index++;
    }

    return output;
}