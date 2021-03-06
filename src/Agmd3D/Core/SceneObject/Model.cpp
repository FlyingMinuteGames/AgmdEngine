/*
============================================================================
Agmd3D - 3D Engine
Author : Cyril Basset (basset.cyril@gmail.com - https://github.com/Agamand)
https://github.com/Agamand/AgmdEngine
============================================================================
*/

#include <Matrix4.h>
#include <Core/SceneObject/Model.h>
#include <Core/SceneObject/Material.h>
#include <Core/Renderer.h>
#include <Debug/Exception.h>
#include <Debug/New.h>
#include <Core/MatStack.h>
#include <Core/ResourceManager.h>

namespace Agmd
{
    Model::Model(Model::TVertex* Vertices, unsigned long VerticesCount, Model::TIndex* Indices, unsigned long IndicesCount, TPrimitiveType type) :
    m_PrimitiveType(type)
    {
        Assert(Vertices != NULL);
        Assert(Indices  != NULL);

        Generate(G_TANGENT, Vertices, VerticesCount, Indices, IndicesCount);

        TDeclarationElement Elements[] =
        {
            {0, ELT_USAGE_POSITION,        ELT_TYPE_FLOAT3},
            {0, ELT_USAGE_NORMAL,        ELT_TYPE_FLOAT3},
            {0, ELT_USAGE_DIFFUSE,        ELT_TYPE_COLOR},
            {0, ELT_USAGE_TEXCOORD0,    ELT_TYPE_FLOAT2},
            {0, ELT_USAGE_BONE_WEIGHT,  ELT_TYPE_FLOAT4},
            {0, ELT_USAGE_BONE_INDEX,   ELT_TYPE_FLOAT4},
            {0, ELT_USAGE_BONE_COUNT,   ELT_TYPE_FLOAT1}

        };
        m_Declaration = Renderer::Get().CreateVertexDeclaration(Elements);

        m_VertexBuffer = Renderer::Get().CreateVertexBuffer(VerticesCount, 0, Vertices);
        m_IndexBuffer  = Renderer::Get().CreateIndexBuffer(IndicesCount, 0, Indices);
    }


    void Model::Draw(const Transform* transform) const
    {
        Renderer::Get().SetCurrentTransform(transform);
        Renderer::Get().SetDeclaration(m_Declaration);
        Renderer::Get().SetVertexBuffer(0, m_VertexBuffer);
        Renderer::Get().SetIndexBuffer(m_IndexBuffer);
        Renderer::Get().DrawIndexedPrimitives(m_PrimitiveType,0,m_IndexBuffer.GetCount());
    }

    void Model::Generate(GenerateType type, TVertex* vertices, unsigned long verticesCount, TIndex* indices, unsigned long indicesCount)
    {
        std::vector<vec3> normal;
        normal.resize(verticesCount,vec3(0.0f));
        std::vector<vec4> tangent;
        tangent.resize(verticesCount,vec4(0.0f));

        if(!type)
            return;
        int nFaces = indicesCount/3;
        // For all face calc Normal and Tangent
        for(int i = 0; i < nFaces; i++)
        {
            vec3 edge1(vertices[indices[i*3+1]].position-vertices[indices[i*3]].position);
            vec3 edge2(vertices[indices[i*3+2]].position-vertices[indices[i*3]].position);
            vec3 _normal = cross(edge1,edge2);
            _normal = normalize(_normal);

            if(type & G_NORMAL)
            {
                vec3 _normal = cross(edge1,edge2);
                normal[indices[i*3]] += _normal;
                normal[indices[i*3+1]] += _normal;
                normal[indices[i*3+2]] += _normal;
            }
            
            if(type & G_TANGENT)
            {
                vec2 texEdge1(vertices[indices[i*3+1]].texCoords-vertices[indices[i*3]].texCoords);
                vec2 texEdge2(vertices[indices[i*3+2]].texCoords-vertices[indices[i*3]].texCoords);

                vec3 t;
                vec3 b;

                float det = (texEdge1.x * texEdge2.y) - (texEdge1.y * texEdge2.x);

                if (det < 0.001f)
                {
                    t = vec3(1.0f, 0.0f, 0.0f);
                    b = vec3(0.0f, 1.0f, 0.0f);
                }
                else
                {
                    det = 1.0f / det;

                    t.x = (texEdge2.y * edge1.x - texEdge1.y * edge2.x) * det;
                    t.y = (texEdge2.y * edge1.y - texEdge1.y * edge2.y) * det;
                    t.z = (texEdge2.y * edge1.z - texEdge1.y * edge2.z) * det;

                    b.x = (-texEdge2.x * edge1.x + texEdge1.x * edge2.x) * det;
                    b.y = (-texEdge2.x * edge1.y + texEdge1.x * edge2.y) * det;
                    b.z = (-texEdge2.x * edge1.z + texEdge1.x * edge2.z) * det;

                    t = normalize(t);
                    b = normalize(b);
                }

                vec3 bitangent = cross(_normal, t);
                float handedness = (dot(bitangent, b) < 0.0f) ? -1.0f : 1.0f;

                tangent[indices[i*3]] += vec4(t,handedness);
                tangent[indices[i*3+1]] += vec4(t,handedness);
                tangent[indices[i*3+2]] += vec4(t,handedness);
            }
        }

        for(uint32 i = 0; i < normal.size(); i++)
        {
            if(type & G_NORMAL)
            {
                normal[i] = normalize(normal[i]);
                vertices[i].normal = normal[i];
            }
            if(type & G_TANGENT)
            {
                tangent[i] = normalize(tangent[i]);
                vertices[i].tangent = tangent[i];
            }
        }

    }
}