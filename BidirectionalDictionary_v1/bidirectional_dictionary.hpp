#pragma once

#include "adapters.hpp"

#include <utility>
#include <vector>
#include <algorithm>
#include <stdexcept>


template<typename First, typename Second, typename HashFuncFirst = std::hash<First>, typename HashFuncSecond = std::hash<Second>>
struct bidirectinal_dictionary
{
	template<typename F, typename S>
	void emplace(F&& first, S&& second)
	{
		const std::size_t firstHashed = HashFuncFirst()(first);
		const std::size_t secondHashed = HashFuncSecond()(second);
		const std::size_t currentPairIndex = std::size(data_);

		firstIndexes_.emplace(std::lower_bound(std::cbegin(firstIndexes_), std::cend(firstIndexes_),
			firstHashed, ComparePairByFirstAdapter<>()), firstHashed, currentPairIndex);

		secondIndexes_.emplace(std::lower_bound(std::cbegin(secondIndexes_), std::cend(secondIndexes_),
			secondHashed, ComparePairByFirstAdapter<>()), secondHashed, currentPairIndex);

		data_.emplace_back(std::forward<F>(first), std::forward<S>(second));
	}

	const Second& second(const First& first) const
	{
		const std::size_t hashedFirst = HashFuncFirst()(first);
		const auto it = std::lower_bound(std::cbegin(firstIndexes_), std::cend(firstIndexes_), 
			hashedFirst, ComparePairByFirstAdapter<>());

		if (it == std::cend(firstIndexes_) || it->first != hashedFirst)
			throw std::out_of_range{"first not found"};

		return data_.at(it->second).second;
	}

	const First& first(const Second& second) const
	{
		const std::size_t hashedSecond = HashFuncSecond()(second);
		const auto it = std::lower_bound(std::cbegin(secondIndexes_), std::cend(secondIndexes_),
			hashedSecond, ComparePairByFirstAdapter<>());

		if (it == std::cend(secondIndexes_) || it->first != hashedSecond)
			throw std::out_of_range{"second not found"};

		return data_.at(it->second).first;
	}

private:
	std::vector<std::pair<std::size_t, std::size_t>> firstIndexes_;
	std::vector<std::pair<std::size_t, std::size_t>> secondIndexes_;
	std::vector<std::pair<First, Second>> data_;
};