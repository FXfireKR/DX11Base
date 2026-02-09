#pragma once
#include <memory>
#include <vector>
#include <cstdint>
#include <cassert>

template <typename T>
class sparse_set
{
private:
	struct DenseEntry {
		std::unique_ptr<T> data;
		uint32_t id;
	};

public:
	sparse_set() {}
	sparse_set(size_t reserve) {
		this->reserve(reserve);
	}
	~sparse_set() {}

	template<typename... Args>
	uint32_t add(Args&&... args)
	{
		uint32_t id;
		if (free.empty()) {
			id = static_cast<uint32_t>(dense.size());
			dense.push_back({ std::make_unique<T>(std::forward<Args>(args)...) , id});
			sparse.push_back(id);
		}
		else {
			id = free.back();
			free.pop_back();
			dense[id] = { std::make_unique<T>(std::forward<Args>(args)...) , id};
			sparse[id] = id;
		}
		return id;
	}

	T* get(uint32_t id)
	{
		if (id < sparse.size()) {
			uint32_t denseIndex = sparse[id];
			if (denseIndex < dense.size()) {
				return dense[denseIndex].data.get();
			}
		}
		return nullptr;
	}

	const T* get(uint32_t id) const
	{
		if (id < sparse.size()) {
			uint32_t denseIndex = sparse[id];
			if (denseIndex < dense.size()) {
				return dense[denseIndex].data.get();
			}
		}
		return nullptr;
	}

	void remove(uint32_t id)
	{
		if (id < sparse.size())
		{
			uint32_t denseIndex = sparse[id];
			if (denseIndex < dense.size() && dense[denseIndex])
			{
				uint32_t lastIndex = static_cast<uint32_t>(dense.size() - 1);
				if (denseIndex != lastIndex)
				{
					std::swap(dense[denseIndex], dense[lastIndex]);
					sparse[dense[denseIndex].id] = denseIndex;
				}
				dense.pop_back();
				sparse[id] = UINT32_MAX;
				free.push_back(id);
			}
		}
	}

	void reserve(size_t resv)
	{
		if (dense.capacity() < resv) {
			sparse.reserve(resv);
			free.reserve(resv);
			dense.reserve(resv);
		}
	}

	const size_t size() const { return dense.size(); }
	const size_t capacity() const { return dense.capacity(); }
	const bool empty() const { return dense.empty(); }

private:
	std::vector<uint32_t> sparse;
	std::vector<uint32_t> free;
	std::vector<DenseEntry> dense;
};