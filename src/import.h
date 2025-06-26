#ifndef IMPORT_H_INCLUDED
#define IMPORT_H_INCLUDED
#include "data_structures.h"
#include <fstream>

//pushes back another 3d object to "vector<obj_3d> obects" to be automaticly rendered
//you will have to manually specify, what texture it has
inline bool import_obj(std::string name, std::string pathto) {
    obj_3d obj;
    std::ifstream file;
    std::string buffer;
    file.open(pathto + name);

    // Clear existing data
    obj.vert.clear();
    obj.uv.clear();

    // Temporary storage
    std::vector<float> verts;     // Raw vertices (v)
    std::vector<float> uvs;        // Raw UVs (vt)
    std::vector<int> vert_indices; // Vertex indices from faces
    std::vector<int> uv_indices;   // UV indices from faces

    while (getline(file, buffer)) {
        std::istringstream ss(buffer);
        std::string type;
        ss >> type;

        // Parse vertices
        if (type == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            verts.push_back(x);
            verts.push_back(y);
            verts.push_back(z);
        }
        // Parse texture coordinates
        else if (type == "vt") {
            float u, v;
            ss >> u >> v;
            uvs.push_back(u);
            uvs.push_back(v);
        }
        // Parse faces (supports triangles and quads)
        else if (type == "f") {
            std::vector<std::string> tokens;
            std::string token;
            while (ss >> token) tokens.push_back(token);

            // Triangulate polygons
            for (int i = 1; i < tokens.size() - 1; i++) {
                // Process each vertex in the triangle
                for (int j : {0, i, i+1}) {
                    std::istringstream v_stream(tokens[j]);
                    std::string part;
                    std::vector<std::string> parts;

                    // Split vertex/texture/normal indices
                    while (getline(v_stream, part, '/')) parts.push_back(part);

                    // Vertex index (always present)
                    int v_idx = std::stoi(parts[0]) - 1;
                    vert_indices.push_back(v_idx);

                    // UV index (if exists)
                    if (parts.size() >= 2 && !parts[1].empty()) {
                        int uv_idx = std::stoi(parts[1]) - 1;
                        uv_indices.push_back(uv_idx);
                    } else {
                        uv_indices.push_back(-1); // Mark missing UV
                    }
                }
            }
        }
    }
    file.close();

    // Build vertex array
    for (int idx : vert_indices) {
        obj.vert.push_back(verts[idx * 3]);
        obj.vert.push_back(verts[idx * 3 + 1]);
        obj.vert.push_back(verts[idx * 3 + 2]);
    }

    // Build UV array if valid UVs exist
    bool valid_uvs = !uv_indices.empty() &&
        std::find(uv_indices.begin(), uv_indices.end(), -1) == uv_indices.end();

    if (valid_uvs) {
        for (int idx : uv_indices) {
            obj.uv.push_back(uvs[idx * 2]);
            obj.uv.push_back(uvs[idx * 2 + 1]);
        }
    }
    objects.push_back(obj);
    return true;
}

//pushes back another texture, into the textures vector
inline bool import_ppm(const std::string& filename,const std::string& pathto) {
    texture text;
    std::ifstream file(pathto+filename, std::ios::binary);
    if (!file.is_open()) {
        return false;  // Failed to open file
    }

    text.texture.clear();
    text.dim_x = 0;
    text.dim_y = 0;

    std::string line;
    int max_val = 0;

    // Read magic number
    std::getline(file, line);

    // Read header (skip comments)
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);

        // Read dimensions if not set
        if (text.dim_x == 0) {
            if (!(iss >> text.dim_x >> text.dim_y)) {
                // Try next line if this one doesn't have both dimensions
                continue;
            }
        }

        // Read max value if dimensions are set
        if (text.dim_x > 0 && max_val == 0) {
            if (!(iss >> max_val)) {
                // Max value might be on next line
                std::getline(file, line);
                std::istringstream max_iss(line);
                max_iss >> max_val;
            }
            break;
        }
    }

    // Validate header
    if (text.dim_x <= 0 || text.dim_y <= 0 || max_val <= 0 || max_val > 65535) {
        file.close();
        return false;
    }

    // Skip single whitespace after header
    if (file.peek() == '\n') file.get();
    if (file.peek() == '\r') file.get();

    // Calculate data size and reserve space
    const size_t pixel_count = text.dim_x * text.dim_y;
    const size_t data_size = pixel_count * 3;
    std::vector<unsigned char> pixel_data(data_size);

    // Read binary pixel data
    file.read(reinterpret_cast<char*>(pixel_data.data()), data_size);
    file.close();

    // Convert to int vector (0-255)
    text.texture.reserve(data_size);
    for (unsigned char pixel : pixel_data) {
        // Scale value if max_val != 255
        int scaled = pixel;
        if (max_val != 255) {
            scaled = static_cast<int>(pixel * 255.0f / max_val);
            scaled = std::clamp(scaled, 0, 255);
        }
        text.texture.push_back(scaled);
    }
    vector<int> temp;
    for (int a=0;a<(text.texture.size()/3);a++){

        temp.push_back(text.texture[a*3]);
        temp.push_back(text.texture[a*3+1]);
        temp.push_back(text.texture[a*3+2]);
    }
    text.texture=temp;

   textures.push_back(text);
   return true;
}

#endif // IMPORT_H_INCLUDED
