#include "RefurekuGenerator/CodeGen/GeneratedVariableCodeTemplate.h"

#include <cassert>

#include <Kodgen/Parsing/FileParserFactoryBase.h>

using namespace rfk;

std::string GeneratedVariableCodeTemplate::generateGetVariableDefinition(kodgen::GeneratedFile& generatedFile, kodgen::VariableInfo const& varInfo) const noexcept
{
	std::string entityId	= getEntityId(varInfo);
	std::string macroName	= internalPrefix + entityId + "_GenerateGetVariableDefinition";
	std::string properties;

	generatedFile.writeLines("#define " + macroName + "\t\\",
							 "	inline rfk::Variable const& " + getGetVariableFunctionName(varInfo) + "() noexcept\t\\",
							 "\t{\t\\",
							 "\t\tstatic bool initialized = false;\t\\",
							 "\t\tstatic rfk::Variable variable(\"" + varInfo.name + "\", " +
																	 entityId + ", "
																	 "rfk::Type::getType<" + varInfo.type.getCanonicalName() + ">(), "
																	 "&" + varInfo.getFullName() + ", "
																	 "static_cast<rfk::EVarFlags>(" + std::to_string(computeVarFlags(varInfo)) + ")"
																	 ");\t\\");

	generatedFile.writeLines("\t\tif (!initialized)\t\\",
							 "\t\t{\t\\",
							 "\t\t\tinitialized = true;\t\\");

	//Fill enum properties
	properties = fillEntityProperties(varInfo, "variable.");
	if (!properties.empty())
	{
		generatedFile.writeLine("\t\t\t" + properties + "\t\\");
	}

	generatedFile.writeLine("\t\t}\t\\");

	generatedFile.writeLines("\t\treturn variable;\t\\",
							 "\t}\n");

	return macroName;
}

std::string GeneratedVariableCodeTemplate::generateRegistrationMacro(kodgen::GeneratedFile& generatedFile, kodgen::VariableInfo const& varInfo) const noexcept
{
	//Don't register to database if the class is contained in another entity (namespace / class)
	if (varInfo.outerEntity != nullptr)
	{
		return std::string();
	}
	else
	{
		std::string entityId	= getEntityId(varInfo);
		std::string macroName	= internalPrefix + entityId + "_RegisterVariable";

		generatedFile.writeMacro(std::string(macroName),
								 "inline rfk::DefaultEntityRegisterer registerer" + entityId + " = &getVariable" + entityId + "();");

		return macroName;
	}
}

void GeneratedVariableCodeTemplate::generateCode(kodgen::GeneratedFile& generatedFile, kodgen::EntityInfo& entityInfo, kodgen::FileGenerationUnit& fgu, std::string& out_errorDescription) const noexcept
{
	GeneratedEntityCodeTemplate::generateCode(generatedFile, entityInfo, fgu, out_errorDescription);

	assert(entityInfo.entityType == kodgen::EEntityType::Variable);

	kodgen::VariableInfo const& varInfo = static_cast<kodgen::VariableInfo const&>(entityInfo);

	std::string	mainMacroName		= internalPrefix + getEntityId(entityInfo) + "_GENERATED";

	std::string variableDefinition	= generateGetVariableDefinition(generatedFile, varInfo);
	std::string variableRegisterer	= generateRegistrationMacro(generatedFile, varInfo);

	//Use parsing macro to avoid parsing generated data
	generatedFile.writeLine("#ifdef " + kodgen::FileParserFactoryBase::parsingMacro);

	generatedFile.writeMacro(std::string(mainMacroName));

	generatedFile.writeLine("#else");

	generatedFile.writeMacro(std::move(mainMacroName),
							 "namespace rfk::generated {",
							 std::move(variableDefinition),
							 std::move(variableRegisterer),
							 "}");

	generatedFile.writeLine("#endif\n");
}

kodgen::uint8 GeneratedVariableCodeTemplate::computeVarFlags(kodgen::VariableInfo const& varInfo) const noexcept
{
	kodgen::uint8 result = 0u;

	if (varInfo.isStatic)
	{
		result |= 1 << 0;
	}

	return result;
}

std::string GeneratedVariableCodeTemplate::getGetVariableFunctionName(kodgen::VariableInfo const& varInfo) noexcept
{
	return "getVariable" + std::to_string(stringHasher(varInfo.id)) + "u";
}