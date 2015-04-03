#pragma once
#include <Urho3D/Core/Object.h>
#include <Urho3D/Urho3D.h>

using namespace Urho3D;

class PoolObject: public Object
{
    //Enable type information
    OBJECT(PoolObject);
public:
    PoolObject(Context* context);
protected:
private:
};

