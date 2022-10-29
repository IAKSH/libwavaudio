#include "wavaudio.hpp"

int wava::WavAudio::getFileCursorMark(std::ifstream &fs, std::string mark)
{
    int len = mark.length();
    // dynamic C-style array may not work on msvc
    // if that happens, fix it by yourself
    // it's sample, good luck
    char buf[len + 1];
    buf[len] = '\0';
    int i = 0;
    while (!fs.eof())
    {
        fs.seekg(i++, std::ios::beg);
        fs.read(buf, sizeof(char) * len);
        if (mark.compare(buf) == 0)
            return i;
    }
    std::cerr << "[libwavaudio] ERROR: failed to locate mark (" << mark << ") in moveFileCursorToMark()\n";
    abort();
}

wava::WavAudio::WavAudio()
{
}

wava::WavAudio::WavAudio(const char *path)
{
    load(path);
}

wava::WavAudio::~WavAudio()
{
    clear();
}

void wava::WavAudio::load(const char *path)
{
    if (loaded)
        clear();

    int i;
    std::ifstream fs(path, std::ios::in | std::ios::binary);
    if (!fs)
    {
        std::cerr << "[libwavaudio] ERROR: can't open file (" << path << ")\n";
        abort();
    }

    i = getFileCursorMark(fs, "RIFF") - 1;
    fs.seekg(i + 4, std::ios::beg);
    fs.read((char *)&chunkSize, 4);
    fs.seekg(i + 8, std::ios::beg);
    fs.read((char *)&format, 4);

    if (std::string(format).compare("WAVE") != 0)
    {
        std::cerr << "[libwavaudio] ERROR: trying to load a none-wav format file (" << path << ")\n";
        abort();
    }

    i = getFileCursorMark(fs, "fmt") - 1;
    fs.seekg(i + 4, std::ios::beg);
    fs.read((char *)&subChunk1Size, 4);
    fs.seekg(i + 8, std::ios::beg);
    fs.read((char *)&audioFormat, 2);
    fs.seekg(i + 10, std::ios::beg);
    fs.read((char *)&numChannels, 2);
    fs.seekg(i + 12, std::ios::beg);
    fs.read((char *)&sampleRate, 4);
    fs.seekg(i + 16, std::ios::beg);
    fs.read((char *)&byteRate, 4);
    fs.seekg(i + 20, std::ios::beg);
    fs.read((char *)&blockAlign, 2);
    fs.seekg(i + 22, std::ios::beg);
    fs.read((char *)&bitsPerSample, 2);
    fs.seekg(i + 24, std::ios::beg);

    i = getFileCursorMark(fs, "data") - 1;
    fs.seekg(i + 4, std::ios::beg);
    fs.read((char *)&subChunk2Size, 4);
    fs.seekg(i + 8, std::ios::beg);
    data = new unsigned char[subChunk2Size];
    fs.read((char *)data, subChunk2Size);

    pcmData = data;
    pcmDataSize = subChunk2Size;

    // load data to OpenAL buffer
    alGenBuffers(1, &buffer);
    if (bitsPerSample == 16)
    {
        if (numChannels == 1)
            alBufferData(buffer, AL_FORMAT_MONO16, data, subChunk2Size, sampleRate);
        else if (numChannels == 2)
            alBufferData(buffer, AL_FORMAT_STEREO16, data, subChunk2Size, sampleRate);
        else
            abort();
    }
    else if (bitsPerSample == 8)
    {
        if (numChannels == 1)
            alBufferData(buffer, AL_FORMAT_MONO8, data, subChunk2Size, sampleRate);
        else if (numChannels == 2)
            alBufferData(buffer, AL_FORMAT_STEREO8, data, subChunk2Size, sampleRate);
        else
            abort();
    }
    else
        abort();

    fs.close();
    loaded = true;
}

uint32_t wava::WavAudio::getBuffer()
{
    if (loaded)
        return buffer;
    else
    {
        std::cerr << "[libwavaudio] ERROR: called getBuffer() from an unloaded WavAudio\n";
        abort();
    }
}

void wava::WavAudio::loadPCMFromMemory(unsigned char *data, int size)
{
    if (loaded)
        clear();

    pcmData = data;
    pcmDataSize = size;

    const char WAVE_FORMAT[] = "WAVE";
    for (int i = 0; i < 4; i++)
        format[i] = WAVE_FORMAT[i];

    subChunk2Size = size + 8;
    chunkSize = size + 36;
    std::cout << chunkSize << '\n';// test
    subChunk1Size = 16;
    audioFormat = 1;
    numChannels = 1;
    sampleRate = 44100;
    byteRate = 88200;
    blockAlign = 2;
    bitsPerSample = 16;

    // load data to OpenAL buffer
    alGenBuffers(1, &buffer);
    if (bitsPerSample == 16)
    {
        if (numChannels == 1)
            alBufferData(buffer, AL_FORMAT_MONO16, data, subChunk2Size, sampleRate);
        else if (numChannels == 2)
            alBufferData(buffer, AL_FORMAT_STEREO16, data, subChunk2Size, sampleRate);
        else
            abort();
    }
    else if (bitsPerSample == 8)
    {
        if (numChannels == 1)
            alBufferData(buffer, AL_FORMAT_MONO8, data, subChunk2Size, sampleRate);
        else if (numChannels == 2)
            alBufferData(buffer, AL_FORMAT_STEREO8, data, subChunk2Size, sampleRate);
        else
            abort();
    }
    else
        abort();

    loaded = true;
}

void wava::WavAudio::save(const char *path)
{
    if(!loaded)
    {
        std::cerr << "[libwavaudio] ERROR: tried to save an empty wav audio to " << path << '\n';
        abort();
    }

    int temp = 66;

    std::ofstream fs(path,std::ios::out | std::ios::binary);
    fs.write("RIFF",4);
    fs.write((char*)&chunkSize,4);
    fs.write(format,4);
    fs.write("fmt ",4);
    fs.write((char*)&subChunk1Size,4);
    fs.write((char*)&audioFormat,2);
    fs.write((char*)&numChannels,2);
    fs.write((char*)&sampleRate,4);
    fs.write((char*)&byteRate,4);
    fs.write((char*)&blockAlign,2);
    fs.write((char*)&bitsPerSample,2);
    fs.write("data",4);
    fs.write((char*)&subChunk2Size,4);
    fs.write((char*)pcmData,pcmDataSize);
    fs.close();
}

void wava::WavAudio::clear()
{
    delete[] pcmData;
    alDeleteBuffers(1, &buffer);
}

unsigned char *wava::WavAudio::getPcmData()
{
    return pcmData;
}

uint32_t wava::WavAudio::getPcmSize()
{
    return pcmDataSize;
}
