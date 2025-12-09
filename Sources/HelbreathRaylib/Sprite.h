#pragma once
#include "raylib_include.h"
#include "PAK.h"
#include "global_constants.h"
#include <filesystem>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include <memory>

class CSprite {
public:
	inline static std::unique_ptr<CSprite> create(const PAKLib::pak& pak, size_t index) {
		std::unique_ptr<CSprite> Sprite(new CSprite());

		Sprite->_sprite_rectangles = pak.sprites[index].sprite_rectangles;
		Sprite->_file_path = pak.pak_file_path;
		Sprite->_sprite_index = index;
		return Sprite;
	}

	~CSprite() {
		raylib::UnloadTexture(_texture);
	}

	void preload() {
        //if (!this || !_file_path.size())  // or a dedicated 'is valid' flag
        //    return;
		_last_used_time = raylib::GetTime();
		if (raylib::IsTextureValid(_texture))
			return;

		const auto& SpriteObj = PAKLib::get_sprite_fast(_file_path, _sprite_index);
		raylib::Image Image_ = raylib::LoadImageFromMemory(constant::SPRITE_IMAGE_TYPE.c_str(), SpriteObj.image_data.data(), static_cast<int>(SpriteObj.image_data.size()));
		_texture = raylib::LoadTextureFromImage(Image_);
		raylib::SetTextureFilter(_texture, raylib::TEXTURE_FILTER_POINT);
		raylib::UnloadImage(Image_);
	}

	void unload_if_unused(double current_time, double timeout_seconds) {
		if (raylib::IsTextureValid(_texture) && (current_time - _last_used_time) >= timeout_seconds) {
			raylib::UnloadTexture(_texture);
			_texture = {};
		}
	}

	void draw(int x, int y, size_t frame);
    void draw(int x, int y, size_t frame, raylib::Color tint);

    PAKLib::sprite_rect get_frame_rectangle(size_t frame) const {
        size_t FrameCount = _sprite_rectangles.size();
        if (FrameCount == 0 || frame >= FrameCount)
            throw std::out_of_range("Frame index is out of range.");
        const auto& Rect = _sprite_rectangles[frame];
        return Rect;
	}

private:
	CSprite() = default;

protected:
	double _last_used_time{};
	std::vector<PAKLib::sprite_rect> _sprite_rectangles{};
	std::string _file_path{};
	size_t _sprite_index{};
	raylib::Texture2D _texture{};
};

class CSpriteLoader {
public:
	static void open_pak(const std::filesystem::path& pak_file, const std::function<void(CSpriteLoader&)>& use) {
		CSpriteLoader Loader;
		Loader._current_pak = PAKLib::loadpak_fast(pak_file.string());
		Loader._is_pak_open = true;

		try {
			use(Loader);
		}
		catch (...) {
			Loader._is_pak_open = false;
			throw;
		}

		Loader._is_pak_open = false;
	}

    static void open_pak(const std::filesystem::path& pak_file, const std::function<void(CSpriteLoader&, PAKLib::pak&)>& use) {
        CSpriteLoader Loader;
        Loader._current_pak = PAKLib::loadpak_fast(pak_file.string());
        Loader._is_pak_open = true;

        try {
            use(Loader, Loader._current_pak);
        }
        catch (...) {
            Loader._is_pak_open = false;
            throw;
        }

        Loader._is_pak_open = false;
    }

	std::unique_ptr<CSprite> get_sprite(size_t index) {
		if (!_is_pak_open)
			throw std::runtime_error("No PAK file is currently open.");
		if (index >= _current_pak.sprites.size())
			throw std::out_of_range("Sprite index is out of range.");
		return CSprite::create(_current_pak, index);
	}

private:
	bool _is_pak_open = false;
	PAKLib::pak _current_pak;
};

class CSpriteCollection {
public:
    class SpriteProxy {
    private:
        CSpriteCollection& _collection;
        size_t _index;

    public:
        SpriteProxy(CSpriteCollection& coll, size_t idx) : _collection(coll), _index(idx) {}

        CSprite* operator->() {
            return _collection.get(_index);
        }

        const CSprite* operator->() const {
            return _collection.get(_index);
        }

        SpriteProxy& operator=(const CSprite& sprite) {
            _collection.set(_index, sprite);
            return *this;
        }

        SpriteProxy& operator=(CSprite&& sprite) {
            _collection.set(_index, std::move(sprite));
            return *this;
        }

        SpriteProxy& operator=(std::unique_ptr<CSprite> sprite) {
            _collection.set(_index, std::move(sprite));
            return *this;
        }

        operator bool() const {
            return _collection.get(_index) != nullptr;
        }
    };

    SpriteProxy operator[](size_t index) {
        return SpriteProxy(*this, index);
    }

    const CSprite* operator[](size_t index) const {
        return get(index);
    }

    CSprite* get(size_t index) {
        auto It = _sprites.find(index);
        if (It != _sprites.end()) {
            return It->second.get();
        }
        return nullptr;
    }

    const CSprite* get(size_t index) const {
        auto It = _sprites.find(index);
        if (It != _sprites.end()) {
            return It->second.get();
        }
        return nullptr;
    }

    void set(size_t index, const CSprite& sprite) {
        _sprites[index] = std::make_unique<CSprite>(sprite);
    }

    void set(size_t index, CSprite&& sprite) {
        _sprites[index] = std::make_unique<CSprite>(std::move(sprite));
    }

    void set(size_t index, std::unique_ptr<CSprite> sprite) {
        _sprites[index] = std::move(sprite);
    }

    void remove(size_t index) {
        _sprites.erase(index);
    }

    void clear() {
        _sprites.clear();
    }

    size_t size() const {
        return _sprites.size();
    }

    bool contains(size_t index) const {
        return _sprites.find(index) != _sprites.end();
    }

    auto begin() { return _sprites.begin(); }
    auto end() { return _sprites.end(); }
    auto cbegin() const { return _sprites.cbegin(); }
    auto cend() const { return _sprites.cend(); }
    auto begin() const { return _sprites.begin(); }
    auto end() const { return _sprites.end(); }

private:
    std::unordered_map<size_t, std::unique_ptr<CSprite>> _sprites;
};