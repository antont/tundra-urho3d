// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

namespace Urho3D
{
    class Node;
    class Scene;
    class Camera;
    class AnimatedModel;
    class Light;
    class AnimationState;
    class Model;
    class Material;
    class Texture2D;
    class Zone;
}

namespace Tundra
{
    class UrhoRenderer;
    class GraphicsWorld;
    class Placeable;
    class Mesh;
    class Camera;
    class IOgreMaterialProcessor;

    typedef SharedPtr<GraphicsWorld> GraphicsWorldPtr;
    typedef WeakPtr<GraphicsWorld> GraphicsWorldWeakPtr;

    namespace Ogre
    {
        class MaterialParser;
    }
}
