#include "MoreShaderInNodes.h"

void WorldPosNode::WriteMyOutputs(std::string & outCode, Shaders shaderType) const
{
    const DataNodePtr & objPos = GetInputs()[0].GetDataNodeValue();
    std::string outName = GetOutputName(0, shaderType),
                tempName = "temp_" + outName;

    outCode += "\tvec4 " + tempName + " = (" + MaterialConstants::WorldMatName + " * vec4(" + objPos->GetOutputName(0, shaderType) + ", 1.0))\n";
    outCode += "\tvec3 " + outName + " = " + tempName + ".xyz / " + tempName + ".w;\n";
}

void ScreenPosNode::WriteMyOutputs(std::string & outCode, Shaders shaderType) const
{
    const DataNodePtr & objPos = GetInputs()[0].GetDataNodeValue();
    std::string outName = GetOutputName(0, shaderType),
                tempName = "temp_" + outName;

    outCode += "\tvec4 " + tempName + " = (" + MaterialConstants::WVPMatName + " * vec4(" + objPos->GetOutputName(0, shaderType) + ", 1.0))\n";
    outCode += "\tvec3 " + outName + " = " + tempName + ".xyz / " + tempName + ".w;\n";
}


void WorldNormalNode::WriteMyOutputs(std::string & outCode, Shaders shaderType) const
{
    const DataNodePtr & objPos = GetInputs()[0].GetDataNodeValue();
    std::string outName = GetOutputName(0, shaderType);

    outCode += "\tvec3 " + outName + " = (" + MaterialConstants::WorldMatName + " * vec4(" + objPos->GetOutputName(0, shaderType) + ", 0.0)).xyz\n";
}

void ScreenNormalNode::WriteMyOutputs(std::string & outCode, Shaders shaderType) const
{
    const DataNodePtr & objPos = GetInputs()[0].GetDataNodeValue();
    std::string outName = GetOutputName(0, shaderType);

    outCode += "\tvec3 " + outName + " = (" + MaterialConstants::WVPMatName + " * vec4(" + objPos->GetOutputName(0, shaderType) + ", 0.0)).xyz\n";
}