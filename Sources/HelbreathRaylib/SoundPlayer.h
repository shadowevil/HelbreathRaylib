#pragma once
#include "raylib_include.h"
#include <vector>
#include <string>
#include <cstddef>
#include <algorithm>
#include <filesystem>
#include <random>

class SoundPlayer
{
private:
    struct Effect
    {
        raylib::Sound base{};
        int id = -1;
        std::filesystem::path path;
        std::vector<raylib::Sound> voices;
        std::size_t rr = 0;
        bool loaded = false;
        double last_play_time = 0.0;
        float duration = 0.0f;
        bool duration_cached = false;
    };

    struct MusicEntry
    {
        std::filesystem::path path;
        raylib::Music stream{};
        bool loaded = false;
    };

    std::vector<Effect> _effects{60};          // Sound effects (E###.WAV)
    std::vector<Effect> _character_sounds{30}; // Character sounds (C###.WAV)
    std::vector<Effect> _monster_sounds{200};  // Monster sounds (M###.WAV)
    std::vector<MusicEntry> _music{20};

    std::vector<int> _shuffle_list;
    int _shuffle_index = -1;
    bool _shuffle_enabled = false;

    float _resume_time = 0.0f;

    raylib::Music *_current = nullptr;
    raylib::Music *_next = nullptr;
    float _transition_time = 0.0f;
    float _transition_elapsed = 0.0f;

    float _music_volume = 1.0f;
    float _sound_volume = 1.0f;
    bool _music_muted = false;
    bool _sound_muted = false;

    bool _had_focus = true;

    void _apply_modifiers(raylib::Sound &snd, float distance_mod, float pan)
    {
        float Vol = 1.0f / (distance_mod <= 0.0f ? 1.0f : distance_mod);
        Vol = std::clamp(Vol, 0.0f, 1.0f);
        if (_sound_muted)
            Vol = 0.0f;
        else
            Vol *= _sound_volume;
        raylib::SetSoundVolume(snd, Vol);
        raylib::SetSoundPan(snd, pan);
    }

    float _music_volume_target(float base) const
    {
        if (_music_muted)
            return 0.0f;
        return std::clamp(base * _music_volume, 0.0f, 1.0f);
    }

    static float _compute_sound_length(const raylib::Sound &snd)
    {
        const raylib::AudioStream &Stream = snd.stream;
        return (Stream.sampleRate > 0)
                   ? static_cast<float>(snd.frameCount) / static_cast<float>(Stream.sampleRate)
                   : 0.0f;
    }

    void _handle_focus_change()
    {
#ifndef PLATFORM_WEB
        bool Focused = raylib::IsWindowFocused();

        if (_had_focus && !Focused)
        {
            if (_current)
                _resume_time = raylib::GetMusicTimePlayed(*_current);
            raylib::CloseAudioDevice();
        }
        else if (!_had_focus && Focused)
        {
            raylib::InitAudioDevice();

            // Reload all effects
            auto reload_effects = [](std::vector<Effect> &effects)
            {
                for (auto &E : effects)
                {
                    if (E.loaded && !E.voices.empty())
                    {
                        E.base = raylib::LoadSound(E.path.string().c_str());
                        for (auto &V : E.voices)
                            V = raylib::LoadSoundAlias(E.base);
                    }
                }
            };

            reload_effects(_effects);
            reload_effects(_character_sounds);
            reload_effects(_monster_sounds);

            // Reload current and next music only
            if (_current)
            {
                for (auto &M : _music)
                {
                    if (M.loaded)
                    {
                        M.stream = raylib::LoadMusicStream(M.path.string().c_str());
                        if (&M.stream == _current || &M.stream == _next)
                        {
                            raylib::PlayMusicStream(M.stream);
                            if (_resume_time > 0.0f)
                                raylib::SeekMusicStream(M.stream, _resume_time);
                            raylib::SetMusicVolume(M.stream, _music_volume_target(1.0f));
                        }
                    }
                }
            }
        }
        _had_focus = Focused;
#endif
    }

    void _handle_shuffle_playback()
    {
        if (!_shuffle_enabled || !_current)
            return;

        float Total = raylib::GetMusicTimeLength(*_current);
        float Played = raylib::GetMusicTimePlayed(*_current);
        float Remaining = Total - Played;

        if (Remaining <= 2.0f && !_next)
        {
            if (_shuffle_list.empty())
                return;
            _shuffle_index = (_shuffle_index + 1) % static_cast<int>(_shuffle_list.size());
            int NextID = _shuffle_list[_shuffle_index];
            play_music(NextID, true, 2.0f);
        }
    }

public:
    SoundPlayer() { raylib::InitAudioDevice(); }

