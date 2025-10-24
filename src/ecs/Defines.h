#pragma once 
#include<bitset>
#include<memory>
#include "UUID.h"

namespace lgt {
	
	
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;


	//forward delecration
	struct Archetype;

	// defines for ECS
	using  EntityHandle = uint32_t;
	using  ComponentId = int;
	const  size_t      MAX_ENTITIES = 50000;
	const  ComponentId MAX_COMPONENTS = 32;
	const  ComponentId ComponentIdError = static_cast<ComponentId>(-1);
	using  Signature = std::bitset<MAX_COMPONENTS>;

}