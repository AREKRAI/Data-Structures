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
    void push(const std::string_view& ident, const T& value)
    {
        uint64_t hash = xorHash(ident.data());

        for (size_t bucketInd = 0; bucketInd < m_buckets.size(); bucketInd++)
        {
            if (m_buckets[bucketInd].hash == hash)
            {
                m_buckets[bucketInd].identifiers.push_back(ident);
                m_buckets[bucketInd].value.push_back(value);
                return;
            }
            else if (m_buckets[bucketInd].hash > hash)
            {
                Bucket bucket;
                bucket.hash = hash;
                bucket.identifiers.push_back(ident);
                bucket.value.push_back(value);

                m_buckets.insert(m_buckets.begin() + bucketInd, bucket);

                return;
            }
        }

        Bucket bucket;
        bucket.hash = hash;
        bucket.identifiers.push_back(ident);
        bucket.value.push_back(value);

        m_buckets.push_back(bucket);
    }

    bool exists(const std::string_view& ident)
    {
        uint64_t hash = xorHash(ident.data());

        for (size_t bucketInd = 0; bucketInd < m_buckets.size(); bucketInd++)
        {
            if (m_buckets[bucketInd].hash == hash)
            {
                for (size_t itemInd = 0; itemInd < m_buckets[bucketInd].identifiers.size(); itemInd++)
                {
                    if (m_buckets[bucketInd].identifiers[itemInd] == ident)
                    {
                        return true;
                    }
                }

                // Item isn't inside the bucket
                return false;
            }
            else if (m_buckets[bucketInd].hash > hash)
            {
                // bucket for the identifier doesn't exist
                return false;
            }
        }
    }

    void pop(const std::string_view& ident)
    {
        uint64_t hash = xorHash(ident.data());

        for (size_t bucketInd = 0; bucketInd < m_buckets.size(); bucketInd++)
        {
            if (m_buckets[bucketInd].hash == hash)
            {
                for (size_t itemInd = 0; itemInd < m_buckets[bucketInd].identifiers.size(); itemInd++)
                {
                    if (m_buckets[bucketInd].identifiers[itemInd] == ident)
                    {
                        m_buckets[bucketInd].identifiers.erase(m_buckets[bucketInd].identifiers.begin() + itemInd);
                        m_buckets[bucketInd].value.erase(m_buckets[bucketInd].value.begin() + itemInd);
                    }
                }

                // Item isn't inside the bucket
                return;
            }
            else if (m_buckets[bucketInd].hash > hash)
            {
                // bucket for the identifier doesn't exist
                return;
            }
        }
    }

    T& operator[](const std::string_view& ident)
    {
        uint64_t hash = xorHash(ident.data());

        for (Bucket& bucket : m_buckets)
        {
            if (bucket.hash == hash)
            {
                for (size_t itemInd = 0; itemInd < bucket.identifiers.size(); itemInd++)
                {
                    if (bucket.identifiers[itemInd] == ident)
                    {
                        return bucket.value[itemInd];
                    }
                }
                printf("Bucket with hash %llu exists, it doesn't contain the string %s.\n", hash, ident.data());
                break; // in this case the string hash exists, but it's not in the bucket
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
        
        push(ident, T()); 
        return this->operator[](ident);
        // I know this solution is bad I can't be bothered to fix the shit.
    }

private:
    struct Bucket
    {
    public:
        uint64_t hash;

        std::vector<std::string_view> identifiers;
        std::vector<T> value;
    };

    std::vector<Bucket> m_buckets;
};