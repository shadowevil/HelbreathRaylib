#pragma once
#include "raylib_include.h"
#include <vector>
#include <string>
#include <cstddef>
#include <algorithm>
#include <filesystem>
#include <random>

class SoundPlayer {
private:
    struct Effect {
        Sound base{};
        int id = -1;
        std::filesystem::path path;
        std::vector<Sound> voices;
        std::size_t rr = 0;
        bool loaded = false;
        double lastPlayTime = 0.0;
        float duration = 0.0f;
        bool durationCached = false;
    };

    struct MusicEntry {
        std::filesystem::path path;
        Music stream{};
        bool loaded = false;
    };

    std::vector<Effect> m_effects{ 300 };
    std::vector<MusicEntry> m_music{ 20 };

    std::vector<int> m_shuffleList;
    int m_shuffleIndex = -1;
    bool m_shuffleEnabled = false;

    float m_resumeTime = 0.0f;

    Music* m_current = nullptr;
    Music* m_next = nullptr;
    float m_transitionTime = 0.0f;
    float m_transitionElapsed = 0.0f;

    float m_musicVolume = 1.0f;
    float m_soundVolume = 1.0f;
    bool m_musicMuted = false;
    bool m_soundMuted = false;

    bool m_hadFocus = true;

    void ApplyModifiers(Sound& snd, float distanceMod, float pan) {
        float vol = 1.0f / (distanceMod <= 0.0f ? 1.0f : distanceMod);
        vol = std::clamp(vol, 0.0f, 1.0f);
        if (m_soundMuted) vol = 0.0f;
        else vol *= m_soundVolume;
        ::SetSoundVolume(snd, vol);
        ::SetSoundPan(snd, pan);
    }

    float MusicVolumeTarget(float base) const {
        if (m_musicMuted) return 0.0f;
        return std::clamp(base * m_musicVolume, 0.0f, 1.0f);
    }

    static float ComputeSoundLength(const Sound& snd) {
        const AudioStream& s = snd.stream;
        return (s.sampleRate > 0)
            ? static_cast<float>(snd.frameCount) / static_cast<float>(s.sampleRate)
            : 0.0f;
    }

    void HandleFocusChange() {
        bool focused = ::IsWindowFocused();

        if (m_hadFocus && !focused) {
            if (m_current)
                m_resumeTime = ::GetMusicTimePlayed(*m_current);
            ::CloseAudioDevice();
        }
        else if (!m_hadFocus && focused) {
            ::InitAudioDevice();

            // Reload effects
            for (auto& e : m_effects) {
                if (e.loaded && !e.voices.empty()) {
                    e.base = ::LoadSound(e.path.string().c_str());
                    for (auto& v : e.voices)
                        v = ::LoadSoundAlias(e.base);
                }
            }

            // Reload current and next music only
            if (m_current) {
                for (auto& m : m_music) {
                    if (m.loaded) {
                        m.stream = ::LoadMusicStream(m.path.string().c_str());
                        if (&m.stream == m_current || &m.stream == m_next) {
                            ::PlayMusicStream(m.stream);
                            if (m_resumeTime > 0.0f)
                                ::SeekMusicStream(m.stream, m_resumeTime);
                            ::SetMusicVolume(m.stream, MusicVolumeTarget(1.0f));
                        }
                    }
                }
            }
        }
        m_hadFocus = focused;
    }

    void HandleShufflePlayback() {
        if (!m_shuffleEnabled || !m_current) return;

        float total = ::GetMusicTimeLength(*m_current);
        float played = ::GetMusicTimePlayed(*m_current);
        float remaining = total - played;

        if (remaining <= 2.0f && !m_next) {
            if (m_shuffleList.empty()) return;
            m_shuffleIndex = (m_shuffleIndex + 1) % static_cast<int>(m_shuffleList.size());
            int nextID = m_shuffleList[m_shuffleIndex];
            PlayMusic(nextID, true, 2.0f);
        }
    }

public:
    SoundPlayer() { ::InitAudioDevice(); }

    ~SoundPlayer() {
        for (auto& e : m_effects) {
            if (e.loaded) {
                for (auto& v : e.voices) ::UnloadSoundAlias(v);
                ::UnloadSound(e.base);
            }
        }
        for (auto& m : m_music)
            if (m.loaded) ::UnloadMusicStream(m.stream);
        ::CloseAudioDevice();
    }

    // Volume controls
    void SetMusicVolume(float v) { m_musicVolume = std::clamp(v, 0.0f, 1.0f); }
    void SetSoundVolume(float v) { m_soundVolume = std::clamp(v, 0.0f, 1.0f); }
    void SetMusicMute(bool mute) { m_musicMuted = mute; }
    void SetSoundMute(bool mute) { m_soundMuted = mute; }
    float GetMusicVolume() const { return m_musicVolume; }
    float GetSoundVolume() const { return m_soundVolume; }
    bool IsMusicMuted() const { return m_musicMuted; }
    bool IsSoundMuted() const { return m_soundMuted; }

