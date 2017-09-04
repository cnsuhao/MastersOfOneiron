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


#include "mastercontrol.h"
#include "resourcemaster.h"
#include "spawnmaster.h"
#include "volcano.h"
#include "storm.h"
#include "world.h"

const Vector3 World::v0(M_PHI,	0.0f, M_PHI_P2);
const Vector3 World::v2(World::MirrorX(v0));
const Vector3 World::v5(0.0f, M_PHI_P2, M_PHI);
const Vector3 World::v8(World::MirrorY(v5));
const Vector3 World::v12(M_PHI_P2, M_PHI, 0.0f);
const Vector3 World::v15(World::MirrorX(v12));
const Vector3 World::v16(World::MirrorXY(v12));
const Vector3 World::v19(World::MirrorY(v12));
const Vector3 World::v23(World::MirrorZ(v5));
const Vector3 World::v26(World::MirrorYZ(v5));
const Vector3 World::v28(World::MirrorZ(v0));
const Vector3 World::v30(World::MirrorXZ(v0));

const Vector3 World::v1(0.0f, 1.0f, M_PHI_P2);
const Vector3 World::v3(World::MirrorY(v1));
const Vector3 World::v10(M_PHI_P2, 0.0f, 1.0f);
const Vector3 World::v11(World::MirrorX(v10));
const Vector3 World::v13(1.0f, M_PHI_P2, 0.0f);
const Vector3 World::v14(World::MirrorX(v13));
const Vector3 World::v17(World::MirrorXY(v13));
const Vector3 World::v18(World::MirrorY(v13));
const Vector3 World::v20(World::MirrorZ(v10));
const Vector3 World::v21(World::MirrorXZ(v10));
const Vector3 World::v29(World::MirrorZ(v1));
const Vector3 World::v31(World::MirrorYZ(v1));

const Vector3 World::v4(M_PHI, M_PHI, M_PHI);
const Vector3 World::v6(World::MirrorX(v4));
const Vector3 World::v7(World::MirrorXY(v4));
const Vector3 World::v9(World::MirrorY(v4));
const Vector3 World::v22(World::MirrorZ(v4));
const Vector3 World::v24(World::MirrorXZ(v4));
const Vector3 World::v25(-v4);
const Vector3 World::v27(World::MirrorYZ(v4));

void World::RegisterObject(Context* context)
{
    context->RegisterFactory<World>();
}

World::World(Context* context) : LogicComponent(context),
    rhombicCenters{},
    radius_{235.0f}
{
}

void World::OnNodeSet(Node* node)
{ if (!node) return;

    //Create water surface
    Node* bubbleNode{ node_->CreateChild("Bubble") };
    bubbleNode->AddTag("Bubble");
    StaticModel* object{ bubbleNode->CreateComponent<StaticModel>() };
    object->SetModel(CreateRhombicTriacontahedron(radius_, 0.05f));
    object->SetMaterial(RESOURCE->GetMaterial("Bubble"));

    //Create volcanoes
    for (Vector3 v : GetIcosahedricPoints())
        SPAWN->Create<Volcano>()->Set(v * M_1_PHI_P2 * radius_);
    //Create storms
    for (Vector3 s : GetDodecahedricPoints())
        SPAWN->Create<Storm>()->Set(s * M_1_PHI_P2 * radius_);

}

void World::Update(float timeStep)
{
}

