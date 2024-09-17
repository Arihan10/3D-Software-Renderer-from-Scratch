// Shape3D.cpp
#include "Shape3D.h"

Shape3D::Shape3D() {
    color = sf::Color::Yellow;
    name = "Obj";
}

void Shape3D::updateTrans() {
    verts.resize(vertsOG.size());
    for (size_t i = 0; i < vertsOG.size(); ++i) {
        verts[i] = vertsOG[i] * (scale * 50.0) + pos;
    }
}

const std::vector<Vector3>& Shape3D::getVerts() const {
    return verts;
}

const std::vector<int>& Shape3D::getTris() const {
    return tris;
}

void Shape3D::setColor(const sf::Color& color) {
    this->color = color;
}

const sf::Color& Shape3D::getColor() const {
    return color;
}
