#pragma once

#include "adapters.hpp"

#include <utility>
#include <vector>
#include <algorithm>
#include <stdexcept>


template<
	typename First, 
	typename Second, 
	typename HashFuncFirst = std::hash<First>, 
	typename HashFuncSecond = std::hash<Second>,
	typename EqualFuncFirst = std::equal_to<First>,
	typename EqualFuncSecond = std::equal_to<Second>
>
class bidirectinal_dictionary
{
	using IndexPairs = std::vector<std::pair<std::size_t, std::size_t>>;
public:

	template<typename F, typename S>
	void emplace(F&& first, S&& second)
	{
		const std::size_t currentPairIndex = std::size(data_);
		data_.emplace_back(std::forward<F>(first), std::forward<S>(second));

		hash_first(currentPairIndex, data_.back().first);
		hash_second(currentPairIndex, data_.back().second);
	}

	const Second& second(const First& first) const
	{
		const std::size_t hashedFirst = HashFuncFirst()(first);
		const auto indexIt = std::lower_bound(std::cbegin(firstIndexes_), std::cend(firstIndexes_), 
			hashedFirst, ComparePairByFirstAdapter<>());

		if (indexIt == std::cend(firstIndexes_) || indexIt->first != hashedFirst)
			throw std::out_of_range{"first not found"};

		EqualFuncFirst equalFirst;
		if (equalFirst(data_.at(indexIt->second).first, first))
			return data_.at(indexIt->second).second;

		auto collisionsFirst = first_collisions_range(hashedFirst);
		if(collisionsFirst == std::cend(firstCollisions_) || collisionsFirst->first != hashedFirst)
			return data_.at(indexIt->second).second;

		for (; collisionsFirst != std::cend(firstCollisions_) && collisionsFirst->first == hashedFirst; ++collisionsFirst)
		{
			const auto& currentPair = data_.at(collisionsFirst->second);
			if (equalFirst(currentPair.first, first))
				return currentPair.second;
		}

		throw std::out_of_range{ "first not found" };
	}

	const First& first(const Second& second) const
	{
		const std::size_t hashedSecond = HashFuncSecond()(second);
		const auto indexIt = std::lower_bound(std::cbegin(secondIndexes_), std::cend(secondIndexes_),
			hashedSecond, ComparePairByFirstAdapter<>());

		if (indexIt == std::cend(secondIndexes_) || indexIt->first != hashedSecond)
			throw std::out_of_range{ "second not found" };

		EqualFuncSecond equalSecond;
		if (equalSecond(data_.at(indexIt->second).second, second))
			return data_.at(indexIt->second).first;

		auto collisionsSecond = second_collisions_range(hashedSecond);
		if (collisionsSecond == std::cend(secondCollisions_) || collisionsSecond->first != hashedSecond)
			return data_.at(indexIt->second).first;

		for (; collisionsSecond != std::cend(secondCollisions_) && collisionsSecond->first == hashedSecond; ++collisionsSecond)
		{
			const auto& currentPair = data_.at(collisionsSecond->second);
			if (equalSecond(currentPair.second, second))
				return currentPair.first;
		}

		throw std::out_of_range{ "second not found" };
	}

private:
	void hash_first(std::size_t lastInsertedIndex, const First& first)
	{
		const std::size_t firstHashed = HashFuncFirst()(first);
		const auto it = std::lower_bound(std::cbegin(firstIndexes_), std::cend(firstIndexes_),
			firstHashed, ComparePairByFirstAdapter<>());

		// handle collision
		if (it != std::cend(firstIndexes_) && it->first == firstHashed)
		{
			EqualFuncFirst equalFirst;
			if (equalFirst(data_.at(it->second).first, first))
				throw std::invalid_argument{ "this pair already exists" };

			auto collisionIt = first_collisions_range(firstHashed);
			for (; collisionIt != std::cend(firstCollisions_) && collisionIt->first == firstHashed; ++collisionIt)
			{
				if(equalFirst(data_.at(collisionIt->second).first, first))
					throw std::invalid_argument{ "this pair already exists" };
			}

			add_first_collision(firstHashed, lastInsertedIndex);
			return;
		}

		firstIndexes_.emplace(it, firstHashed, lastInsertedIndex);
	}

	void hash_second(std::size_t lastInsertedIndex, const Second& second)
	{
		const std::size_t secondHashed = HashFuncSecond()(second);
		const auto it = std::lower_bound(std::cbegin(secondIndexes_), std::cend(secondIndexes_),
			secondHashed, ComparePairByFirstAdapter<>());

		// handle collision
		if (it != std::cend(secondIndexes_) && it->first == secondHashed)
		{
			EqualFuncSecond equalSecond;
			if (equalSecond(data_.at(it->second).second, second))
				throw std::invalid_argument{ "second already exists" };

			auto collisionIt = second_collisions_range(secondHashed);
			for (; collisionIt != std::cend(secondCollisions_) && collisionIt->first == secondHashed; ++collisionIt)
			{
				if (equalSecond(data_.at(collisionIt->second).second, second))
					throw std::invalid_argument{ "second already exists" };
			}

			add_second_collision(secondHashed, lastInsertedIndex);
			return;
		}

		secondIndexes_.emplace(it, secondHashed, lastInsertedIndex);
	}

	void add_first_collision(std::size_t hashed, std::size_t index)
	{
		firstCollisions_.emplace(std::lower_bound(
			std::cbegin(firstCollisions_), 
			std::cend(firstCollisions_), 
			hashed, ComparePairByFirstAdapter<>()), hashed, index);
	}

	void add_second_collision(std::size_t hashed, std::size_t index)
	{
		secondCollisions_.emplace(std::lower_bound(
			std::cbegin(secondCollisions_), 
			std::cend(secondCollisions_), 
			hashed, ComparePairByFirstAdapter<>()), hashed, index);
	}

	IndexPairs::const_iterator first_collisions_range(std::size_t hashed) const
	{
		return std::lower_bound(std::cbegin(firstCollisions_), 
			std::cend(firstCollisions_), hashed, ComparePairByFirstAdapter<>());
	}

	IndexPairs::const_iterator second_collisions_range(std::size_t hashed) const
	{
		return std::lower_bound(std::cbegin(secondCollisions_), 
			std::cend(secondCollisions_), hashed, ComparePairByFirstAdapter<>());
	}

private:
	IndexPairs firstIndexes_;
	IndexPairs secondIndexes_;
	IndexPairs firstCollisions_;
	IndexPairs secondCollisions_;
	std::vector<std::pair<First, Second>> data_;
};