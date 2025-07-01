#ifndef _PAK_ARCHIVE_H_
#define _PAK_ARCHIVE_H_


#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>

// Структуры PAK
#pragma pack(push, 1)
struct PakHeader {
    uint32_t magic;
    uint32_t entries;
};

struct PakEntry {
    char name[32];
    uint32_t offset;
    uint32_t size;
};
#pragma pack(pop)

// Менеджер ресурсов
class PakLoader {
    std::ifstream pak;
    std::unordered_map<std::string, PakEntry> catalog;

public:
    bool Open(const char* path) {
        pak.open(path, std::ios::binary);
        if (!pak) return false;

        PakHeader header;
        pak.read(reinterpret_cast<char*>(&header), sizeof(header));

        if (header.magic != 0x4B4150) {
            pak.close();
            return false;
        }

        for (uint32_t i = 0; i < header.entries; ++i) {
            PakEntry entry;
            pak.read(reinterpret_cast<char*>(&entry), sizeof(entry));
            catalog[entry.name] = entry;
        }

        return true;
    }

    std::vector<char> Load(const char* name) {
        auto it = catalog.find(name);
        if (it == catalog.end()) return {};

        PakEntry& entry = it->second;
        std::vector<char> data(entry.size);

        pak.seekg(entry.offset);
        pak.read(data.data(), entry.size);

        return data;
    }
};


LPDIRECT3DTEXTURE9 LoadTextureFromPak(PakLoader& loader, const char* name);


#endif // _PAK_ARCHIVE_H_
