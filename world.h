/* Masters of Oneiron
// Copyright (C) 2017 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/



#ifndef WORLD_H
#define WORLD_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

class World : public LogicComponent
{
    URHO3D_OBJECT(World, LogicComponent);
public:
    World(Context* context);
    static void RegisterObject(Context* context);
    virtual void OnNodeSet(Node* node);
    virtual void Update(float timeStep);

    Vector<Vector3> GetUniquePoints() { return { v0, v1, v4 }; }

    Vector<Vector3> GetRhombicTriacontahedricPoints() { return { v0,  v1,  v2,  v3,  v4,  v5,  v6,  v7,
                                                                 v8,  v9, v10, v11, v12, v13, v14, v15,
                                                                v16, v17, v18, v19, v20, v21, v22, v23,
                                                                v24, v25, v26, v27, v28, v29, v30, v31 }; }

    Vector<Vector3> GetIcosahedricPoints() { return {  v0,  v2,  v5,  v8,
                                                      v12, v15, v16, v19,
                                                      v23, v26, v28, v30 }; }

    Vector<Vector3> GetDodecahedricPoints() { return {  v1,  v3,  v4,  v6,  v7,
                                                        v9, v10, v11, v13, v14,
                                                       v17, v18, v20, v21, v22,
                                                       v24, v25, v27, v29, v31 }; }

    Vector<Vector3> GetCubicPoints() { return {  v4,  v6,  v7,  v9,
                                                v22, v24, v25, v27 }; }

    SharedPtr<Model> CreateRhombicTriacontahedron(float radius = 1.0f, float thickness = 0.0f);

    const Vector<Vector3>& GetRhombicCenters() const { return rhombicCenters_; }
    Vector3 GetNearestRhombicCenter(Vector3 position);
    Vector3 ToSurface(const Vector3& position);
private:
    static constexpr float M_PHI    = 1.61803398874989484820458683436564f;
    static constexpr float M_PHI_P2 = 2.61803398874989484820458683436564f;
    static constexpr float M_1_PHI_P2 = 0.38196601125010515179541316563436f;

//    static constexpr float M_PHI_P3 = 6.85410196624968454461376050309691f;

//Rhombic Triacontahedron { 2, 4, 6, 8, 11, 12, 13, 16, 17, 18, 20, 23, 27, 28, 30, 31, 33, 34, 36, 37, 38, 41, 45, 46, 47, 50, 51, 52, 54, 56, 58, 60}
//Mapping                 { 0, 1, 2, 3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31}

    static const Vector3 v0,  v1,  v2,  v3,  v4,  v5,  v6,  v7,  v8,
                         v9, v10, v11, v12, v13, v14, v15, v16, v17,
                        v18, v19, v20, v21, v22, v23, v24, v25, v26,
                        v27, v28, v29, v30, v31;


    static Vector3 MirrorX(const Vector3& vec)  { return Vector3(-vec.x_, vec.y_, vec.z_); }
    static Vector3 MirrorY(const Vector3& vec)  { return Vector3( vec.x_,-vec.y_, vec.z_); }
    static Vector3 MirrorZ(const Vector3& vec)  { return Vector3( vec.x_, vec.y_,-vec.z_); }
    static Vector3 MirrorXY(const Vector3& vec) { return Vector3(-vec.x_,-vec.y_, vec.z_); }
    static Vector3 MirrorXZ(const Vector3& vec) { return Vector3(-vec.x_, vec.y_,-vec.z_); }
    static Vector3 MirrorYZ(const Vector3& vec) { return Vector3( vec.x_,-vec.y_,-vec.z_); }

    Vector<Vector3> rhombicCenters_;

    float radius_;
  /*
{( 2, 4, 6), ( 6, 8, 2)}, {( 2, 11, 4), ( 4, 11, 12)},
{( 4, 12, 13), ( 4, 13, 6)}, {( 6, 16, 8), ( 8, 16, 17)},
{( 8, 17, 18), ( 8, 18, 2)}, {( 2, 18, 37), ( 2, 37, 20)},
{( 2, 20, 27), ( 2, 27, 11)}, {(11, 27, 28), (11, 28, 12)},
{( 6, 13, 31), ( 6, 31, 23)}, {( 6, 23, 33), ( 6, 33, 16)},
{(54, 60, 58), (58, 56, 54)}, {(54, 56, 45), (45, 56, 46)},
{(56, 58, 47), (47, 46, 56)}, {(47, 58, 41), (41, 31, 47)},
{(58, 50, 33), (33, 41, 58)}, {(58, 60, 51), (51, 50, 58)},
{(60, 54, 52), (52, 51, 60)}, {(54, 38, 37), (37, 52, 54)},
{(45, 27, 38), (38, 54, 45)}, {(20, 37, 38), (38, 27, 20)},
{(23, 31, 41), (41, 33, 23)}, {(12, 28, 46), (46, 30, 12)},
{(12, 30, 31), (31, 13, 12)}, {(31, 30, 46), (46, 47, 31)},
{(28, 27, 45), (45, 46, 28)}, {(17, 34, 51), (51, 36, 17)},
{(18, 17, 36), (36, 37, 18)}, {(37, 36, 51), (51, 52, 37)},
{(17, 16, 33), (33, 34, 17)}, {(34, 33, 50), (50, 51, 34)}*/
};

#endif // WORLD_H



