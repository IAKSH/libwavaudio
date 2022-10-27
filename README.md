# libwavaudio
1. 由于没有找到心仪的库，我自己搓了个
2. 这个库是给OpenAL用的.wav音频封装，实现了简单的.wav解码
3. 这个库未与OpenAL解耦，以后可能会
4. 没有做编码，以后可能有

# 使用方法
1. 详见`test/test.cpp`
2. 载入并解码一个wav文件
    ````cpp
    wava::WavAudio wa("../test/sounds/heart.wav");

    // 或者这样
    wava::WavAudio wa;
    wa.load("../test/sounds/heart.wav");
    ````
3. 获取PCM数据
    ````cpp
    wav.getBuffer();

    // 可以像这样直接赛给OpenAL用
    alSourcei(audioSource, AL_BUFFER, wav.getBuffer());

    // 另外，WavAudio类会管理自己的内存，*应该*不会泄露
    ````