// main.cpp
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "Vector3.h"
#include "Shape3D.h"
#include "Cube.h"
#include "CustomShape.h"

#include <vector>
#include <queue>
#include <cmath>
#include <iostream>
#include <string>
#include <functional>
#include <algorithm>

// Comparator for the priority queue to sort triangles back-to-front
struct CompareTriangles {
    bool operator()(const std::pair<double, std::pair<std::vector<double>, sf::Color>>& a,
        const std::pair<double, std::pair<std::vector<double>, sf::Color>>& b) {
        return a.first < b.first; // Reverse order
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "3D Renderer");
    window.setFramerateLimit(60);

    // Camera and lighting
    Vector3 camPos(0.0, 0.0, 0.0);
    Vector3 lightDir(0.0, 1.0, -1.0);

    // Timing
    sf::Clock clock;
    double deltaTime = 0.0;

    // Camera rotation angles
    double angleY = 0.0;
    double angleX = 0.0;
    double FoV = 28.0;
    double zFar = 80.0;
    double zNear = 2.0;
    double moveSpeed = 200.0;
    double mouseSensitivityX = 0.4;
    double mouseSensitivityY = 0.3;
    double deltaX = 0.0;
    double deltaY = 0.0;

    // Directional keys
    bool dirs[10] = { false };

    // Mouse control
    bool movingCamera = false;

    // Objects in the scene
    std::vector<Shape3D*> objects;

    // Add initial objects
    // objects.push_back(new Cube(Vector3(0, 0, -10), Vector3(3, 3, 3)));
    objects.push_back(new CustomShape("Suzanne.obj", Vector3(0, 0, -10), Vector3(3, 3, 3)));

    // Input handling
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close window
            if (event.type == sf::Event::Closed)
                window.close();

