#include "particle_link.h"

float ParticleLink::currentLength() const {
    using namespace DirectX;

    XMVECTOR relativePos = particle[0]->getPosition() - particle[1]->getPosition();
    return XMVectorGetX(XMVector3Length(relativePos));
}
