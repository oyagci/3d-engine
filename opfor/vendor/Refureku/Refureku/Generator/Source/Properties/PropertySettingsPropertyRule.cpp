#include "RefurekuGenerator/Properties/PropertySettingsPropertyRule.h"

#include <cassert>

#include <Kodgen/InfoStructures/MethodInfo.h>
#include <Kodgen/Properties/ComplexProperty.h>

#include "RefurekuGenerator/Properties/NativeProperties.h"
#include "RefurekuGenerator/Properties/CodeGenData/PropertyCodeGenData.h"

using namespace rfk;

PropertySettingsPropertyRule::PropertySettingsPropertyRule() noexcept:
	DefaultComplexPropertyRule(NativeProperties::propertySettingsProperty, kodgen::EEntityType::Struct | kodgen::EEntityType::Class)
{
}

std::string PropertySettingsPropertyRule::generatePrePropertyAddCode(kodgen::EntityInfo const& entity, kodgen::ComplexProperty const& /* property */, PropertyCodeGenPropertyAddData& /* data */) const noexcept
{
	return "static_assert(std::is_base_of_v<rfk::Property, " + entity.name + ">, \"[Refureku] Can't attach " + mainPropName + " property to " + entity.getFullName() + " as it doesn't inherit from rfk::Property.\");";
}

std::string PropertySettingsPropertyRule::generateClassFooterCode(kodgen::EntityInfo const& /* entity */, kodgen::ComplexProperty const& property, PropertyCodeGenClassFooterData& /* data */) const noexcept
{
	std::string result;

	//Generate constexpr fields + get overrides
	if (!property.subProperties.empty())
	{
		//generate targetEntityKind
		result += "public: static constexpr rfk::EEntityKind targetEntityKind = " + property.subProperties[0] + ";";
		result += "virtual rfk::EEntityKind getTargetEntityKind() const noexcept override { return targetEntityKind; }";

		if (property.subProperties.size() >= 2)
		{
			//generate allowMultiple
			result += "static constexpr bool allowMultiple = " + property.subProperties[1] + ";";
			result += "virtual bool getAllowMultiple() const noexcept override { return allowMultiple; }";

			if (property.subProperties.size() >= 3)
			{
				//generate shouldInherit
				result += "static constexpr bool shouldInherit = " + property.subProperties[2] + ";";
				result += "virtual bool getShouldInherit() const noexcept override { return shouldInherit; }";
			}
		}
	}

	return result;
}