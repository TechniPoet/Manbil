#pragma once


//Different kinds of data about what a material uses.
//TODO: Create "world render sampler" built-in uniform so that objects can sample and display/distort the world behind them (along with usage flags -- no DataNode is needed though, as a SamplerNode will work fine).
struct MaterialUsageFlags
{
public:

    enum Flags : unsigned int
    {
        //Uses elapsed time uniform.
        DNF_USES_TIME = 0x00001,

        //Uses world matrix uniform.
        DNF_USES_WORLD_MAT = 0x00002,
        //Uses view matrix uniform.
        DNF_USES_VIEW_MAT = 0x00004,
        //Uses projection matrix uniform.
        DNF_USES_PROJ_MAT = 0x00008,
        //Uses view-projection matrix uniform.
        DNF_USES_VIEWPROJ_MAT = 0x00010,
        //Uses WVP matrix uniform.
        DNF_USES_WVP_MAT = 0x00020,

        //Uses camera position uniform.
        DNF_USES_CAM_POS = 0x00040,
        //Uses camera forward vector uniform.
        DNF_USES_CAM_FORWARD = 0x00080,
        //Uses camera upwards vector uniform.
        DNF_USES_CAM_UPWARDS = 0x00100,
        //Uses camera sideways vector uniform.
        DNF_USES_CAM_SIDEWAYS = 0x00200,

        //Uses screen width uniform.
        DNF_USES_WIDTH = 0x00400,
        //Uses screen height uniform.
        DNF_USES_HEIGHT = 0x00800,
        //Uses screen z-near uniform.
        DNF_USES_ZNEAR = 0x01000,
        //Uses screen z-far uniform.
        DNF_USES_ZFAR = 0x02000,
        //Uses FOV uniform.
        DNF_USES_FOV = 0x04000,

        //Uses orthographic min uniform.
        DNF_USES_ORTHO_MIN = 0x08000,
        //Uses orthographic max uniform.
        DNF_USES_ORTHO_MAX = 0x10000,
    };

    bool GetFlag(Flags flag) const { return (value & (unsigned int)flag) > 0; }

    void EnableFlag(Flags flag)
    {
        value |= (unsigned int)flag;
        value = value;
    }
    void DisableFlag(Flags flag) { value &= ~((unsigned int)flag); }

    void EnableAllFlags(void) { value = 0xffffffff; }
    void ClearAllFlags(void) { value = 0; }


private:

    //TODO: Track the outputs taken from every input data node (an unordered_map indexed by pointer?), then use that information to remove any unnecessary lines of shader code in the data node shader code generation functions.

    unsigned short value = 0;
};