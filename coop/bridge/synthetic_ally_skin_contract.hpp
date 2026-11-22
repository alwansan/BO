// BO-COOP synthetic-only CPU pose/skin math contract.
// This is NOT BLACK's verified RCB matrix convention, does not parse the
// proprietary 36-byte vertex fields, and NEVER spawns/controls an NPC or actor.
// Purpose: safety-tested, bounded reference for future human remote AllyBody.
#pragma once
#include <array>
#include <cmath>
#include <cstdint>
#include <span>

namespace blackcoop::synthetic {
constexpr std::size_t kMaxJoints=64;
struct Vec3 {float x=0,y=0,z=0;};
struct Mat4 {
    // ROW-MAJOR matrices, column vectors; synthetic convention only.
    std::array<float,16> a{};
};
inline Mat4 Identity() noexcept {
    Mat4 m;for(int i=0;i<4;++i)m.a[4*i+i]=1.f;return m;
}
inline Mat4 Translate(float x,float y,float z) noexcept {
    Mat4 m=Identity();m.a[3]=x;m.a[7]=y;m.a[11]=z;return m;
}
inline bool Finite(Mat4 const& m) noexcept {
    for(float f:m.a)if(!std::isfinite(f))return false;
    return true;
}
inline Mat4 Multiply(Mat4 const& l,Mat4 const& r) noexcept {
    Mat4 out{};
    for(int row=0;row<4;++row)
        for(int col=0;col<4;++col)
            for(int k=0;k<4;++k)
                out.a[4*row+col]+=l.a[4*row+k]*r.a[4*k+col];
    return out;
}
inline Vec3 Point(Mat4 const& m, Vec3 p) noexcept {
    return {
        m.a[0]*p.x+m.a[1]*p.y+m.a[2]*p.z+m.a[3],
        m.a[4]*p.x+m.a[5]*p.y+m.a[6]*p.z+m.a[7],
        m.a[8]*p.x+m.a[9]*p.y+m.a[10]*p.z+m.a[11]
    };
}
struct WeightedVertex {
    Vec3 position{};
    std::array<uint8_t,4> joint{};
    std::array<float,4> weight{};
};

class SyntheticRig final {
    std::array<int16_t,kMaxJoints> parent_{};
    std::array<Mat4,kMaxJoints> local_{},inverse_bind_{},skin_{};
    std::array<uint8_t,kMaxJoints> visiting_{};
    std::size_t count_=0;

    bool Resolve(std::size_t j) noexcept {
        if(visiting_[j]==2)return true;
        if(visiting_[j]==1)return false; // parent cycle
        visiting_[j]=1;
        const int16_t p=parent_[j];
        Mat4 world=local_[j];
        if(p!=-1){
            if(!Resolve(static_cast<std::size_t>(p)))return false;
            world=Multiply(skin_[static_cast<std::size_t>(p)],world);
            // While calculating tree, skin_ temporarily holds WORLD matrix.
        }
        if(!Finite(world))return false;
        skin_[j]=world;visiting_[j]=2;
        return true;
    }

public:
    // All arguments must originate from an independently verified legitimate
    // skeleton, NOT from untrusted UDP packets or a guessed MODEL +58 array.
    bool Prepare(std::span<const int16_t> parents,
                 std::span<const Mat4> local_pose,
                 std::span<const Mat4> inverse_bind) noexcept {
        count_=0;skin_={};visiting_={};
        const std::size_t n=parents.size();
        if(!n||n>kMaxJoints||local_pose.size()!=n||inverse_bind.size()!=n)
            return false;
        std::size_t roots=0;
        for(std::size_t j=0;j<n;++j) {
            const int p=parents[j];
            if(p < -1 || p >= static_cast<int>(n) || p==static_cast<int>(j) ||
               !Finite(local_pose[j])||!Finite(inverse_bind[j]))return false;
            parent_[j]=static_cast<int16_t>(p);
            local_[j]=local_pose[j];
            inverse_bind_[j]=inverse_bind[j];
            roots+=p==-1;
        }
        if(!roots)return false;
        for(std::size_t j=0;j<n;++j)if(!Resolve(j))return false;
        for(std::size_t j=0;j<n;++j) {
            skin_[j]=Multiply(skin_[j],inverse_bind_[j]);
            if(!Finite(skin_[j])){count_=0;return false;}
        }
        count_=n;
        return true;
    }
    // Normalized convex skin blend, four max; invalid/reused indices refused.
    // No game memory or engine side-effects. Float/weight layout synthetic.
    bool Skin(WeightedVertex const& v,Vec3& out) const noexcept {
        out={};
        if(!count_||!std::isfinite(v.position.x)||!std::isfinite(v.position.y)||
           !std::isfinite(v.position.z))return false;
        float sum=0;
        for(std::size_t i=0;i<4;++i) {
            if(!std::isfinite(v.weight[i])||v.weight[i]<0||v.weight[i]>1)
                return false;
            if(v.weight[i]>0 && v.joint[i]>=count_)return false;
            for(std::size_t j=0;j<i;++j)
                if(v.weight[i]>0 && v.weight[j]>0 && v.joint[i]==v.joint[j])
                    return false;
            sum+=v.weight[i];
        }
        if(!std::isfinite(sum)||std::abs(sum-1.f)>0.005f)return false;
        for(std::size_t i=0;i<4;++i) {
            const float w=v.weight[i];
            if(w==0)continue;
            const Vec3 p=Point(skin_[v.joint[i]],v.position);
            if(!std::isfinite(p.x)||!std::isfinite(p.y)||!std::isfinite(p.z))return false;
            out.x+=w*p.x;out.y+=w*p.y;out.z+=w*p.z;
        }
        return std::isfinite(out.x)&&std::isfinite(out.y)&&std::isfinite(out.z);
    }
    std::size_t JointCount() const noexcept {return count_;}
};
} // namespace blackcoop::synthetic
