#pragma once
#include "vector.h"

class Vector;
class QAngle;

enum soundlevel_t {
    SNDLVL_NONE = 0,
    SNDLVL_20dB = 20,
    SNDLVL_25dB = 25,
    SNDLVL_30dB = 30,
    SNDLVL_35dB = 35,
    SNDLVL_40dB = 40,
    SNDLVL_45dB = 45,
    SNDLVL_50dB = 50,
    SNDLVL_55dB = 55,
    SNDLVL_60dB = 60,
    SNDLVL_65dB = 65,
    SNDLVL_70dB = 70,
    SNDLVL_75dB = 75,
    SNDLVL_80dB = 80,
    SNDLVL_85dB = 85,
    SNDLVL_90dB = 90,
    SNDLVL_95dB = 95,
    SNDLVL_100dB = 100,
    SNDLVL_105dB = 105,
    SNDLVL_110dB = 110,
    SNDLVL_120dB = 120,
    SNDLVL_125dB = 125,
    SNDLVL_130dB = 130,
    SNDLVL_140dB = 140,
    SNDLVL_150dB = 150,
    SNDLVL_180dB = 180,
};

enum SoundFlags_t {
    SND_NOFLAGS = 0,
    SND_CHANGE_VOL = 1,
    SND_CHANGE_PITCH = 2,
    SND_STOP = 4,
    SND_SPAWNING = 8,
    SND_DELAY = 16,
    SND_STOP_LOOPING = 32,
    SND_SPEAKER = 64,
    SND_SHOULDPAUSE = 128,
    SND_IGNORE_MORE_THAN_ONCE = 256,
    SND_IS_NOT_DUP = 512,
    SND_IS_DUP = 1024,
};

enum SoundChannel_t {
    CHAN_REPLACE = -1,
    CHAN_AUTO = 0,
    CHAN_WEAPON = 1,
    CHAN_VOICE = 2,
    CHAN_ITEM = 3,
    CHAN_BODY = 4,
    CHAN_STREAM = 5,
    CHAN_STATIC = 6,
    CHAN_VOICE2 = 7,
    CHAN_VOICE_BASE = 8,
};

struct SpatializationInfo_t {
    Vector* m_pOrigin;
    QAngle* m_pAngles;
    Vector* m_pVelocity;
    int m_nFixCount;
};

// IEngineSound interface (L4D2 uses IEngineSound003)
// Obtained via CreateInterface("engine.dll", "IEngineSound003")
class IEngineSound {
public:
    virtual ~IEngineSound() {}
    virtual bool PlaySound(const char* pSample, float flVolume, float flAttenuation, int nFlags, int iPitch) = 0;
    virtual void EmitSound(void* filter, int iEntIndex, int iChannel, const char* pSoundEntry,
        unsigned int nSoundEntryHash, const char* pSample, float flVolume, float flAttenuation,
        int nSeed, int iFlags, int iPitch, const Vector* pOrigin, const Vector* pDirection,
        void* pUtlVecOrigins, bool bUpdatePositions, float soundTime, int nSpeakerEntity,
        int nStartDelay) = 0;
    virtual void EmitSentence(void* filter, int iEntIndex, int iChannel, const char* pSentenceName,
        float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch,
        const Vector* pOrigin, const Vector* pDirection, void* pUtlVecOrigins,
        bool bUpdatePositions, float soundTime, int nSpeakerEntity) = 0;
    virtual void StopSound(int iEntIndex, int iChannel) = 0;
    virtual void StopAllSounds(bool bClear) = 0;
    virtual void SetRoomType(int iEntIndex, int iRoomType) = 0;
    virtual void SetAudioState(const void* pAudioState) = 0;
    virtual void PrecacheSound(const char* pSample, bool bPreload, bool bIsUISound) = 0;
    virtual void PrecacheSentenceGroup(const char* pGroupName) = 0;
    virtual bool IsSoundPrecached(const char* pSample) = 0;
    virtual float SoundDuration(const char* pSample) = 0;
    virtual float GetSoundDuration(const char* pSample) = 0;
    virtual int GetActiveSounds(void* pActiveSounds) = 0;
    virtual bool IsLoopingSound(const char* pSample) = 0;
    virtual int GetSoundChannel(int nEntIndex, int nChannel) = 0;
};

// Listener parameters for S_UpdateListener
struct ListenerParams_t {
    Vector origin;
    Vector forward;
    Vector right;
    Vector up;
};
