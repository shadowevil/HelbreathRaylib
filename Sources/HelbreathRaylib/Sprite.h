#pragma once
#include "raylib_include.h"
#include "PAK.h"
#include "global_constants.h"
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include <memory>

class CSprite {
public:
	inline static std::unique_ptr<CSprite> Create(const PAKLib::pak& pak, size_t index) {
		std::unique_ptr<CSprite> sprite(new CSprite());

		std::string filename = get_filename(pak.pak_file_path);
		std::string filepath = path_combine(constant::SPRITE_PATH, filename);

		sprite->m_sprite_rectangles = pak.sprites[index].sprite_rectangles;
		sprite->file_path = filepath;
		sprite->sprite_index = index;
		return sprite;
	}

	~CSprite() {
		UnloadTexture(m_texture);
	}

	void Preload() {
		last_used_time = GetTime();
		if (IsTextureValid(m_texture))
			return;

		const auto& sprite_obj = PAKLib::get_sprite_fast(file_path, sprite_index);
		Image image = LoadImageFromMemory(constant::SPRITE_IMAGE_TYPE.c_str(), sprite_obj.image_data.data(), static_cast<int>(sprite_obj.image_data.size()));
		m_texture = LoadTextureFromImage(image);
		SetTextureFilter(m_texture, TEXTURE_FILTER_POINT);
		UnloadImage(image);
	}

	void UnloadIfUnused(double current_time, double timeout_seconds) {
		if (IsTextureValid(m_texture) && (current_time - last_used_time) >= timeout_seconds) {
			UnloadTexture(m_texture);
			m_texture = {};
		}
	}

	void Draw(int x, int y, size_t frame);

private:
	CSprite() = default;

protected:
	double last_used_time{};
	std::vector<PAKLib::sprite_rect> m_sprite_rectangles{};
	std::string file_path{};
	size_t sprite_index{};
	Texture2D m_texture{};
};

class CSpriteLoader {
public:
	static void OpenPAK(const std::string& PAKFile, const std::function<void(CSpriteLoader&)>& use) {
		CSpriteLoader loader;
		loader.m_currentPAK = PAKLib::loadpak_fast(
			path_combine(get_executable_dir(), constant::SPRITE_PATH, PAKFile)
		);
		loader.m_isPAKOpen = true;

		try {
			use(loader);
		}
		catch (...) {
			loader.m_isPAKOpen = false;
			throw;
		}

		loader.m_isPAKOpen = false;
	}

	std::unique_ptr<CSprite> GetSprite(size_t index) {
		if (!m_isPAKOpen)
			throw std::runtime_error("No PAK file is currently open.");
		if (index >= m_currentPAK.sprites.size())
			throw std::out_of_range("Sprite index is out of range.");
		return CSprite::Create(m_currentPAK, index);
	}

private:
	bool m_isPAKOpen = false;
	PAKLib::pak m_currentPAK;
};

class CSpriteCollection {
public:
    class SpriteProxy {
    private:
        CSpriteCollection& collection;
        size_t index;

    public:
        SpriteProxy(CSpriteCollection& coll, size_t idx) : collection(coll), index(idx) {}

        CSprite* operator->() {
            return collection.Get(index);
        }

        const CSprite* operator->() const {
            return collection.Get(index);
        }

        SpriteProxy& operator=(const CSprite& sprite) {
            collection.Set(index, sprite);
            return *this;
        }

        SpriteProxy& operator=(CSprite&& sprite) {
            collection.Set(index, std::move(sprite));
            return *this;
        }

        SpriteProxy& operator=(std::unique_ptr<CSprite> sprite) {
            collection.Set(index, std::move(sprite));
            return *this;
        }

        operator bool() const {
            return collection.Get(index) != nullptr;
        }
    };

    SpriteProxy operator[](size_t index) {
        return SpriteProxy(*this, index);
    }

    const CSprite* operator[](size_t index) const {
        return Get(index);
    }

    CSprite* Get(size_t index) {
        auto it = sprites.find(index);
        if (it != sprites.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    const CSprite* Get(size_t index) const {
        auto it = sprites.find(index);
        if (it != sprites.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    void Set(size_t index, const CSprite& sprite) {
        sprites[index] = std::make_unique<CSprite>(sprite);
    }

    void Set(size_t index, CSprite&& sprite) {
        sprites[index] = std::make_unique<CSprite>(std::move(sprite));
    }

    void Set(size_t index, std::unique_ptr<CSprite> sprite) {
        sprites[index] = std::move(sprite);
    }

    void Remove(size_t index) {
        sprites.erase(index);
    }

    void Clear() {
        sprites.clear();
    }

    size_t Size() const {
        return sprites.size();
    }

    bool Contains(size_t index) const {
        return sprites.find(index) != sprites.end();
    }

    auto begin() { return sprites.begin(); }
    auto end() { return sprites.end(); }
    auto cbegin() const { return sprites.cbegin(); }
    auto cend() const { return sprites.cend(); }
    auto begin() const { return sprites.begin(); }
    auto end() const { return sprites.end(); }

private:
    std::unordered_map<size_t, std::unique_ptr<CSprite>> sprites;
};