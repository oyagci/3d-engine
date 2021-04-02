#include "Kodgen/Properties/NativeProperties/GenCodeTemplatePropertyRule.h"

#include "Kodgen/Properties/NativeProperties/NativeProperties.h"

using namespace kodgen;

GenCodeTemplatePropertyRule::GenCodeTemplatePropertyRule() noexcept:
	FixedComplexPropertyRule(NativeProperties::generatedCodeTemplateProperty,
							 EEntityType::Namespace | EEntityType::Struct | EEntityType::Class | EEntityType::Field | EEntityType::Method | EEntityType::Enum | EEntityType::EnumValue)
{
	subProperties.emplace_back(ComplexPropertySubProperty::EType::String, "generatedCodeTemplateName", "Name of the registered generated code template to use for this entity.");
}

bool GenCodeTemplatePropertyRule::isSubPropSyntaxValid(std::string const& subProperty, uint8 subPropIndex, std::string& out_errorDescription) const noexcept
{
	if (!FixedComplexPropertyRule::isSubPropSyntaxValid(subProperty, subPropIndex, out_errorDescription))
	{
		return false;
	}

	if (validTemplateNames.find(subProperty.substr(1u, subProperty.size() - 2u)) == validTemplateNames.cend())	//substr remove start and end ""
	{
		out_errorDescription = "The provided generated code template has not been registered to the FileGenerator.";

		return false;
	}

	return true;
}

bool GenCodeTemplatePropertyRule::isPropertyGroupValid(PropertyGroup const& propertyGroup, uint8 propertyIndex, std::string& out_errorDescription) const noexcept
{
	return	FixedComplexPropertyRule::isPropertyGroupValid(propertyGroup, propertyIndex, out_errorDescription) &&
			isUsedOnlyOnce(propertyGroup, propertyIndex, out_errorDescription);
}