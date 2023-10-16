#ifndef  SCENE_H
#define SCENE_H
#include "Model.h";
#include "Triangle.h";
#include "Material.h";

using namespace std;

class Scene
{
private:
    GLuint trianglesTextureBuffer;
    GLuint nodesTextureBuffer;
    GLuint materialsTextureBuffer;

public:
	vector<Model>models;
	vector<Triangle>triangles;//场景所有三角形
	vector<Material>materials;
	BVH myBVH;
	void Add(Model model)
	{
		models.push_back(model);
	}
	void setupScene()
	{
		for (int i = 0; i < models.size(); i++)
		{
			for (int j = 0; j < models[i].meshes.size(); j++)
			{
				//合并mesh
				triangles.insert(triangles.end(), models[i].meshes[j].triangles.begin(), models[i].meshes[j].triangles.end());
				int len = models[i].meshes[j].triangles.size();
				for (int k = 0; k < len; k++)
				{
                    //std::cout << models[i].meshes[j].material.baseColor.x <<"   "<< models[i].meshes[j].material.baseColor.y << "    " << models[i].meshes[j].material.baseColor.z << endl;
					materials.insert(materials.end(),  models[i].meshes[j].material);
				}
			}
			
			vector<int> vec1 = { 10, 20, 30, 40 };
			vector<char> vec2;

			// inserts at the beginning of vec2 
			vec2.insert(vec2.end(), 3, 4);
		}

		this->myBVH.triangles = this->triangles;
		this->myBVH.buildBVHwithSAH(0, this->triangles.size() - 1);
		std::cout << "SAH BVHNode数量:    " << this->myBVH.nodes.size() << endl;
        buildDatas();
	}
	void Draw()
	{
		for (int i = 0; i < models.size(); i++)
		{
			models[i].Draw();
		}
	}
    void buildDatas()
    {
        //存储整个mesh的信息
        unsigned int tbo;
        //编码三角形
        vector<Triangle_encoded>triangles_encoded(triangles.size());
        for (int i = 0; i < triangles.size(); i++)
        {
            Triangle& t = triangles[i];

            //顶点坐标
            triangles_encoded[i].p1 = t.vertex[0].Position;
            triangles_encoded[i].p2 = t.vertex[1].Position;
            triangles_encoded[i].p3 = t.vertex[2].Position;
            //顶点法线
            triangles_encoded[i].n1 = t.vertex[0].Normal;
            triangles_encoded[i].n2 = t.vertex[1].Normal;
            triangles_encoded[i].n3 = t.vertex[2].Normal;
        }
        //编码node
        vector<BVHNode_encoded>nodes_encoded(this->myBVH.nodes.size());
        for (int i = 0; i < this->myBVH.nodes.size(); i++)
        {
            nodes_encoded[i].childs = vec3(this->myBVH.nodes[i].left, this->myBVH.nodes[i].right, 0);
            nodes_encoded[i].leafInfo = vec3(this->myBVH.nodes[i].n, this->myBVH.nodes[i].index, 0);
            nodes_encoded[i].AA = this->myBVH.nodes[i].AA;
            nodes_encoded[i].BB = this->myBVH.nodes[i].BB;
        }
        
        //编码 material
        vector<Material_encoded>materials_encoded(this->materials.size());
        for (int i = 0; i < materials_encoded.size(); i++)
        {
            materials_encoded[i] = materials[i].encoded();
        }

        GLuint tbo0;
        glGenBuffers(1, &tbo0);
        glBindBuffer(GL_TEXTURE_BUFFER, tbo0);
        //glBufferData(GL_TEXTURE_BUFFER, triangles_encoded.size() * sizeof(Triangle_encoded), &triangles_encoded[0], GL_STATIC_DRAW);
        glBufferData(GL_TEXTURE_BUFFER, triangles_encoded.size() * sizeof(Triangle_encoded), &triangles_encoded[0], GL_STATIC_DRAW);
        glGenTextures(1, &trianglesTextureBuffer);
        glBindTexture(GL_TEXTURE_BUFFER, trianglesTextureBuffer);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo0);

        // BVHNode 数组
        GLuint tbo1;
        glGenBuffers(1, &tbo1);
        glBindBuffer(GL_TEXTURE_BUFFER, tbo1);
        glBufferData(GL_TEXTURE_BUFFER, nodes_encoded.size() * sizeof(BVHNode_encoded), &nodes_encoded[0], GL_STATIC_DRAW);
        glGenTextures(1, &nodesTextureBuffer);
        glBindTexture(GL_TEXTURE_BUFFER, nodesTextureBuffer);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo1);
        
        // Material 数组
        GLuint tbo2;
        glGenBuffers(1, &tbo2);
        glBindBuffer(GL_TEXTURE_BUFFER, tbo2);
        glBufferData(GL_TEXTURE_BUFFER, materials_encoded.size() * sizeof(Material_encoded), &materials_encoded[0], GL_STATIC_DRAW);
        glGenTextures(1, &materialsTextureBuffer);
        glBindTexture(GL_TEXTURE_BUFFER, materialsTextureBuffer);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, tbo2);

        for (int i = 0; i < models.size(); i++)
        {
            models[i].setupDataOfShader(trianglesTextureBuffer, nodesTextureBuffer, materialsTextureBuffer);
        }
    }

};

#endif // ! SCENE_H
