#include "Refureku/TypeInfo/Functions/MethodBase.h"

using namespace rfk;

MethodBase::MethodBase(std::string&& name, uint64 id, Type const& returnType, std::unique_ptr<ICallable>&& internalMethod, EMethodFlags flags) noexcept:
	FunctionBase(std::forward<std::string>(name), id, EEntityKind::Method, returnType, std::forward<std::unique_ptr<ICallable>>(internalMethod)),
	flags{flags}
{
}

EAccessSpecifier MethodBase::getAccess() const noexcept
{
	using UnderlyingType = std::underlying_type_t<EMethodFlags>;

	return	(static_cast<UnderlyingType>(flags & EMethodFlags::Public)) ? EAccessSpecifier::Public :
			(static_cast<UnderlyingType>(flags & EMethodFlags::Protected)) ? EAccessSpecifier::Protected :
			(static_cast<UnderlyingType>(flags & EMethodFlags::Private)) ? EAccessSpecifier::Private :
			EAccessSpecifier::Undefined;
}