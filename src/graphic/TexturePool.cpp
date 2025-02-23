﻿#include <bgfx/bgfx.h>
#include <stb/stb_image.h>
#include <unordered_map>
#include <memory>

#include "TexturePool.hpp"

//MASSIVE todo: store texture in one texture atlas and use textureId to index coordinates.
//as well as dynamically managing texture size, allowing registering 32x32 or bigger texture.
namespace TexturePool {
	typedef uint8_t u8;
	typedef uint32_t u32;
	typedef u32 TextureID;

	using std::unordered_map, bgfx::UniformHandle, bgfx::createUniform, bgfx::destroy;

	unordered_map<TextureID, Texture> registry;
	TextureID nextId;
	UniformHandle sampler;

	void init(const char* samplerName) {
		sampler = createUniform(samplerName, bgfx::UniformType::Sampler);
		nextId = 1;
	}

	void shutdown() {
		destroy(sampler);
		for (const auto& pair : registry) {
			destroy(pair.second.handle);
			stbi_image_free(pair.second.raw);
		}
	}

	TextureID addTexture(const char* filePath, bool noVerticalFilp) {
		Texture texture{};
		if (!noVerticalFilp) stbi_set_flip_vertically_on_load(1);
		else stbi_set_flip_vertically_on_load(0);
		texture.raw = stbi_load(filePath, &texture.height, &texture.width, &texture.bitsPerPixel, 4);
		texture.handle = bgfx::createTexture2D(
			uint16_t(texture.width),
			uint16_t(texture.height),
			false, 1,
			bgfx::TextureFormat::RGBA8,
			BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT,
			bgfx::makeRef(texture.raw, texture.height * texture.width * texture.bitsPerPixel)
		);
		registry.emplace(nextId, texture);
		nextId++;
		return nextId - 1;
	}

	void useTexture(TextureID id, u8 textureDataIndex) {
		auto p = registry.find(id);
		//todo: A special internal texture ided `0` will be added shortly to allow fallback (probably black and purple-ish?).
		if (p == registry.end()) return; //throw ERROR_TEXTURE_NOT_EXIST;
		bgfx::setTexture(textureDataIndex, sampler, p->second.handle);
	}

	void removeTexture(TextureID id) {
		auto p = registry.find(id);
		if (p == registry.end()) return;
		destroy(p->second.handle);
		stbi_image_free(p->second.raw);
		registry.erase(p);
	}
}