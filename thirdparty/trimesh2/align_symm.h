#include "Math/MathDefines.h"
#include <vector>
#include "XForm.h"
#include "lineqn.h"
#ifdef Q3D_GCL_LIBRARY
#include "Core/QICPAlign.h"
typedef GCL::QICPAlign::PtPair PtPair;
#elif
struct PtPair {
    Vec3 p1, n1, p2, n2;
    PtPair(const Vec3 &p1_, const Vec3 &n1_,
           const Vec3 &p2_, const Vec3 &n2_) :
            p1(p1_), n1(n1_), p2(p2_), n2(n2_)
        {}
};
#endif
namespace GCL {
using namespace std;

#define MAX_ITERS 150
#define TERMINATION_ITER_THRESH 25
#define FINAL_ITERS 2
#define MIN_PAIRS 10
#define DESIRED_PAIRS 200
#define DESIRED_PAIRS_FINAL 5000
#define CDF_UPDATE_INTERVAL 20
#define APPROX_EPS 0.05f
#define REJECT_BDY false
#define USE_NORMCOMPAT true
#define HUBER_THRESH_MULT 2.0f
#define REGULARIZATION 0.0002f
#define DIST_THRESH_MULT 12.0f
#define DIST_THRESH_MULT_FINAL 4.0f
#define ANGLE_THRESH_MULT 1.5f
#define ANGLE_THRESH_MIN 0.1f
#define ANGLE_THRESH_MAX 1.0f

// A pair of points, with associated normals
typedef  Vec<3,Scalar> point;

namespace trimesh {


// Do symmetric point-to-plane alignment, returning alignxf
// as well as eigenVec3tors and inverse eigenvalues
static void align_symm(const vector<PtPair> &pairs, float scale,
                       const point &centroid1, const point &centroid2,
                       float median_dist, xform &alignxf)
{
    float huber_thresh = HUBER_THRESH_MULT * scale * median_dist;
	size_t npairs = pairs.size();
	float A[6][6] = { { 0 } }, b[6] = { 0 };
	for (size_t i = 0; i < npairs; i++) {
        Vec3 p1 = scale * (pairs[i].p1 - centroid1);
        Vec3 p2 = scale * (pairs[i].p2 - centroid2);
        Vec3 n = pairs[i].n1 + pairs[i].n2;
        Vec3 p = p1 + p2;
        Vec3 c = p ^ n;
        Vec3 d = p1 - p2;

		float x[6] = { c[0], c[1], c[2], n[0], n[1], n[2] };
        float dn = d * n;

		// Huber weights, used for IRLS
		float wt = huber_thresh / max(fabs(dn), huber_thresh);

		for (int j = 0; j < 6; j++) {
			b[j] += wt * dn * x[j];
			for (int k = j; k < 6; k++)
				A[j][k] += wt * x[j] * x[k];
		}
	}

	// Make matrix symmetric
	for (int j = 1; j < 6; j++)
		for (int k = 0; k < j; k++)
			A[j][k] = A[k][j];

	// Eigen-decomposition and inverse
	float eval[6], einv[6];
	eigdc<float,6>(A, eval);
	for (int i = 0; i < 6; i++)
		einv[i] = 1.0f / (eval[i] + REGULARIZATION * eval[5]);

	// Solve system
	eigmult<float,6>(A, einv, b);

	// Extract rotation and translation
    Vec3 rot(b[0], b[1], b[2]), trans(b[3], b[4], b[5]);
    float rotangle = atan((rot).length());
	trans *= cos(rotangle);
	trans *= 1.0f / scale;
	xform R = xform::rot(rotangle, rot);
        alignxf = xform::trans(centroid1) *
                  R * xform::trans(trans) * R *
                  xform::trans(-centroid2);
}


// Do symmetric point-to-plane translation-only alignment
static void align_pt2pl_trans(const vector<PtPair> &pairs,
                              const point &centroid1, const point &centroid2,
                              xform &alignxf)
{
	size_t npairs = pairs.size();

    float eVec3[3][3] = { { 0 } }, einv[3] = { 0 };
    float b[3];
	for (size_t i = 0; i < npairs; i++) {
        Vec3 p1 = pairs[i].p1 - centroid1;
        Vec3 p2 = pairs[i].p2 - centroid2;
        Vec3 n = 0.5f * (pairs[i].n1 + pairs[i].n2);
        float d = (p1 - p2) * n;

		for (int j = 0; j < 3; j++) {
			b[j] += d * n[j];
			for (int k = 0; k < 3; k++)
                eVec3[j][k] += n[j] * n[k];
		}
	}

	// Eigen-decomposition and inverse
    float eval[3];
    eigdc<float,3>(eVec3, eval);
	for (int i = 0; i < 3; i++)
		einv[i] = 1.0f / (eval[i] + REGULARIZATION * eval[2]);

	// Solve system
    eigmult<float,3>(eVec3, einv, b);
    Vec3 bb(b[0],b[1],b[2]);
    bb += centroid1 - centroid2;
    alignxf = xform::trans(bb);
}
static float median(vector<float> vals)
{
        size_t n = vals.size();
        if (!n)
                return 0.0f;

        size_t mid = n / 2;
        nth_element(vals.begin(), vals.begin() + mid, vals.end());
        return vals[mid];
}

}
}
