#include "halftorus.h"
#include <iostream>

namespace Models {

    HalfTorus halfTorus;

    HalfTorus::HalfTorus() {
        buildHalfTorus(0.5, 0.25, 20, 20); // Adjust radius and divisions as needed
    }

    HalfTorus::HalfTorus(float R, float r, float mainDivs, float tubeDivs) {
        buildHalfTorus(R, r, mainDivs, tubeDivs);
    }

    HalfTorus::~HalfTorus() {
    }

    inline float HalfTorus::d2r(float deg) {
        return PI * deg / 180.0f;
    }

    vec4 HalfTorus::generateHalfTorusPoint(float R, float r, float alpha, float beta) {
        alpha = d2r(alpha);
        beta = d2r(beta);
        return vec4((R + r * cos(alpha)) * cos(beta), (R + r * cos(alpha)) * sin(beta), r * sin(alpha), 1.0f);
    }

    vec4 HalfTorus::computeVertexNormal(float alpha, float beta) {
        alpha = d2r(alpha);
        beta = d2r(beta);
        return vec4(cos(alpha) * cos(beta), cos(alpha) * sin(beta), sin(alpha), 0.0f);
    }

    vec4 HalfTorus::computeFaceNormal(vector<vec4>& face) {
        vec3 a = vec3(face[1] - face[0]);
        vec3 b = vec3(face[2] - face[0]);

        return normalize(vec4(cross(b, a), 0.0f));
    }

    void HalfTorus::generateHalfTorusFace(vector<vec4>& vertices, vector<vec4>& vertexNormals, vec4& faceNormal, float R, float r, float alpha, float beta, float step_alpha, float step_beta) {
        vertices.clear();
        vertexNormals.clear();

        vertices.push_back(generateHalfTorusPoint(R, r, alpha, beta));
        vertices.push_back(generateHalfTorusPoint(R, r, alpha + step_alpha, beta));
        vertices.push_back(generateHalfTorusPoint(R, r, alpha + step_alpha, beta + step_beta));
        vertices.push_back(generateHalfTorusPoint(R, r, alpha, beta + step_beta));

        faceNormal = computeFaceNormal(vertices);

        vertexNormals.push_back(computeVertexNormal(alpha, beta));
        vertexNormals.push_back(computeVertexNormal(alpha + step_alpha, beta));
        vertexNormals.push_back(computeVertexNormal(alpha + step_alpha, beta + step_beta));
        vertexNormals.push_back(computeVertexNormal(alpha, beta + step_beta));
    }

    void HalfTorus::buildHalfTorus(float R, float r, float mainDivs, float tubeDivs) {
        vector<vec4> face;
        vector<vec4> faceVertexNormals;
        vec4 normal;

        internalVertices.clear();
        internalFaceNormals.clear();
        internalVertexNormals.clear();

        float mult_alpha = 360.0f / tubeDivs;
        float mult_beta = 180.0f / mainDivs; // 180 degrees for half torus

        for (int alpha = 0; alpha < round(tubeDivs); alpha++) {
            for (int beta = 0; beta < round(mainDivs); beta++) {

                generateHalfTorusFace(face, faceVertexNormals, normal, R, r, alpha * mult_alpha, beta * mult_beta, mult_alpha, mult_beta);

                internalVertices.push_back(face[0]);
                internalVertices.push_back(face[1]);
                internalVertices.push_back(face[2]);

                internalVertices.push_back(face[0]);
                internalVertices.push_back(face[2]);
                internalVertices.push_back(face[3]);

                internalVertexNormals.push_back(faceVertexNormals[0]);
                internalVertexNormals.push_back(faceVertexNormals[1]);
                internalVertexNormals.push_back(faceVertexNormals[2]);

                internalVertexNormals.push_back(faceVertexNormals[0]);
                internalVertexNormals.push_back(faceVertexNormals[2]);
                internalVertexNormals.push_back(faceVertexNormals[3]);

                for (int i = 0; i < 6; i++) internalFaceNormals.push_back(normal);

            }
        }

        vertices = (float*)internalVertices.data();
        normals = (float*)internalFaceNormals.data();
        vertexNormals = (float*)internalVertexNormals.data();
        texCoords = vertexNormals;
        vertexCount = internalVertices.size();
    }

    void HalfTorus::drawSolid(bool smooth) {
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, vertices);
        if (!smooth) glVertexAttribPointer(1, 4, GL_FLOAT, false, 0, normals);
        else glVertexAttribPointer(1, 4, GL_FLOAT, false, 0, vertexNormals);
        glVertexAttribPointer(2, 4, GL_FLOAT, false, 0, texCoords);
        //cout << "vertex count: " << vertexCount;
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }

}
