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
        double last_play_time = 0.0;
        float duration = 0.0f;
        bool duration_cached = false;
    };

    struct MusicEntry {
        std::filesystem::path path;
        Music stream{};
        bool loaded = false;
    };

    std::vector<Effect> _effects{ 300 };
    std::vector<MusicEntry> _music{ 20 };

    std::vector<int> _shuffle_list;
    int _shuffle_index = -1;
    bool _shuffle_enabled = false;

    float _resume_time = 0.0f;

    Music* _current = nullptr;
    Music* _next = nullptr;
    float _transition_time = 0.0f;
    float _transition_elapsed = 0.0f;

    float _music_volume = 1.0f;
    float _sound_volume = 1.0f;
    bool _music_muted = false;
    bool _sound_muted = false;

    bool _had_focus = true;

    void _apply_modifiers(Sound& snd, float distance_mod, float pan) {
        float Vol = 1.0f / (distance_mod <= 0.0f ? 1.0f : distance_mod);
        Vol = std::clamp(Vol, 0.0f, 1.0f);
        if (_sound_muted) Vol = 0.0f;
        else Vol *= _sound_volume;
        ::SetSoundVolume(snd, Vol);
        ::SetSoundPan(snd, pan);
    }

    float _music_volume_target(float base) const {
        if (_music_muted) return 0.0f;
        return std::clamp(base * _music_volume, 0.0f, 1.0f);
    }

    static float _compute_sound_length(const Sound& snd) {
        const AudioStream& Stream = snd.stream;
        return (Stream.sampleRate > 0)
            ? static_cast<float>(snd.frameCount) / static_cast<float>(Stream.sampleRate)
            : 0.0f;
    }

    void _handle_focus_change() {
        bool Focused = ::IsWindowFocused();

        if (_had_focus && !Focused) {
            if (_current)
                _resume_time = ::GetMusicTimePlayed(*_current);
            ::CloseAudioDevice();
        }
        else if (!_had_focus && Focused) {
            ::InitAudioDevice();

            // Reload effects
            for (auto& E : _effects) {
                if (E.loaded && !E.voices.empty()) {
                    E.base = ::LoadSound(E.path.string().c_str());
                    for (auto& V : E.voices)
                        V = ::LoadSoundAlias(E.base);
                }
            }

            // Reload current and next music only
            if (_current) {
                for (auto& M : _music) {
                    if (M.loaded) {
                        M.stream = ::LoadMusicStream(M.path.string().c_str());
                        if (&M.stream == _current || &M.stream == _next) {
                            ::PlayMusicStream(M.stream);
                            if (_resume_time > 0.0f)
                                ::SeekMusicStream(M.stream, _resume_time);
                            ::SetMusicVolume(M.stream, _music_volume_target(1.0f));
                        }
                    }
                }
            }
        }
        _had_focus = Focused;
    }

    void _handle_shuffle_playback() {
        if (!_shuffle_enabled || !_current) return;

        float Total = ::GetMusicTimeLength(*_current);
        float Played = ::GetMusicTimePlayed(*_current);
        float Remaining = Total - Played;

        if (Remaining <= 2.0f && !_next) {
            if (_shuffle_list.empty()) return;
            _shuffle_index = (_shuffle_index + 1) % static_cast<int>(_shuffle_list.size());
            int NextID = _shuffle_list[_shuffle_index];
            play_music(NextID, true, 2.0f);
        }
    }

