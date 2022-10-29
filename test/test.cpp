#include "../wavaudio.hpp"
#include <thread>
#include <chrono>

class AudioPlayer
{
private:
    ALCdevice *device = nullptr;
    ALCcontext *context = nullptr;
    ALuint audioSource;
    ALfloat audioSourcePos[3];
    ALfloat audioSourceVel[3];

    void initializeOpenAL()
    {
        device = alcOpenDevice(nullptr); // open defeault device
        context = alcCreateContext(device, nullptr);
        alcMakeContextCurrent(context);
    }

    void closeOpenAL()
    {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        alcCloseDevice(device);
    }

public:
    AudioPlayer()
    {
        initializeOpenAL();
        alGenSources(1, &audioSource);
    }

    ~AudioPlayer()
    {
        closeOpenAL();
    }

    void play(wava::WavAudio &wav, bool loopable, float posX, float posY, float posZ, float velX, float velY, float velZ)
    {
        audioSourcePos[0] = posX;
        audioSourcePos[1] = posY;
        audioSourcePos[2] = posZ;

        audioSourceVel[0] = velX;
        audioSourceVel[1] = velY;
        audioSourceVel[2] = velZ;

        alSourcei(audioSource, AL_BUFFER, wav.getBuffer());
        alSourcef(audioSource, AL_PITCH, 1.0f);
        alSourcef(audioSource, AL_GAIN, 1.0f);
        alSourcefv(audioSource, AL_POSITION, audioSourcePos);
        alSourcefv(audioSource, AL_VELOCITY, audioSourceVel);
        alSourcei(audioSource, AL_LOOPING, static_cast<ALboolean>(loopable));

        alSourcePlay(audioSource);
    }

    void stop()
    {
        alSourceStop(audioSource);
    }
};

int main() noexcept
{
    std::cout << "hello world!\n";

    AudioPlayer ap;
    wava::WavAudio wa("../test/sounds/heart.wav");
    wava::WavAudio toSave;
    toSave.loadPCMFromMemory(wa.getPcmData(),wa.getPcmSize());
    toSave.save("../out.wav");

    ap.play(toSave, true, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    std::cout << "finished!\n";
    while (true)
        std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}