#pragma once

#include <string>
#include <vector>
#include <optional>

#define CIRCULAR_SHIFT_RIGHT(x, s) (x << s | x >> (64 - s))

uint64_t xorHash(const char* str);

template<typename T>
struct HashTable
{
public:
    void push(std::string_view ident, const T& value)
    {
        uint64_t hash = xorHash(ident.data());

        for (size_t bucketInd = 0; bucketInd < m_buckets.size(); bucketInd++)
        {
            if (m_buckets[bucketInd].hash == hash)
            {
                m_buckets[bucketInd].strings.push_back(ident);
                m_buckets[bucketInd].value.push_back(value);
                return;
            }
            else if (m_buckets[bucketInd].hash > hash)
            {
                Bucket bucket;
                bucket.hash = hash;
                bucket.strings.push_back(ident);
                bucket.value.push_back(value);

                m_buckets.insert(m_buckets.begin() + bucketInd, bucket);

                return;
            }
        }

        Bucket bucket;
        bucket.hash = hash;
        bucket.strings.push_back(ident);
        bucket.value.push_back(value);

        m_buckets.push_back(bucket);
    }

    T& operator[](std::string_view ident)
    {
        uint64_t hash = xorHash(ident.data());

        for (Bucket& bucket : m_buckets)
        {
            if (bucket.hash == hash)
            {
                if (bucket.strings.size() > 1)
                {
                    for (size_t itemInd = 0; itemInd < bucket.strings.size(); itemInd++)
                    {
                        if (bucket.strings[itemInd] == ident)
                        {
                            return bucket.value[itemInd];
                        }
                    }
                    printf("Bucket with hash %llu exists, it doesn't contain the string %s.\n", hash, ident.data());
                    break; // in this case the string hash exists, but it's not in the bucket
                }
                else
                {
                    return bucket.value[0];
                }
            }
            else if (bucket.hash > hash)
            {
                printf("Bucket with hash %llu doesn't exist.\n", hash);
                // Bucket with the hash doesn't exist
                break;
            }
        }

        printf("Bucket with string %s doesn't exist.\n", ident.data());
        // assert, crash, log -> because the string is not registered
    }

private:
    struct Bucket
    {
    public:
        uint64_t hash;

        std::vector<std::string_view> strings;
        std::vector<T> value;
    };

    std::vector<Bucket> m_buckets;
};