public:
    SoundPlayer() { ::InitAudioDevice(); }

    ~SoundPlayer() {
        for (auto& E : _effects) {
            if (E.loaded) {
                for (auto& V : E.voices) ::UnloadSoundAlias(V);
                ::UnloadSound(E.base);
            }
        }
        for (auto& M : _music)
            if (M.loaded) ::UnloadMusicStream(M.stream);
        ::CloseAudioDevice();
    }

    // Volume controls
    void set_music_volume(float v) { _music_volume = std::clamp(v, 0.0f, 1.0f); }
    void set_sound_volume(float v) { _sound_volume = std::clamp(v, 0.0f, 1.0f); }
    void set_music_mute(bool mute) { _music_muted = mute; }
    void set_sound_mute(bool mute) { _sound_muted = mute; }
    float get_music_volume() const { return _music_volume; }
    float get_sound_volume() const { return _sound_volume; }
    bool is_music_muted() const { return _music_muted; }
    bool is_sound_muted() const { return _sound_muted; }

    // Loaders
    void load_effect(int index, const std::filesystem::path& path, int voices = 4) {
        if (index < 0 || index >= static_cast<int>(_effects.size())) return;
        if (voices < 1) voices = 1;
        if (!std::filesystem::exists(path)) return;

        Effect E;
        E.base = ::LoadSound(path.string().c_str());
        E.voices.reserve(static_cast<size_t>(voices));
        for (int I = 0; I < voices; ++I)
            E.voices.push_back(::LoadSoundAlias(E.base));
        E.loaded = true;
        E.duration_cached = false;
        E.id = index;
        E.path = path;
        _effects[index] = std::move(E);
    }

    void load_music(int index, const std::filesystem::path& path) {
        if (index < 0 || index >= static_cast<int>(_music.size())) return;
        if (!std::filesystem::exists(path)) return;
        _music[index] = { path, {}, false };
    }

    // --- SOUND EFFECTS ---
    void play_single(int id, float distance_mod = 1.0f, float pan = 0.0f) {
        if (!IsAudioDeviceReady()) return;
        if (id < 0 || id >= static_cast<int>(_effects.size())) return;
        auto& E = _effects[id];
        if (!E.loaded) return;

        if (!E.duration_cached) {
            E.duration = _compute_sound_length(E.base);
            E.duration_cached = true;
        }

        double Now = ::GetTime();
        double Cooldown = std::max(0.05, E.duration * 0.25);
        if (Now - E.last_play_time < Cooldown) return;
        E.last_play_time = Now;

        Sound& Snd = E.base;
        if (::IsSoundPlaying(Snd)) return;
        _apply_modifiers(Snd, distance_mod, pan);
        ::StopSound(Snd);
        ::PlaySound(Snd);
    }

    void play_multi(int id, float distance_mod = 1.0f, float pan = 0.0f) {
        if (!IsAudioDeviceReady()) return;
        if (id < 0 || id >= static_cast<int>(_effects.size())) return;
        auto& E = _effects[id];
        if (!E.loaded) return;

        if (!E.duration_cached) {
            E.duration = _compute_sound_length(E.base);
            E.duration_cached = true;
        }

        double Now = ::GetTime();
        double Cooldown = std::max(0.05, E.duration * 0.25);
        if (Now - E.last_play_time < Cooldown) return;
        E.last_play_time = Now;

        for (auto& V : E.voices) {
            if (!::IsSoundPlaying(V)) {
                _apply_modifiers(V, distance_mod, pan);
                ::PlaySound(V);
                return;
            }
        }

        Sound& V = E.voices[E.rr % E.voices.size()];
        ++E.rr;
        ::StopSound(V);
        _apply_modifiers(V, distance_mod, pan);
        ::PlaySound(V);
    }

    // --- MUSIC CONTROL ---
    void play_music(int id, bool loop = true, float transition_seconds = 1.0f) {
        if (!IsAudioDeviceReady()) return;
        if (id < 0 || id >= static_cast<int>(_music.size())) return;

        auto& Entry = _music[id];
        if (!Entry.loaded) {
            Entry.stream = ::LoadMusicStream(Entry.path.string().c_str());
            Entry.loaded = true;
        }

        Entry.stream.looping = loop;
        _next = &Entry.stream;
        _transition_time = transition_seconds;
        _transition_elapsed = 0.0f;

        if (!_current) {
            _current = _next;
            _next = nullptr;
            ::PlayMusicStream(*_current);
            ::SetMusicVolume(*_current, _music_volume_target(1.0f));
        }
        else {
            ::PlayMusicStream(*_next);
            ::SetMusicVolume(*_next, _music_volume_target(0.0f));
        }
    }

    void play_music_shuffle(const std::vector<int>& playlist) {
        if (playlist.empty()) return;
        _shuffle_list = playlist;
        _shuffle_enabled = true;
        std::shuffle(_shuffle_list.begin(), _shuffle_list.end(), std::mt19937{ std::random_device{}() });
        _shuffle_index = 0;
        play_music(_shuffle_list[_shuffle_index], true, 1.0f);
    }

    void stop_shuffle() {
        _shuffle_enabled = false;
        _shuffle_list.clear();
        _shuffle_index = -1;
    }

    void stop_music(int id) {
        if (!IsAudioDeviceReady()) return;
        if (id < 0 || id >= static_cast<int>(_music.size())) return;
        auto& Entry = _music[id];
        if (Entry.loaded) ::StopMusicStream(Entry.stream);
    }

    void update() {
        _handle_focus_change();
        if (!IsAudioDeviceReady()) return;

        if (_current) ::UpdateMusicStream(*_current);
        if (_next) ::UpdateMusicStream(*_next);

        _handle_shuffle_playback();

        if (_next && _current) {
            _transition_elapsed += ::GetFrameTime();
            float T = std::clamp(_transition_elapsed / _transition_time, 0.0f, 1.0f);
            ::SetMusicVolume(*_current, _music_volume_target(1.0f - T));
            ::SetMusicVolume(*_next, _music_volume_target(T));
            if (T >= 1.0f) {
                ::StopMusicStream(*_current);
                _current = _next;
                _next = nullptr;
            }
        }
        else if (_current) {
            ::SetMusicVolume(*_current, _music_volume_target(1.0f));
        }
    }
};
