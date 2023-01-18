#pragma once

#include <Tether/Common/HashedString.hpp>
#include <Tether/Common/TypeTools.hpp>

namespace Tether
{
	template<typename T>
	class HashedType : public HashedString
	{
	public:
		HashedType()
			:
			HashedString(TypeTools::GetTypeName<T>())
		{}
	};
}