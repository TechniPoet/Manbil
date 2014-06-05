#include "TextureSampleNode.h"

#include "../DataNodeIncludes.h"


std::string TextureSampleNode::GetOutputName(unsigned int index) const
{
    std::string base = GetSampleOutputName();

    switch (index)
    {
        case 0: return base + ".x";
        case 1: return base + ".y";
        case 2: return base + ".z";
        case 3: return base + ".w";
        case 4: return base + ".xyz";
        case 5: return base;
        default:
            Assert(false, std::string() + "Invalid output index: " + ToString(index));
    }
}

unsigned int TextureSampleNode::GetOutputIndex(ChannelsOut channel)
{
    switch (channel)
    {
        case ChannelsOut::CO_Red: return 0;
        case ChannelsOut::CO_Green: return 1;
        case ChannelsOut::CO_Blue: return 2;
        case ChannelsOut::CO_Alpha: return 3;
        case ChannelsOut::CO_AllColorChannels: return 4;
        case ChannelsOut::CO_AllChannels: return 5;

        default: assert(false);
    }
}


TextureSampleNode::TextureSampleNode(const DataLine & uvs, std::string _samplerName)
    : DataNode(MakeVector(uvs), makeVector())
{
    Assert(uvs.GetDataLineSize() == 2,
           std::string() + "UV input isn't size 2; it's size " + ToString(uvs.GetDataLineSize()));

    samplerName = _samplerName;
    if (samplerName.empty())
        samplerName = GetName() + ToString(GetUniqueID()) + "_" + "sample";
}


void TextureSampleNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\tvec4 " + GetSampleOutputName() + " = texture2D(" + GetSamplerUniformName() + ", " + GetUVInput().GetValue() + ");\n";
}