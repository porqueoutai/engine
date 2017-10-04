/**
 * @file
 */

#pragma once

#include <string>
#include <vector>

namespace persistence {

class Model;

/**
 * @brief Comparators for the @c DBCondition class.
 */
enum class Comparator {
	Equal,
	NotEqual,
	Bigger,
	Lesser,
	BiggerOrEqual,
	LessOrEqual,
	Like,
	In,
	NotIn,

	Max
};

/**
 * @brief A DBCondition is used to build and fill the @c WHERE statements.
 */
class DBCondition {
protected:
	Comparator _comp = Comparator::Max;
	const char* _field = nullptr;
	char *_valueCopy = nullptr;
	const char* _value = nullptr;

	constexpr DBCondition() {}
public:
	constexpr DBCondition(const char* field, const char* value, Comparator comp = Comparator::Equal) :
			_comp(comp), _field(field), _value(value) {
	}

	DBCondition(const char* field, const std::string& value, Comparator comp = Comparator::Equal);

	virtual ~DBCondition();

	/**
	 * @return A string - and never @c nullptr (but can be empty of course) - that the field of the
	 * query is checked against.
	 */
	virtual const char* value(int index) const;

	virtual std::string statement(int& parameterCount) const;
};

inline const char* DBCondition::value(int index) const {
	if (_value == nullptr) {
		return "";
	}
	return _value;
}

/**
 * @brief This is the 'empty' @c DBCondition - it always evaluates to true
 */
class DBConditionOne : public DBCondition {
private:
	using Super = DBCondition;
public:
	constexpr DBConditionOne() :
			Super() {
	}

	std::string statement(int& parameterCount) const override;
};

/**
 * @brief Chain multiple @c DBCondition objects together into one
 */
class DBConditionMultiple : public DBCondition {
private:
	using Super = DBCondition;
	std::vector<const DBCondition*> _conditions;
	bool _and;
public:
	// TODO: somehow make this constexpr
	DBConditionMultiple(bool __and, std::vector<const DBCondition*>&& conditions) :
			Super(), _conditions(conditions), _and(__and) {
	}

	std::string statement(int& parameterCount) const override;
	const char* value(int index) const override;
};

}
