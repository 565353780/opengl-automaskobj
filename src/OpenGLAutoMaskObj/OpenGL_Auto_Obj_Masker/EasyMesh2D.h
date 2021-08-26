#include <iostream>
#include <vector>

class EasyVertex2D
{
public:
    EasyVertex2D()
    {
    }

    bool haveThisVertexNeighboor(
        const int &neighboor_vertex_idx);

    bool haveThisFaceNeighboor(
        const int &neighboor_face_idx);

    bool addVertexNeighboor(
        const int &neighboor_vertex_idx);

    bool addFaceNeighboor(
        const int &neighboor_face_idx);

    float x;
    float y;
    std::vector<int> neighboor_vertex_idx_vec;
    std::vector<int> neighboor_face_idx_vec;
};

class EasyFace2D
{
public:
    EasyFace2D()
    {
    }
    
    bool setVertexIndex(
        const int &vertex_idx_1,
        const int &vertex_idx_2,
        const int &vertex_idx_3);

    bool haveThisVertex(
        const int &vertex_idx);

    bool haveThisFaceNeighboor(
        const int &neighboor_face_idx);

    bool addFaceNeighboor(
        const int &neighboor_face_idx);

    std::vector<int> vertex_idx_vec;
    std::vector<int> neighboor_face_idx_vec;
};

class EasyMesh2D
{
public:
    EasyMesh2D()
    {
    }

    bool addVertex(
        const float &x,
        const float &y);

    bool addFace(
        const int &vertex_idx_1,
        const int &vertex_idx_2,
        const int &vertex_idx_3);

private:
    bool addVertexNeighboor(
        const int &vertex_idx_1,
        const int &vertex_idx_2);

    bool addFaceNeighboor(
        const int &face_idx_1,
        const int &face_idx_2);

public:
    std::vector<EasyVertex2D> vertex_2d_list;
    std::vector<EasyFace2D> face_2d_list;

    int first_vertex_idx=0;
};