    // Loaders
    void LoadEffect(int index, const std::filesystem::path& path, int voices = 4) {
        if (index < 0 || index >= static_cast<int>(m_effects.size())) return;
        if (voices < 1) voices = 1;
        if (!std::filesystem::exists(path)) return;

        Effect e;
        e.base = ::LoadSound(path.string().c_str());
        e.voices.reserve(static_cast<size_t>(voices));
        for (int i = 0; i < voices; ++i)
            e.voices.push_back(::LoadSoundAlias(e.base));
        e.loaded = true;
        e.durationCached = false;
        e.id = index;
        e.path = path;
        m_effects[index] = std::move(e);
    }

    void LoadMusic(int index, const std::filesystem::path& path) {
        if (index < 0 || index >= static_cast<int>(m_music.size())) return;
        if (!std::filesystem::exists(path)) return;
        m_music[index] = { path, {}, false };
    }

    // --- SOUND EFFECTS ---
    void PlaySingle(int id, float distanceMod = 1.0f, float pan = 0.0f) {
        if (!IsAudioDeviceReady()) return;
        if (id < 0 || id >= static_cast<int>(m_effects.size())) return;
        auto& e = m_effects[id];
        if (!e.loaded) return;

        if (!e.durationCached) {
            e.duration = ComputeSoundLength(e.base);
            e.durationCached = true;
        }

        double now = ::GetTime();
        double cooldown = std::max(0.05, e.duration * 0.25);
        if (now - e.lastPlayTime < cooldown) return;
        e.lastPlayTime = now;

        Sound& snd = e.base;
        if (::IsSoundPlaying(snd)) return;
        ApplyModifiers(snd, distanceMod, pan);
        ::StopSound(snd);
        ::PlaySound(snd);
    }

    void PlayMulti(int id, float distanceMod = 1.0f, float pan = 0.0f) {
        if (!IsAudioDeviceReady()) return;
        if (id < 0 || id >= static_cast<int>(m_effects.size())) return;
        auto& e = m_effects[id];
        if (!e.loaded) return;

        if (!e.durationCached) {
            e.duration = ComputeSoundLength(e.base);
            e.durationCached = true;
        }

        double now = ::GetTime();
        double cooldown = std::max(0.05, e.duration * 0.25);
        if (now - e.lastPlayTime < cooldown) return;
        e.lastPlayTime = now;

        for (auto& v : e.voices) {
            if (!::IsSoundPlaying(v)) {
                ApplyModifiers(v, distanceMod, pan);
                ::PlaySound(v);
                return;
            }
        }

        Sound& v = e.voices[e.rr % e.voices.size()];
        ++e.rr;
        ::StopSound(v);
        ApplyModifiers(v, distanceMod, pan);
        ::PlaySound(v);
    }

    // --- MUSIC CONTROL ---
    void PlayMusic(int id, bool loop = true, float transitionSeconds = 1.0f) {
        if (!IsAudioDeviceReady()) return;
        if (id < 0 || id >= static_cast<int>(m_music.size())) return;

        auto& entry = m_music[id];
        if (!entry.loaded) {
            entry.stream = ::LoadMusicStream(entry.path.string().c_str());
            entry.loaded = true;
        }

        entry.stream.looping = loop;
        m_next = &entry.stream;
        m_transitionTime = transitionSeconds;
        m_transitionElapsed = 0.0f;

        if (!m_current) {
            m_current = m_next;
            m_next = nullptr;
            ::PlayMusicStream(*m_current);
            ::SetMusicVolume(*m_current, MusicVolumeTarget(1.0f));
        }
        else {
            ::PlayMusicStream(*m_next);
            ::SetMusicVolume(*m_next, MusicVolumeTarget(0.0f));
        }
    }

    void PlayMusicShuffle(const std::vector<int>& playlist) {
        if (playlist.empty()) return;
        m_shuffleList = playlist;
        m_shuffleEnabled = true;
        std::shuffle(m_shuffleList.begin(), m_shuffleList.end(), std::mt19937{ std::random_device{}() });
        m_shuffleIndex = 0;
        PlayMusic(m_shuffleList[m_shuffleIndex], true, 1.0f);
    }

    void StopShuffle() {
        m_shuffleEnabled = false;
        m_shuffleList.clear();
        m_shuffleIndex = -1;
    }

    void StopMusic(int id) {
        if (!IsAudioDeviceReady()) return;
        if (id < 0 || id >= static_cast<int>(m_music.size())) return;
        auto& entry = m_music[id];
        if (entry.loaded) ::StopMusicStream(entry.stream);
    }

    void Update() {
        HandleFocusChange();
        if (!IsAudioDeviceReady()) return;

        if (m_current) ::UpdateMusicStream(*m_current);
        if (m_next) ::UpdateMusicStream(*m_next);

        HandleShufflePlayback();

        if (m_next && m_current) {
            m_transitionElapsed += ::GetFrameTime();
            float t = std::clamp(m_transitionElapsed / m_transitionTime, 0.0f, 1.0f);
            ::SetMusicVolume(*m_current, MusicVolumeTarget(1.0f - t));
            ::SetMusicVolume(*m_next, MusicVolumeTarget(t));
            if (t >= 1.0f) {
                ::StopMusicStream(*m_current);
                m_current = m_next;
                m_next = nullptr;
            }
        }
        else if (m_current) {
            ::SetMusicVolume(*m_current, MusicVolumeTarget(1.0f));
        }
    }
};
