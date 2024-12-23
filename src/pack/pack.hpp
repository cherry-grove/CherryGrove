#pragma once
#include <string>
#include <vector>
#include <array>
#include <memory>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

using std::vector, std::array, std::unique_ptr, std::string;

//statusFlags map
constexpr u64 PACK_STATUS_VALID = 0x1u;
constexpr u64 PACK_STATUS_ENABLED = 0x2u;
constexpr u64 PACK_STATUS_GLOBAL_ENABLED = 0x4u;

//packFlags map
constexpr u64 PACK_WARN_ON_NS_DUPE = 0x1u;
constexpr u64 PACK_ALWAYS_LOAD = 0x2u;
constexpr u64 PACK_LICENSE_LIBRE = 0x4u;
constexpr u64 PACK_ALLOW_INSECURE_EXECUTION = 0x8u;

//featureFlags map
constexpr u64 PACK_FEATURE_COMPONENT = 0x1u;
constexpr u64 PACK_FEATURE_BLOCK = 0x2u;
constexpr u64 PACK_FEATURE_FLUID = 0x4u;
constexpr u64 PACK_FEATURE_ENTITY = 0x8u;
constexpr u64 PACK_FEATURE_ITEM = 0x10u;
constexpr u64 PACK_FEATURE_WORLDGEN = 0x20u;
constexpr u64 PACK_FEATURE_BIOME = 0x40u;
constexpr u64 PACK_FEATURE_STRUCTURE = 0x80u;
constexpr u64 PACK_FEATURE_COMPOSITE = 0x100u;
constexpr u64 PACK_FEATURE_GUI = 0x200u;
constexpr u64 PACK_FEATURE_TEXTURE = 0x400u;
constexpr u64 PACK_FEATURE_SOUND = 0x800u;
constexpr u64 PACK_FEATURE_TEXT = 0x1000u;
constexpr u64 PACK_FEATURE_ACHIEVEMENT = 0x2000u;
constexpr u64 PACK_FEATURE_SHADER = 0x4000u;
//#define PACK_FEATURE_FONT 

struct Pack {
	PackIdentifier identifier;
	u32 minEngineVersion;
	u16 statusFlags;
	u16 packFlags;
	u32 featureFlags;
	//RESERVED, DO NOT USE!
	u32 reserved1;
	//RESERVED, DO NOT USE!
	u32 reserved2;
	unique_ptr<const char> nameSpace;
	//Dependencies are going to 
	vector<PackIdentifier> dependencies;
	PackMeta meta;
};

struct PackIdentifier {
	u8 uuid[16];
	u32 packVersion;
};

struct PackMeta {
	unique_ptr<const char> name;
	unique_ptr<const char> description;
	vector<string> authors;
};