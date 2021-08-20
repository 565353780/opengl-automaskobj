#include "QICPAlign.h"
#include <algorithm>
#include "trimesh2/align_symm.h"
namespace GCL {

QICPAlign::QICPAlign()
{

}

void QICPAlign::align_symm(const std::vector<PtPair> &pairs,
                           const Vec3 &centroid1,
                           const Vec3 &centroid2, QMatrix4x4 &x_matrix)
{
    if(pairs.empty()) return;

    std::vector<float> dist_list;
    for(const PtPair &p : pairs)
    {
       dist_list.push_back((p.p1-p.p2).length());
    }
    float scale = 0.0f;
    for (size_t i = 0; i < pairs.size(); i++) {
        scale += (pairs[i].p1-centroid1).length2();
        scale += (pairs[i].p2-centroid2).length2();
    }
    scale = sqrt(scale / (2 * pairs.size()));
    scale = 1.0f / scale;


    float median_dist = trimesh::median(dist_list);
    xform alignxf;
    trimesh::align_symm(pairs,scale,centroid1,centroid2,median_dist,alignxf);


    orthogonalize(alignxf);
    for(int i=0; i < 4; i++)
    {
        for(int j=0; j < 4; j++)
        {
            x_matrix(i, j) = float(alignxf(i,j));
        }
    }
}
}