    ~SoundPlayer()
    {
        auto cleanup_effects = [](std::vector<Effect> &effects)
        {
            for (auto &E : effects)
            {
                if (E.loaded)
                {
                    for (auto &V : E.voices)
                        raylib::UnloadSoundAlias(V);
                    raylib::UnloadSound(E.base);
                }
            }
        };

        cleanup_effects(_effects);
        cleanup_effects(_character_sounds);
        cleanup_effects(_monster_sounds);

        for (auto &M : _music)
            if (M.loaded)
                raylib::UnloadMusicStream(M.stream);
        raylib::CloseAudioDevice();
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
    void load_effect(int index, const std::filesystem::path &path, int voices = 4)
    {
        if (index < 0 || index >= static_cast<int>(_effects.size()))
            return;
        if (voices < 1)
            voices = 1;
        if (!std::filesystem::exists(path))
            return;

        Effect E;
        E.base = raylib::LoadSound(path.string().c_str());
        E.voices.reserve(static_cast<size_t>(voices));
        for (int I = 0; I < voices; ++I)
            E.voices.push_back(raylib::LoadSoundAlias(E.base));
        E.loaded = true;
        E.duration_cached = false;
        E.id = index;
        E.path = path;
        _effects[index] = std::move(E);
    }

    void load_character_sound(int index, const std::filesystem::path &path, int voices = 4)
    {
        if (index < 0 || index >= static_cast<int>(_character_sounds.size()))
            return;
        if (voices < 1)
            voices = 1;
        if (!std::filesystem::exists(path))
            return;

        Effect E;
        E.base = raylib::LoadSound(path.string().c_str());
        E.voices.reserve(static_cast<size_t>(voices));
        for (int I = 0; I < voices; ++I)
            E.voices.push_back(raylib::LoadSoundAlias(E.base));
        E.loaded = true;
        E.duration_cached = false;
        E.id = index;
        E.path = path;
        _character_sounds[index] = std::move(E);
    }

    void load_monster_sound(int index, const std::filesystem::path &path, int voices = 4)
    {
        if (index < 0 || index >= static_cast<int>(_monster_sounds.size()))
            return;
        if (voices < 1)
            voices = 1;
        if (!std::filesystem::exists(path))
            return;

        Effect E;
        E.base = raylib::LoadSound(path.string().c_str());
        E.voices.reserve(static_cast<size_t>(voices));
        for (int I = 0; I < voices; ++I)
            E.voices.push_back(raylib::LoadSoundAlias(E.base));
        E.loaded = true;
        E.duration_cached = false;
        E.id = index;
        E.path = path;
        _monster_sounds[index] = std::move(E);
    }

    void load_music(int index, const std::filesystem::path &path)
    {
        if (index < 0 || index >= static_cast<int>(_music.size()))
            return;
        if (!std::filesystem::exists(path))
            return;
        _music[index] = {path, {}, false};
    }

    // --- SOUND EFFECTS ---
private:
    void _play_single_internal(std::vector<Effect> &effects, int id, float distance_mod, float pan)
    {
        if (!raylib::IsAudioDeviceReady())
            return;
        if (id < 0 || id >= static_cast<int>(effects.size()))
            return;
        auto &E = effects[id];
        if (!E.loaded)
            return;

        if (!E.duration_cached)
        {
            E.duration = _compute_sound_length(E.base);
            E.duration_cached = true;
        }

        double Now = raylib::GetTime();
        double Cooldown = std::max(0.05, E.duration * 0.25);
        if (Now - E.last_play_time < Cooldown)
            return;
        E.last_play_time = Now;

        raylib::Sound &Snd = E.base;
        if (raylib::IsSoundPlaying(Snd))
            return;
        _apply_modifiers(Snd, distance_mod, pan);
        raylib::StopSound(Snd);
        raylib::PlaySound(Snd);
    }

    void _play_multi_internal(std::vector<Effect> &effects, int id, float distance_mod, float pan)
    {
        if (!raylib::IsAudioDeviceReady())
            return;
        if (id < 0 || id >= static_cast<int>(effects.size()))
            return;
        auto &E = effects[id];
        if (!E.loaded)
            return;

        if (!E.duration_cached)
        {
            E.duration = _compute_sound_length(E.base);
            E.duration_cached = true;
        }

        double Now = raylib::GetTime();
        double Cooldown = std::max(0.05, E.duration * 0.25);
        if (Now - E.last_play_time < Cooldown)
            return;
        E.last_play_time = Now;

        for (auto &V : E.voices)
        {
            if (!raylib::IsSoundPlaying(V))
            {
                _apply_modifiers(V, distance_mod, pan);
                raylib::PlaySound(V);
                return;
            }
        }

        raylib::Sound &V = E.voices[E.rr % E.voices.size()];
        ++E.rr;
        raylib::StopSound(V);
        _apply_modifiers(V, distance_mod, pan);
        raylib::PlaySound(V);
    }

public:
    // Play effect sounds (E###.WAV files)
    void play_effect(int id, float distance_mod = 1.0f, float pan = 0.5f)
    {
        _play_single_internal(_effects, id, distance_mod, pan);
    }

    void play_effect_multi(int id, float distance_mod = 1.0f, float pan = 0.5f)
    {
        _play_multi_internal(_effects, id, distance_mod, pan);
    }

    // Play character sounds (C###.WAV files)
    void play_character_sound(int id, float distance_mod = 1.0f, float pan = 0.5f)
    {
        _play_single_internal(_character_sounds, id, distance_mod, pan);
    }

    void play_character_sound_multi(int id, float distance_mod = 1.0f, float pan = 0.5f)
    {
        _play_multi_internal(_character_sounds, id, distance_mod, pan);
    }

    // Play monster sounds (M###.WAV files)
    void play_monster_sound(int id, float distance_mod = 1.0f, float pan = 0.5f)
    {
        _play_single_internal(_monster_sounds, id, distance_mod, pan);
    }

    void play_monster_sound_multi(int id, float distance_mod = 1.0f, float pan = 0.5f)
    {
        _play_multi_internal(_monster_sounds, id, distance_mod, pan);
    }

    // --- MUSIC CONTROL ---
    void play_music(int id, bool loop = true, float transition_seconds = 1.0f)
    {
        if (!raylib::IsAudioDeviceReady())
            return;
        if (id < 0 || id >= static_cast<int>(_music.size()))
            return;

        auto &Entry = _music[id];
        if (!Entry.loaded)
        {
            Entry.stream = raylib::LoadMusicStream(Entry.path.string().c_str());
            Entry.loaded = true;
        }

        Entry.stream.looping = loop;
        _next = &Entry.stream;
        _transition_time = transition_seconds;
        _transition_elapsed = 0.0f;

        if (!_current)
        {
            _current = _next;
            _next = nullptr;
            raylib::PlayMusicStream(*_current);
            raylib::SetMusicVolume(*_current, _music_volume_target(1.0f));
        }
        else
        {
            raylib::PlayMusicStream(*_next);
            raylib::SetMusicVolume(*_next, _music_volume_target(0.0f));
        }
    }

    void play_music_shuffle(const std::vector<int> &playlist)
    {
        if (playlist.empty())
            return;
        _shuffle_list = playlist;
        _shuffle_enabled = true;
        std::shuffle(_shuffle_list.begin(), _shuffle_list.end(), std::mt19937{std::random_device{}()});
        _shuffle_index = 0;
        play_music(_shuffle_list[_shuffle_index], true, 1.0f);
    }

    void stop_shuffle()
    {
        _shuffle_enabled = false;
        _shuffle_list.clear();
        _shuffle_index = -1;
    }

    void stop_music(int id)
    {
        if (!raylib::IsAudioDeviceReady())
            return;
        if (id < 0 || id >= static_cast<int>(_music.size()))
            return;
        auto &Entry = _music[id];
        if (Entry.loaded)
            raylib::StopMusicStream(Entry.stream);
    }

    void update()
    {
        _handle_focus_change();
        if (!raylib::IsAudioDeviceReady())
            return;

        if (_current)
            raylib::UpdateMusicStream(*_current);
        if (_next)
            raylib::UpdateMusicStream(*_next);

        _handle_shuffle_playback();

        if (_next && _current)
        {
            _transition_elapsed += raylib::GetFrameTime();
            float T = std::clamp(_transition_elapsed / _transition_time, 0.0f, 1.0f);
            raylib::SetMusicVolume(*_current, _music_volume_target(1.0f - T));
            raylib::SetMusicVolume(*_next, _music_volume_target(T));
            if (T >= 1.0f)
            {
                raylib::StopMusicStream(*_current);
                _current = _next;
                _next = nullptr;
            }
        }
        else if (_current)
        {
            raylib::SetMusicVolume(*_current, _music_volume_target(1.0f));
        }
    }
};
