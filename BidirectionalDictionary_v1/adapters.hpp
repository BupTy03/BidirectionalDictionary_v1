#pragma once

#include <functional>


template<typename Comp = std::less<>>
struct ComparePairByFirstAdapter {
	ComparePairByFirstAdapter() = default;
	explicit ComparePairByFirstAdapter(Comp comp) : comp_{ std::move(comp) } {}

	template<typename FirstType, typename SecondType>
	bool operator()(const std::pair<FirstType, SecondType>& lhs, const std::pair<FirstType, SecondType>& rhs) const {
		return comp_(lhs.first, rhs.first);
	}

	template<typename FirstType, typename SecondType>
	bool operator()(const std::pair<FirstType, SecondType>& lhs, const FirstType& rhs) const {
		return comp_(lhs.first, rhs);
	}

	template<typename FirstType, typename SecondType>
	bool operator()(const FirstType& lhs, const std::pair<FirstType, SecondType>& rhs) const {
		return comp_(lhs, rhs.first);
	}

private:
	Comp comp_;
};


template<typename Comp = std::less<>>
struct ComparePairBySecondAdapter {
	ComparePairBySecondAdapter() = default;
	explicit ComparePairBySecondAdapter(Comp comp) : comp_{ std::move(comp) } {}

	template<typename FirstType, typename SecondType>
	bool operator()(const std::pair<FirstType, SecondType>& lhs, const std::pair<FirstType, SecondType>& rhs) const {
		return comp_(lhs.second, rhs.second);
	}

	template<typename FirstType, typename SecondType>
	bool operator()(const std::pair<FirstType, SecondType>& lhs, const SecondType& rhs) const {
		return comp_(lhs.second, rhs);
	}

	template<typename FirstType, typename SecondType>
	bool operator()(const SecondType& lhs, const std::pair<FirstType, SecondType>& rhs) const {
		return comp_(lhs, rhs.second);
	}

private:
	Comp comp_;
};
