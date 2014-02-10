#include "ShaderInNode.h"

std::string ShaderInNode::GetOutputName(unsigned int outputIndex, Shaders shader) const
{
    assert(outputIndex == 0);

    switch (shader)
    {
    case Shaders::SH_Vertex_Shader: return vertIn;
    case Shaders::SH_Fragment_Shader: return fragIn;
    default: assert(false);
    }
}