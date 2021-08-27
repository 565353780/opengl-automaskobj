#include "EasyMesh2D.h"

bool EasyVertex2D::haveThisVertexNeighboor(
    const int &neighboor_vertex_idx)
{
    if(neighboor_vertex_idx_vec.size() == 0)
    {
        return false;
    }

    for(const int &exist_neighboor_vertex_idx : neighboor_vertex_idx_vec)
    {
        if(exist_neighboor_vertex_idx == neighboor_vertex_idx)
        {
            return true;
        }
    }

    return false;
}

bool EasyVertex2D::haveThisFaceNeighboor(
    const int &neighboor_face_idx)
{
    if(neighboor_face_idx_vec.size() == 0)
    {
        return false;
    }

    for(const int &exist_neighboor_face_idx : neighboor_face_idx_vec)
    {
        if(exist_neighboor_face_idx == neighboor_face_idx)
        {
            return true;
        }
    }

    return false;
}

bool EasyVertex2D::addVertexNeighboor(
    const int &neighboor_vertex_idx)
{
    if(haveThisVertexNeighboor(neighboor_vertex_idx))
    {
        return true;
    }
    neighboor_vertex_idx_vec.emplace_back(neighboor_vertex_idx);
    return true;
}

bool EasyVertex2D::addFaceNeighboor(
    const int &neighboor_face_idx)
{
    if(haveThisFaceNeighboor(neighboor_face_idx))
    {
        return true;
    }
    neighboor_face_idx_vec.emplace_back(neighboor_face_idx);
    return true;
}

bool EasyFace2D::setVertexIndex(
    const int &vertex_idx_1,
    const int &vertex_idx_2,
    const int &vertex_idx_3)
{
    vertex_idx_vec.resize(3);
    vertex_idx_vec[0] = vertex_idx_1;
    vertex_idx_vec[1] = vertex_idx_2;
    vertex_idx_vec[2] = vertex_idx_3;
    return true;
}

bool EasyFace2D::haveThisVertex(
    const int &vertex_idx)
{
    for(const int &face_vertex_idx : vertex_idx_vec)
    {
        if(face_vertex_idx == vertex_idx)
        {
            return true;
        }
    }

    return false;
}

bool EasyFace2D::haveThisFaceNeighboor(
    const int &neighboor_face_idx)
{
    if(neighboor_face_idx_vec.size() == 0)
    {
        return false;
    }

    for(const int &exist_neighboor_face_idx : neighboor_face_idx_vec)
    {
        if(exist_neighboor_face_idx == neighboor_face_idx)
        {
            return true;
        }
    }

    return false;
}

bool EasyFace2D::addFaceNeighboor(
    const int &neighboor_face_idx)
{
    if(haveThisFaceNeighboor(neighboor_face_idx))
    {
        return true;
    }
    neighboor_face_idx_vec.emplace_back(neighboor_face_idx);
    return true;
}



bool EasyMesh2D::addVertex(
    const float &x,
    const float &y)
{
    EasyVertex2D vertex_2d;
    vertex_2d.x = x;
    vertex_2d.y = y;

    vertex_2d_list.emplace_back(vertex_2d);

    return true;
}

