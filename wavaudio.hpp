#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include <iostream>
#include <fstream>
#include <array>

#include <cstdint>

namespace wava
{
    class WavAudio
    {
    private:
        uint32_t buffer;
        bool loop = true;
        bool loaded = false;

        // RIFF chunk (main chunk)
        uint32_t chunkSize;
        char format[5] = {'\0'};

        // sub-chunk 1 (fmt chunk)
        uint32_t subChunk1Size;
        uint16_t audioFormat;
        uint16_t numChannels;
        uint32_t sampleRate;
        uint32_t byteRate;
        uint16_t blockAlign;
        uint16_t bitsPerSample;

        // sub-chunk 2 (data)
        uint32_t subChunk2Size;
        unsigned char *data;

        int getFileCursorMark(std::ifstream &fs, std::string mark);
        void setDefeautWavMeta(unsigned char *newData, int size);
        void loadPcmToOpenAL();
        void clear();

        template <typename T>
        void readFile(std::ifstream &fs, T &t);

        template <typename T>
        void readFile(std::ifstream &fs, T &t, int size);

        template <typename T>
        void readFileWithOffset(std::ifstream &fs, T &t, int offset);

        template <typename T>
        void readFileWithOffset(std::ifstream &fs, T &t, int offset, int size);

        template <typename T>
        void writeFile(std::ofstream &fs, T &t);

        template <typename T>
        void writeFile(std::ofstream &fs, T &t, int size);

        template <typename T>
        void writeFileWithOffset(std::ofstream &fs, T &t, int offset);

        template <typename T>
        void writeFileWithOffset(std::ofstream &fs, T &t, int offset, int size);

    public:
        WavAudio();
        WavAudio(const char *path);
        ~WavAudio();

        void load(const char *path);
        void loadPCMFromMemory(unsigned char *data, int size);
        void save(const char *path);
        unsigned char *getPcmData();
        uint32_t getBuffer();
        uint32_t getPcmSize();
    };
}