SharedPtr<Model> World::CreateRhombicTriacontahedron(float radius, float thickness)
{
    Vector<Vector3> uniquePoints{ GetUniquePoints() };

    Vector<Vector3> vertices{};
    Vector<unsigned> indices{};

    Vector<bool> sides{ true };
    if (thickness > 0.0f)
        sides.Push(false);

    for (bool outside : sides) {

        //Six sides to a cube
        for (int c{0}; c < 6; ++c) {
            //Five rhombi per side
            for (int r{0}; r < 5; ++r){
                //Four vertices per rhombus
                for (int v {0}; v < 4; ++v) {

                    Vector3 position{ uniquePoints.At(v % 3) };

                    if (v == 3 && r != 4) {
                        position = Vector3(0.0f, position.z_, position.x_);
                    }

                    switch(r) {
                    case 0: default:
                        break;
                    case 1:
                        position = MirrorX(position);
                        break;
                    case 2:
                        position = MirrorXY(position);
                        break;
                    case 3:
                        position = MirrorY(position);
                        break;
                    case 4:
                        if ( v > 1) {
                            position = uniquePoints.At(3 - v);
                            position = MirrorXY(position);
                        }
                    }

                    switch(c) {
                    case 0: case 3: default:
                        break;
                    case 1: case 4:
                        position = Quaternion(90.0f, Vector3::RIGHT) * Quaternion(90.0f, Vector3::FORWARD) * position;
                        break;
                    case 2: case 5:
                        position = Quaternion(90.0f, Vector3::FORWARD) * Quaternion(90.0f, Vector3::RIGHT) * position;
                        break;
                    }
                    if (c > 2) {
                        position = -position;
                    }

                    float offset{ outside ?  thickness * 0.5f
                                          : -thickness * 0.5f };

                    float scalar{ M_1_PHI_P2 * (radius + offset) };

                    vertices.Push(position * scalar);
                }

                //Add indices
                bool reverse{ r == 0
                           || r == 2 };

                if ((c > 2) ^ !outside)
                    reverse = !reverse;

                if (reverse) {
                    for (int i{0}; i < 2; ++i)
                        for (int j{0}; j < 3; ++j)
                            indices.Push(i == 0 ? vertices.Size() - 2 - j
                                                : vertices.Size() - 3 + j);
                } else {
                    for (int i{0}; i < 2; ++i)
                        for (int j{0}; j < 3; ++j)
                            indices.Push(i == 0 ? vertices.Size() - 4 + j
                                                : vertices.Size() - 1 - j);
                }
            }
        }
    }
    const unsigned numVertices{ vertices.Size() };

    //Fill vertex data with positions
    float vertexData[numVertices * 6];
    int vertexDataIndex{ 0 };

    for (Vector3 v : vertices) {
        for (int i{0}; i < 6; ++i) {
            if (i < 3) {
                vertexData[vertexDataIndex] = v.Data()[i];
            } else {
                vertexData[vertexDataIndex] = 0.0f;
            }
            ++vertexDataIndex;
        }
    }

    //Fill index data
    unsigned indexCount{ indices.Size() };
    unsigned short indexData[indexCount];

    for (unsigned i{0}; i < indices.Size(); ++i) {
        indexData[i] = indices.At(i);
    }

    // Calculate face normals
    for (unsigned i{0}; i < numVertices; i += 4)
    {
        Vector3& v1{ *(reinterpret_cast<Vector3*>(&vertexData[6 * i])) };
        Vector3& v2{ *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 1)])) };
        Vector3& v3{ *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 2)])) };
        Vector3& v4{ *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 3)])) };

        Vector3& n1{ *(reinterpret_cast<Vector3*>(&vertexData[6 * i + 3])) };
        Vector3& n2{ *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 1) + 3])) };
        Vector3& n3{ *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 2) + 3])) };
        Vector3& n4{ *(reinterpret_cast<Vector3*>(&vertexData[6 * (i + 3) + 3])) };

        Vector3 edge1{ v1 - v2 };
        Vector3 edge2{ v1 - v3 };
        Vector3 normal{ edge1.CrossProduct(edge2).Normalized() };

        Vector3 averagePosition{ 0.25f * (v1 + v2 + v3 + v4) };
        if (averagePosition.ProjectOntoAxis(normal) < 0.0f
          ^ averagePosition.Length() - radius < 0.0f)
        {
            normal = -normal;
        }

        n1 = n2 = n3 = n4 = normal;

        if (rhombicCenters.Size() < 30)
            rhombicCenters.Push(normal * radius);
    }

    SharedPtr<Model> fromScratchModel(new Model(context_));
    SharedPtr<VertexBuffer> vb(new VertexBuffer(context_));
    SharedPtr<IndexBuffer> ib(new IndexBuffer(context_));
    SharedPtr<Geometry> geom(new Geometry(context_));

    // Shadowed buffer needed for raycasts to work, and so that data can be automatically restored on device loss
    vb->SetShadowed(true);
    // We could use the "legacy" element bitmask to define elements for more compact code, but let's demonstrate
    // defining the vertex elements explicitly to allow any element types and order
    PODVector<VertexElement> elements;
    elements.Push(VertexElement(TYPE_VECTOR3, SEM_POSITION));
    elements.Push(VertexElement(TYPE_VECTOR3, SEM_NORMAL));
    vb->SetSize(numVertices, elements);
    vb->SetData(vertexData);

    ib->SetShadowed(true);
    ib->SetSize(indexCount, false);
    ib->SetData(indexData);

    geom->SetVertexBuffer(0, vb);
    geom->SetIndexBuffer(ib);
    geom->SetDrawRange(TRIANGLE_LIST, 0, indexCount, 0, numVertices);

    fromScratchModel->SetNumGeometries(1);
    fromScratchModel->SetGeometry(0, 0, geom);
    fromScratchModel->SetBoundingBox(BoundingBox(Vector3(-radius, -radius, -radius), Vector3(radius, radius, radius)));

    // Though not necessary to render, the vertex & index buffers must be listed in the model so that it can be saved properly
    Vector<SharedPtr<VertexBuffer> > vertexBuffers;
    Vector<SharedPtr<IndexBuffer> > indexBuffers;
    vertexBuffers.Push(vb);
    indexBuffers.Push(ib);
    // Morph ranges could also be not defined. Here we simply define a zero range (no morphing) for the vertex buffer
    PODVector<unsigned> morphRangeStarts;
    PODVector<unsigned> morphRangeCounts;
    morphRangeStarts.Push(0);
    morphRangeCounts.Push(0);
    fromScratchModel->SetVertexBuffers(vertexBuffers, morphRangeStarts, morphRangeCounts);
    fromScratchModel->SetIndexBuffers(indexBuffers);

    return fromScratchModel;
}

Vector3 World::GetNearestRhombicCenter(Vector3 position)
{
    Vector3 rhombicCenter{};
    for (Vector3 center : GetRhombicCenters()) {
        if (rhombicCenter == Vector3::ZERO
                || LucKey::Distance(position, center)
                < LucKey::Distance(position, rhombicCenter))
        {
            rhombicCenter = center;
        }
    }

    return rhombicCenter;
}