bool EasyMesh2D::addFace(
    const int &vertex_idx_1,
    const int &vertex_idx_2,
    const int &vertex_idx_3)
{
    addVertexNeighboor(vertex_idx_1, vertex_idx_2);
    addVertexNeighboor(vertex_idx_2, vertex_idx_3);
    addVertexNeighboor(vertex_idx_3, vertex_idx_1);

    EasyFace2D face_2d;
    face_2d.setVertexIndex(
        vertex_idx_1 - first_vertex_idx,
        vertex_idx_2 - first_vertex_idx,
        vertex_idx_3 - first_vertex_idx);
    face_2d_list.emplace_back(face_2d);

    for(const int &face_vertex_idx : face_2d.vertex_idx_vec)
    {
        std::vector<int> current_neighboor_face_idx_vec;
        for(const int &current_vertex_neighboor_face_idx :
            vertex_2d_list[face_vertex_idx].neighboor_face_idx_vec)
        {
            bool is_neighboor_of_face_vertex_1 = face_2d_list[current_vertex_neighboor_face_idx].
              haveThisVertex(face_2d.vertex_idx_vec[0]);
            bool is_neighboor_of_face_vertex_2 = face_2d_list[current_vertex_neighboor_face_idx].
              haveThisVertex(face_2d.vertex_idx_vec[1]);
            bool is_neighboor_of_face_vertex_3 = face_2d_list[current_vertex_neighboor_face_idx].
              haveThisVertex(face_2d.vertex_idx_vec[2]);

            if(is_neighboor_of_face_vertex_1)
            {
                if(is_neighboor_of_face_vertex_2 || is_neighboor_of_face_vertex_3)
                {
                    current_neighboor_face_idx_vec.emplace_back(current_vertex_neighboor_face_idx);
                }
            }
            else if(is_neighboor_of_face_vertex_2 && is_neighboor_of_face_vertex_3)
            {
                current_neighboor_face_idx_vec.emplace_back(current_vertex_neighboor_face_idx);
            }
        }

        vertex_2d_list[face_vertex_idx].addFaceNeighboor(face_2d_list.size() - 1);

        for(const int &current_neighboor_face_idx : current_neighboor_face_idx_vec)
        {
            face_2d_list.back().addFaceNeighboor(current_neighboor_face_idx);
            face_2d_list[current_neighboor_face_idx].addFaceNeighboor(face_2d_list.size() - 1);
        }
    }

    return true;
}

bool EasyMesh2D::addVertexNeighboor(
    const size_t &vertex_idx_1,
    const size_t &vertex_idx_2)
{
    if(vertex_idx_1 < first_vertex_idx || vertex_idx_1 >= vertex_2d_list.size() + first_vertex_idx)
    {
        std::cout << "Add Neighboor failed! Vertex 1 idx out of range" << std::endl;
        std::cout << "valid value must in [" <<
          0 << "," << vertex_2d_list.size() - first_vertex_idx <<
          "]" << std::endl;
        std::cout << "current value is " << vertex_idx_1 << std::endl;
        return false;
    }
    if(vertex_idx_2 < first_vertex_idx || vertex_idx_2 >= vertex_2d_list.size() + first_vertex_idx)
    {
        std::cout << "Add Neighboor failed! Vertex 2 idx out of range" << std::endl;
        std::cout << "valid value must in [" <<
          0 << "," << vertex_2d_list.size() - first_vertex_idx <<
          "]" << std::endl;
        std::cout << "current value is " << vertex_idx_2 << std::endl;
        return false;
    }

    vertex_2d_list[vertex_idx_1 - first_vertex_idx].addVertexNeighboor(vertex_idx_2 - first_vertex_idx);
    vertex_2d_list[vertex_idx_2 - first_vertex_idx].addVertexNeighboor(vertex_idx_1 - first_vertex_idx);

    return true;
}

bool EasyMesh2D::addFaceNeighboor(
    const size_t &face_idx_1,
    const size_t &face_idx_2)
{
    if(face_idx_1 >= face_2d_list.size())
    {
        std::cout << "Add Neighboor failed! Face 1 idx out of range" << std::endl;
        std::cout << "valid value must in [" <<
          0 << "," << face_2d_list.size() - first_vertex_idx <<
          "]" << std::endl;
        std::cout << "current value is " << face_idx_1 << std::endl;
        return false;
    }
    if(face_idx_2 >= face_2d_list.size())
    {
        std::cout << "Add Neighboor failed! Face 2 idx out of range" << std::endl;
        std::cout << "valid value must in [" <<
          0 << "," << face_2d_list.size() - first_vertex_idx <<
          "]" << std::endl;
        std::cout << "current value is " << face_idx_2 << std::endl;
        return false;
    }

    face_2d_list[face_idx_1].addFaceNeighboor(face_idx_2);
    face_2d_list[face_idx_2].addFaceNeighboor(face_idx_1);

    return true;
}

