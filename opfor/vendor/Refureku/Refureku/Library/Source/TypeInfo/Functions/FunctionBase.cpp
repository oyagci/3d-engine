#include "Refureku/TypeInfo/Functions/FunctionBase.h"

using namespace rfk;

FunctionBase::FunctionBase(std::string&& name, uint64 id, EEntityKind kind, Type const& returnType, std::unique_ptr<ICallable>&& internalMethod) noexcept:
	Entity(std::forward<std::string>(name), id, kind),
	internalMethod{std::forward<std::unique_ptr<ICallable>>(internalMethod)},
	returnType{returnType}
{
}

bool FunctionBase::hasSamePrototype(FunctionBase const* other) const noexcept
{
	//Compare return type
	if (returnType != other->returnType)
	{
		return false;
	}

	//Compare parameters
	if (parameters.size() != other->parameters.size())
	{
		return false;
	}

	for (size_t i = 0u; i < parameters.size(); i++)
	{
		if (parameters[i].type != other->parameters[i].type)
		{
			return false;
		}
	}

	return true;
}

FunctionBase* FunctionBase::addParameter(std::string parameterName, Type const& parameterType) noexcept
{
	parameters.emplace_back(std::move(parameterName), parameterType);

	return this;
}

ICallable const* FunctionBase::getInternalFunction() const noexcept
{
	return internalMethod.get();
}