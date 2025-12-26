#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdexcept>

void Mesh::Load(const std::string& file, ID3D11Device* device)
{
    Assimp::Importer imp;
    const aiScene* scene = imp.ReadFile(file, aiProcess_Triangulate);
    if (!scene) throw std::runtime_error(imp.GetErrorString());

    aiMesh* m = scene->mMeshes[0];

    std::vector<Vertex> verts(m->mNumVertices);
    for (UINT i = 0; i < m->mNumVertices; ++i)
        verts[i] = { m->mVertices[i].x, m->mVertices[i].y, m->mVertices[i].z };

    std::vector<unsigned short> inds;
    for (UINT i = 0; i < m->mNumFaces; ++i)
        for (UINT j = 0; j < 3; ++j)
            inds.push_back((unsigned short)m->mFaces[i].mIndices[j]);

    indexCount = (UINT)inds.size();

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = sizeof(Vertex) * (UINT)verts.size();
    D3D11_SUBRESOURCE_DATA sd = { verts.data() };
    device->CreateBuffer(&bd, &sd, &vb);

    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.ByteWidth = sizeof(unsigned short) * indexCount;
    sd.pSysMem = inds.data();
    device->CreateBuffer(&bd, &sd, &ib);
}

void Mesh::Draw(ID3D11DeviceContext* ctx) const
{
    UINT stride = sizeof(Vertex), offset = 0;
    ctx->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
    ctx->IASetIndexBuffer(ib, DXGI_FORMAT_R16_UINT, 0);
    ctx->DrawIndexed(indexCount, 0, 0);
}
