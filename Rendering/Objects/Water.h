#pragma once

#include "../Rendering.hpp"
#include "../../Math/HigherMath.hpp"

//Represents a flowing body of water.
//A bumpmap can be stored in the first texture.
class Water
{
public:

    //Different kinds of water motion.
    enum WaterTypes
    {
        //Moves in a direction.
        Directed,
        //Ripples outward from several spots.
        Rippling,
    };

    //Data that only applies to directional water.
    struct DirectionalWaterArgs
    {
    public:
        //The direction and magnitude of the water flow (in object space).
        //The magnitude is in units per second.
        Vector2f Flow;
        float Amplitude;
        DirectionalWaterArgs(Vector2f flow, float amplitude) : Flow(flow), Amplitude(amplitude) { }
    };

    //Data that only applies to rippling water.
    struct RippleWaterArgs
    {
    public:
        //The position (in world space) of the ripple's source.
        Vector3f Source;
        //The distance from the source at which the amplitude becomes 0.
        float DropoffPoint;
        //The time since this ripple was created.
        float TimeSinceCreated;
        //The height of the waves.
        float Amplitude;
        //The wavelength of the waves.
        float Period;
        //The speed of the waves.
        float Speed;
        RippleWaterArgs(Vector3f source, float dropoffPoint, float height, float period, float speed) : Source(source), Period(period), Speed(speed), DropoffPoint(dropoffPoint), Amplitude(height), TimeSinceCreated(0.0f) { }
    };


    Material * Mat;
    TransformObject Transform;


    //Makes a new Water object that uses circular ripples.
    Water(unsigned int size, unsigned int maxRipples, Vector2f texturePanDirection, Vector3f pos = Vector3f());
    //Makes a new Water object that uses directional water.
    Water(unsigned int size, Vector2f texturePanDirection, DirectionalWaterArgs mainFlow, unsigned int maxRipples);
    ~Water(void);

    bool HasError(void) const { return !errorMsg.empty(); }
    const std::string & GetErrorMessage(void) const { return errorMsg; }
    void ClearErrorMessage(void) { errorMsg.clear(); }


    WaterTypes GetWaterType(void) const { return waterType; }


    //Adds another ripple to the water.
    //This function only applies to rippling water.
    //Returns the id for the created ripple, or -1 if it was unsuccessful.
    int AddRipple(const RippleWaterArgs & args);
    //Changes the water ripples with the given ID.
    //This function only applies to rippling water.
    void ChangeRipple(int element, const RippleWaterArgs & args);

    //Adds a new flow to the water.
    //This function only applies to directional water.
    //Returns the id for the created flow, or -1 if it was unsuccessful.
    int AddFlow(const DirectionalWaterArgs & args);
    //Changes he water flow with the given ID.
    //This function only applies to directional water.
    void ChangeFlow(int element, const DirectionalWaterArgs & args);


    //TODO: Allow ripples to be stopped, and track in the shader how long ago they were stopped. Maybe use negative "TimeSinceCreated" values?

    //Returns whether the uniform was set successfully.
    bool SetBumpmapHeight(float newHeight) { return Mat->SetUniformF("bumpmapHeight", &newHeight, 1); }


private:

    WaterTypes waterType;
    std::string errorMsg;

    //Ripple stuff.
    struct RippleWaterArgsElement { public: RippleWaterArgs Args; int Element; RippleWaterArgsElement(const RippleWaterArgs & a, int e) : Args(a), Element(e) { } };
    int currentRippleIndex;
    int maxRipples;
    int nextRippleID;
    int totalRipples;
    std::vector<RippleWaterArgsElement> ripples;

    //Flow stuff.
    struct DirectionalWaterArgsElement { public: DirectionalWaterArgs Args; int Element; DirectionalWaterArgsElement(const DirectionalWaterArgs & a, int e) : Args(a), Element(e) { } };
    int currentFlowIndex;
    int maxFlows;
    int nextFlowID;
    int totalFlows;
    std::vector<DirectionalWaterArgsElement> flows;

    Mesh waterMesh;

    //Gets the single RenderingPass needed to render rippling water.
    static RenderingPass GetRippleWaterRenderer(int maxRipples);
    //Gets the single RenderingPass needed to render directional water.
    static RenderingPass GetDirectionalWaterRenderer(void);
};