#include "RefurekuGenerator/CodeGen/GeneratedEnumCodeTemplate.h"

#include <cassert>

#include <Kodgen/Parsing/FileParserFactoryBase.h>	//For FileParser::parsingMacro

using namespace rfk;

void GeneratedEnumCodeTemplate::generateCode(kodgen::GeneratedFile& generatedFile, kodgen::EntityInfo& entityInfo, kodgen::FileGenerationUnit& fgu, std::string& out_errorDescription) const noexcept
{
	GeneratedEntityCodeTemplate::generateCode(generatedFile, entityInfo, fgu, out_errorDescription);

	assert(entityInfo.entityType == kodgen::EEntityType::Enum);

	generateEnumCode(generatedFile, static_cast<kodgen::EnumInfo&>(entityInfo));
}

void GeneratedEnumCodeTemplate::generateEnumCode(kodgen::GeneratedFile& generatedFile, kodgen::EnumInfo& enumInfo) const noexcept
{
	std::string	mainMacroName			= internalPrefix + getEntityId(enumInfo) + "_GENERATED";

	std::string specializationMacroName	= generateGetEnumSpecialization(generatedFile, enumInfo);
	std::string registerMacroName		= generateRegistrationMacro(generatedFile, enumInfo);

	//Use parsing macro to avoid parsing generated data
	generatedFile.writeLine("#ifdef " + kodgen::FileParserFactoryBase::parsingMacro);

	generatedFile.writeMacro(std::string(mainMacroName));

	generatedFile.writeLine("#else");

	generatedFile.writeMacro(std::move(mainMacroName),
							 "namespace rfk {",
								std::move(specializationMacroName),
								std::move(registerMacroName),
							 "}");

	generatedFile.writeLine("#endif\n");
}

std::string GeneratedEnumCodeTemplate::generateGetEnumSpecialization(kodgen::GeneratedFile& generatedFile, kodgen::EnumInfo& enumInfo) const noexcept
{
	std::string entityId	= getEntityId(enumInfo);
	std::string macroName	= internalPrefix + entityId + "_GenerateGetEnumSpecialization";
	std::string typeName	= enumInfo.type.getCanonicalName();
	std::string properties;

	generatedFile.writeLines("#define " + macroName + "\t\\",
							 "	template <>\t\\",
							 "	inline rfk::Enum const* getEnum<" + typeName + ">() noexcept\t\\",
							 "	{\t\\",
							 "		static bool			initialized = false;\t\\",
							 "		static rfk::Enum	type(\"" + enumInfo.name + "\", " +
															 entityId + ", "
															 "sizeof(" + typeName + "), "
															 "rfk::Type::getType<" + enumInfo.underlyingType.getCanonicalName() + ">());\t\\");
	
	generatedFile.writeLines("		if (!initialized)\t\\",
							 "		{\t\\",
							 "			initialized = true;\t\\");

	//Fill enum properties
	properties = fillEntityProperties(enumInfo, "type.");
	if (!properties.empty())
	{
		generatedFile.writeLine("			" + properties + "\t\\");
	}

	if (!enumInfo.enumValues.empty())
	{
		generatedFile.writeLines("			rfk::EnumValue*	enumValue = nullptr;\t\\",
								 "			type.values.reserve(" + std::to_string(enumInfo.enumValues.size()) + ");\t\\");

		for (kodgen::EnumValueInfo& enumValue : enumInfo.enumValues)
		{
			generatedFile.writeLine("			enumValue = type.addEnumValue(\"" + enumValue.name + "\", " + std::to_string(stringHasher(enumValue.id)) + "u, " + std::to_string(enumValue.defaultValue) + ");\t\\");

			//Fill enum value properties
			enumValue.properties.removeStartAndTrailSpaces();
			properties = fillEntityProperties(enumValue, "enumValue->");
			if (!properties.empty())
			{
				generatedFile.writeLine("			" + properties + "\t\\");
			}
		}
	}

	generatedFile.writeLine("		}\t\\");

	generatedFile.writeLines("		return &type;\t\\",
							 "	}",
							 "");

	return macroName;
}

std::string GeneratedEnumCodeTemplate::generateRegistrationMacro(kodgen::GeneratedFile& generatedFile, kodgen::EnumInfo const& enumInfo) const noexcept
{
	//Don't register to database if the class is contained in another entity (namespace / class)
	if (enumInfo.outerEntity != nullptr)
	{
		return std::string();
	}
	else
	{
		std::string entityId	= getEntityId(enumInfo);
		std::string macroName	= internalPrefix + entityId + "_RegisterEnum";

		//Wrap into a generated namespace to avoid pollution in rfk namespace
		generatedFile.writeMacro(std::string(macroName),
								 "namespace generated { inline rfk::ArchetypeRegisterer registerer" + entityId + " = rfk::getEnum<" + enumInfo.type.getCanonicalName() + ">(); }");

		return macroName;
	}
}