#pragma once

#include <string>
#include <vector>
using std::string, std::vector;

#include <graphics/mesh.h>

namespace objloader {

    vector<Mesh> LoadModel(const string &filepath);
}