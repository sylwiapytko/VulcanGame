#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include "tiny_obj_loader.h"

#include <vulkan/vulkan.hpp>

#include <fstream>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <array>
#include <set>
#include <unordered_map>

#include "VHHelper.h"

	namespace std {
		template<> struct hash<vh::vhVertex> {
			size_t operator()(vh::vhVertex const& vertex) const {
				//return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
				return hash<glm::vec3>()(vertex.pos) + hash<glm::vec3>()(vertex.normal) + hash<glm::vec3>()(vertex.tangent) + hash<glm::vec2>()(vertex.texCoord);
			}
		};
	}

namespace vh {

	//-------------------------------------------------------------------------------------------------------
	//check validation layers

	std::vector<char> vhFileRead(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}


	void vhFileLoadOBJ(std::string basedir, std::string filename, std::vector<vhVertex> &vertices, std::vector<uint32_t> &indices, float texFactor ) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;

		std::string name = basedir + "\\" + filename;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, name.c_str(), basedir.c_str(), true) ) {
			throw std::runtime_error(err);
		}

		std::unordered_map<vhVertex, uint32_t> uniqueVertices = {};

		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				vhVertex vertex = {};

				vertex.pos = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.texCoord = {
					texFactor * attrib.texcoords[2 * index.texcoord_index + 0],
					texFactor * (1.0f - attrib.texcoords[2 * index.texcoord_index + 1])
				};

				if (attrib.normals.size()>0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}
				else {
					vertex.normal = { 1.0f,1.0f,1.0f };
				}

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}


	//-------------------------------------------------------------------------------------------

	size_t vhCombineHashes(size_t h1, size_t h2) {
		return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
	}

	size_t vhVertexHash(size_t idx_pos, size_t idx_normal, size_t idx_tex, size_t smoothgroup, int32_t materialId) {
		size_t h1 = std::hash<size_t>{}(idx_pos);
		size_t h2 = std::hash<size_t>{}(idx_normal);
		size_t h3 = std::hash<size_t>{}(idx_tex);
		size_t h4 = std::hash<size_t>{}(smoothgroup);
		size_t h5 = std::hash<size_t>{}(materialId);
		return vhCombineHashes(vhCombineHashes(vhCombineHashes(vhCombineHashes(h1, h2), h3), h4), h5);
	}

	glm::vec3 vhComputeNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
		glm::vec3 diff1 = v2 - v1;
		glm::vec3 diff2 = v3 - v1;
		return cross(diff1, diff2);
	}

	void vhComputeTangent(vhFileVertex &v1, vhFileVertex &v2, vhFileVertex &v3) {
		glm::vec3 diff1 = v2.pos - v1.pos;
		glm::vec3 diff2 = v3.pos - v1.pos;

		glm::vec2 deltat1 = v2.texCoord - v1.texCoord;
		glm::vec2 deltat2 = v3.texCoord - v1.texCoord;

		float r = 1.0f / (deltat1.x * deltat2.y - deltat1.y * deltat2.x);
		glm::vec3 tangent = (diff1 * deltat2.y - diff2 * deltat1.y)*r;

		v1.tangent += tangent;
		v2.tangent += tangent;
		v3.tangent += tangent;
	}

	glm::vec3 vhGetPosition(tinyobj::attrib_t &attrib, size_t index) {
		return glm::vec3(attrib.vertices[3 * index], attrib.vertices[3 * index + 1], attrib.vertices[3 * index + 2]);
	}

	glm::vec3 vhGetNormal(tinyobj::attrib_t &attrib, size_t index) {
		return glm::normalize(glm::vec3(attrib.normals[3 * index], attrib.normals[3 * index + 1], attrib.normals[3 * index + 2]));
	}

	glm::vec2 vhGetTexCoord(tinyobj::attrib_t &attrib, size_t index) {
		return glm::vec2(attrib.texcoords[2 * index], 1.0f - attrib.texcoords[2 * index + 1]);
	}


	void vhProcessFace(tinyobj::attrib_t &attrib,
		std::vector<tinyobj::shape_t> &shapes,
		std::vector<tinyobj::material_t> &materials,
		std::unordered_map<size_t, uint32_t> &uniqueVertices,
		size_t s,												// index of shape
		size_t f,												// index of face
		size_t index_offset,									// aggregated sum of vertices as index into indices
		vhFileData &fileData) {

		int fv = shapes[s].mesh.num_face_vertices[f];				// number of vertices per face
		int32_t materialId = shapes[s].mesh.material_ids.size() > 0 ? shapes[s].mesh.material_ids[f] : -1;
		size_t smoothGroup = shapes[s].mesh.smoothing_group_ids.size() > 0 ? shapes[s].mesh.smoothing_group_ids[f] : 0;			// per face smoothing group

		bool averageNormals = false;
		tinyobj::index_t idx1 = shapes[s].mesh.indices[index_offset + 0];		// indices for the first vertex
		glm::vec3 normal;														// indices here is a struct with 3 indices!
		if (attrib.normals.size() == 0 || idx1.normal_index < 0) {				// compute normal of face if there are no normals
			averageNormals = true;
			tinyobj::index_t idx2 = shapes[s].mesh.indices[index_offset + 1];	// second vertex of face
			tinyobj::index_t idx3 = shapes[s].mesh.indices[index_offset + 2];	// third vertex of face -> can compute normal+tangent
			glm::vec3 v1 = vhGetPosition(attrib, idx1.vertex_index);
			glm::vec3 v2 = vhGetPosition(attrib, idx2.vertex_index);
			glm::vec3 v3 = vhGetPosition(attrib, idx3.vertex_index);

			normal = vhComputeNormal(v1, v2, v3);
		}
		else smoothGroup = 0;													// if there are normals, ignore smooth group

		for (size_t v = 0; v < fv; v++) {										// Loop over vertices in the face.
																				// triangle->3, quad->4
			vhFileVertex vertex;												// storage for the vertex	

			tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];	// indices is the array holding all
																				// face indices one after another
																				// index points to vertex, tex, norm

			size_t vertexHash = vhVertexHash(idx.vertex_index, idx.normal_index, idx.texcoord_index, smoothGroup, materialId);
			if (uniqueVertices.count(vertexHash) > 0) {							// we have seen this vertex before
				if (averageNormals) {											// if we average normals then simply add the new normal
					fileData.vertices[uniqueVertices[vertexHash]].normal += normal;		// average the normals in this smooth group
				}
			}
			else {
				vertex.pos = vhGetPosition(attrib, idx.vertex_index);			// a new vertex

				if (attrib.normals.size() > 0 && idx.normal_index >= 0) {		// if normals, then get them
					vertex.normal = vhGetNormal(attrib, idx.normal_index);
				}
				else {															// no normals -> use the computed ones
					vertex.normal = normal;
				}
				if (attrib.texcoords.size() > 0) {								// get texture coordinates
					vertex.texCoord = vhGetTexCoord(attrib, idx.texcoord_index);
				}
				//vertex.materialId = materialId;				// all vertices of the face have the samme matId due to hash!

				uniqueVertices[vertexHash] = static_cast<uint32_t>(fileData.vertices.size());	// index of this new vertex in the vertices array
				fileData.vertices.push_back(vertex);							//store new vertex in the array
			}
			fileData.indices.push_back(uniqueVertices[vertexHash]);				//store the vertex idx in indices array
		}
	}

	bool vhFileLoadOBJ2(std::string basedir, std::string filename, vhFileData &fileData ) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;

		fileData.basedir = basedir;
		fileData.filename = filename;

		std::string fullFilename = basedir + "\\" + filename;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fullFilename.c_str(), basedir.c_str(), true)) {
			return false;
		}

		fileData.materials = materials;
		for (auto &material : fileData.materials) {
			material.name = fullFilename + "\\" + material.name;
		}

		std::unordered_map<size_t, uint32_t> uniqueVertices = {}; //identify indivual vertices

																  // Loop over shapes
		size_t shapeIdxOffset = 0;
		for (size_t s = 0; s < shapes.size(); s++) {			//a shape consists of many faces, either triangles or quads or more
			vhFileShape shape;									//we enforce triangles, but keep code general
			shape.name = shapes[s].name;
			shape.idxOffset = shapeIdxOffset;
			shape.numTriangles = 0;

			// Loop over faces(polygon)
			size_t index_offset = 0;		//index into indices array, must count up since faces can have more than 3 vertices
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {		//loop over all faces of the shape
																						//array contains the num of vertices for each 
																						//face as a sequence, so sum up these numbers

				vhProcessFace(attrib, shapes, fileData.materials, uniqueVertices, s, f, index_offset, fileData);

				index_offset += shapes[s].mesh.num_face_vertices[f];					//go further down the indices array
				shape.numTriangles++;
			}

			shapeIdxOffset += shape.numTriangles;
			fileData.shapes.push_back(shape);
		}

		for (int k = 0; k < fileData.indices.size(); k += 3) {				//all triangles of the mesh
			int idx1 = fileData.indices[k];
			int idx2 = fileData.indices[k + 1];
			int idx3 = fileData.indices[k + 2];
			vhComputeTangent(fileData.vertices[idx1], fileData.vertices[idx2], fileData.vertices[idx3]); //compute tangents
		}

		for (int i = 0; i < fileData.vertices.size(); i++) {
			fileData.vertices[i].normal = glm::normalize(fileData.vertices[i].normal);		//average the computed normals
			fileData.vertices[i].tangent = glm::normalize(fileData.vertices[i].tangent);	//average the computed tangents

			float proj = glm::dot(fileData.vertices[i].normal, fileData.vertices[i].tangent); //projection	
			fileData.vertices[i].tangent -= proj *  fileData.vertices[i].normal;			//make orthogonal again
		}
		return true;
	}

}