            // Key pressed
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::W) dirs[2] = true;
                if (event.key.code == sf::Keyboard::S) dirs[3] = true;
                if (event.key.code == sf::Keyboard::A) dirs[4] = true;
                if (event.key.code == sf::Keyboard::D) dirs[5] = true;
                if (event.key.code == sf::Keyboard::E) dirs[0] = true;
                if (event.key.code == sf::Keyboard::Q) dirs[1] = true;
                if (event.key.code == sf::Keyboard::Escape) window.close();
                if (event.key.code == sf::Keyboard::LShift) {
                    if (!movingCamera) {
                        sf::Mouse::setPosition(sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2), window);
                    }
                    movingCamera = true;
                }
            }

            // Key released
            if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::W) dirs[2] = false;
                if (event.key.code == sf::Keyboard::S) dirs[3] = false;
                if (event.key.code == sf::Keyboard::A) dirs[4] = false;
                if (event.key.code == sf::Keyboard::D) dirs[5] = false;
                if (event.key.code == sf::Keyboard::E) dirs[0] = false;
                if (event.key.code == sf::Keyboard::Q) dirs[1] = false;
                if (event.key.code == sf::Keyboard::LShift) movingCamera = false;
            }

            // Mouse movement
            if (event.type == sf::Event::MouseMoved) {
                if (movingCamera) {
                    int centerX = window.getSize().x / 2;
                    int centerY = window.getSize().y / 2;
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    deltaX = mousePos.x - centerX;
                    deltaY = mousePos.y - centerY;
                    sf::Mouse::setPosition(sf::Vector2i(centerX, centerY), window);
                }
            }
        }

        // Time management
        deltaTime = clock.restart().asSeconds();

        // Camera movement
        if (movingCamera) {
            angleY -= deltaX * deltaTime * mouseSensitivityX;
            angleX += deltaY * deltaTime * mouseSensitivityY;

            if (dirs[0]) camPos.y -= moveSpeed * deltaTime;
            if (dirs[1]) camPos.y += moveSpeed * deltaTime;
            if (dirs[2]) {
                camPos.x += sin(-angleY) * moveSpeed * deltaTime;
                camPos.z += cos(-angleY) * moveSpeed * deltaTime;
            }
            if (dirs[3]) {
                camPos.x -= sin(-angleY) * moveSpeed * deltaTime;
                camPos.z -= cos(-angleY) * moveSpeed * deltaTime;
            }
            if (dirs[4]) {
                camPos.x -= sin(-angleY + 3.14 / 2) * moveSpeed * deltaTime;
                camPos.z -= cos(-angleY + 3.14 / 2) * moveSpeed * deltaTime;
            }
            if (dirs[5]) {
                camPos.x += sin(-angleY + 3.14 / 2) * moveSpeed * deltaTime;
                camPos.z += cos(-angleY + 3.14 / 2) * moveSpeed * deltaTime;
            }

            window.setMouseCursorVisible(false);
        }
        else {
            window.setMouseCursorVisible(true);
        }

        // Rendering
        window.clear(sf::Color(105, 105, 105));

        // Rotation matrices
        double rotationX[4][4] = {
            {1, 0, 0, 0},
            {0, cos(angleX), -sin(angleX), 0},
            {0, sin(angleX), cos(angleX), 0},
            {0, 0, 0, 1}
        };

        double rotationY[4][4] = {
            {cos(angleY), 0, sin(angleY), 0},
            {0, 1, 0, 0},
            {-sin(angleY), 0, cos(angleY), 0},
            {0, 0, 0, 1}
        };

        // Projection matrix
        double FoVScale = 1 / tan(FoV * 3.14 / 180.0 / 2.0);
        double normalization = zFar / (zFar - zNear);
        double lambda = (-zFar * zNear) / (zFar - zNear);
        double perspective[4][4] = {
            {FoVScale, 0, 0, 0},
            {0, FoVScale, 0, 0},
            {0, 0, normalization, lambda},
            {0, 0, 1, 0}
        };

        // Triangle queue
        std::priority_queue<std::pair<double, std::pair<std::vector<double>, sf::Color>>,
            std::vector<std::pair<double, std::pair<std::vector<double>, sf::Color>>>,
            CompareTriangles> trisRaster;

        // Clipping arrays
        Vector3 clipped[2][3];

        // Rendering each object
        for (Shape3D* object : objects) {
            const std::vector<int>& tris = object->getTris();
            const std::vector<Vector3>& verts = object->getVerts();

            for (size_t j = 0; j < tris.size(); j += 3) {
                Vector3 projectedVerts3D[3], translatedVertices[3];

                // Transform and project vertices
                for (int k = 0; k < 3; ++k) {
                    int vertIndex = tris[j + k];
                    Vector3 vert = verts[vertIndex];

                    vert.y = -vert.y; 

                    // Translate by camera position
                    vert = vert - camPos;
                    translatedVertices[k] = vert;

                    // Rotate
                    vert = vert.multiplyMatrix(rotationY);
                    vert = vert.multiplyMatrix(rotationX);

                    // Project
                    vert = vert.multiplyMatrix(perspective);
                    projectedVerts3D[k] = vert;
                }

                // Calculate normal
                Vector3 normal = (projectedVerts3D[1] - projectedVerts3D[0]).cross(projectedVerts3D[2] - projectedVerts3D[0]).normalize();
                normal = normal * -1; 
                if (normal.dot(projectedVerts3D[0]) > 0) continue;

                // Lighting
                Vector3 light = lightDir.normalize();
                double dp = (normal.dot(light) + 1) / 2.0;

                // Midpoint distance
                Vector3 midPoint = Vector3::getMidpoint(translatedVertices[0], translatedVertices[1], translatedVertices[2]);
                double midpointDist = midPoint.x * midPoint.x + midPoint.y * midPoint.y + midPoint.z * midPoint.z;

                // Clipping
                int clippedTrianglesCount = Vector3::clipTriangleAgainstPlane(
                    Vector3(0, 0, 0.1), Vector3(0, 0, 1), projectedVerts3D, clipped);

                // Process clipped triangles
                for (int k = 0; k < clippedTrianglesCount; ++k) {
                    std::vector<double> triPoints;
                    Vector3* currTri = clipped[k];

                    for (int l = 0; l < 3; ++l) {
                        Vector3 vert = currTri[l];

                        // Perspective divide and screen space transform
                        vert.x /= (vert.w / 100.0);
                        vert.y /= (vert.w / 100.0);

                        vert.x += window.getSize().x / 2.0;
                        vert.y += window.getSize().y / 2.0;

                        triPoints.push_back(vert.x);
                        triPoints.push_back(vert.y);
                    }

                    // Color shading
                    sf::Color objCol = object->getColor();
                    sf::Color finalColor(
                        static_cast<sf::Uint8>(dp * objCol.r),
                        static_cast<sf::Uint8>(dp * objCol.g),
                        static_cast<sf::Uint8>(dp * objCol.b));

                    trisRaster.push(std::make_pair(midpointDist, std::make_pair(triPoints, finalColor)));
                }
            }
        }

        // Render triangles
        while (!trisRaster.empty()) {
            auto tri = trisRaster.top().second;
            trisRaster.pop();

            sf::ConvexShape polygon;
            polygon.setPointCount(3);
            polygon.setPoint(0, sf::Vector2f(tri.first[0], tri.first[1]));
            polygon.setPoint(1, sf::Vector2f(tri.first[2], tri.first[3]));
            polygon.setPoint(2, sf::Vector2f(tri.first[4], tri.first[5]));
            polygon.setFillColor(tri.second);

            window.draw(polygon);
        }

        // Display
        window.display();
    }

    // Clean up
    for (Shape3D* obj : objects) {
        delete obj;
    }

    return 0;
}
