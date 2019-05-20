//
// Created by hackyhacker on 18/05/19.
//

#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"
#include "Mesh.h"

class Model
{
public:
    vector<Texture> textures_loaded;
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;

    Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    void Draw(Shader shader, glm::mat4 model, glm::mat4 projection, glm::mat4 view)
    {
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader, model, projection, view);
    }

private:
    void loadModel(string const &path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    //unsigned int TextureFromfile(const char *path, const string &directory, bool gamma);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

