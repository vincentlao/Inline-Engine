#include "EntityStore.hpp"

#include <BaseLibrary/Exception/Exception.hpp>
#include <BaseLibrary/HashCombine.hpp>


namespace inl::game {

void EntityStore::Insert(EntityStore others) {
	if (!CompareTypes(others)) {
		throw InvalidArgumentException("The two stores must contain entities with the same set of components.");
	}

	m_size += others.SizeEntities();

	auto myIt = m_components.begin();
	auto myEnd = m_components.end();
	auto otherIt = m_components.begin();
	auto otherEnd = m_components.end();

	while (myIt != myEnd && otherIt != otherEnd) {
		myIt->second.insert(myIt->second.container, std::move(otherIt->second.container));
		++myIt;
		++otherIt;
	}
}


void EntityStore::Erase(size_t index) {
	for (auto& vec : m_components) {
		vec.second.erase(vec.second.container, index);
	}
	--m_size;
}

EntityStore EntityStore::Extract(size_t index) {
	EntityStore extracted;

	// Double iteration to keep the order of the multiset strict.
	for (auto& vec : m_components) {
		extracted.m_components.insert({ vec.first, ComponentVector{ std::any{}, vec.second.insert, vec.second.erase, vec.second.extract } });
	}

	auto extractedIt = m_components.begin();
	for (auto& vec : m_components) {
		extractedIt->second.container = vec.second.extract(vec.second.container, index);
		++extractedIt;
	}

	--m_size;
	return extracted;
}


size_t EntityStore::SizeEntities() const {
	return m_size;
}


size_t EntityStore::SizeComponentTypes() const {
	return m_components.size();
}


bool EntityStore::CompareTypes(const EntityStore& other) {
	if (m_components.size() != other.m_components.size()) {
		return false;
	}

	auto myIt = m_components.begin();
	auto myEnd = m_components.end();
	auto otherIt = m_components.begin();
	auto otherEnd = m_components.end();

	while (myIt != myEnd && otherIt != otherEnd) {
		if (myIt->first != otherIt->first) {
			return false;
		}
		++myIt;
		++otherIt;
	}
	return true;
}


size_t EntityStore::HashTypes() const {
	size_t hash = 17;
	for (auto& v : m_components) {
		hash = CombineHash(hash, v.first.hash_code());
	}
	return hash;
}


} // namespace inl::game