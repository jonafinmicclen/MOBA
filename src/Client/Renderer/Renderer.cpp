#include "Client/Renderer/Renderer.hpp"

Renderer::~Renderer() {
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
[[nodiscard]] MeshId Renderer::uploadAssetMesh(Asset& asset) {
    MeshData& mesh = asset.mesh;

    auto glMesh = std::make_unique<GLMesh>();

    glGenVertexArrays(1, &glMesh->vao);
    glBindVertexArray(glMesh->vao);

    glGenBuffers(1, &glMesh->vbo);
    glGenBuffers(1, &glMesh->ebo);

    struct VertexGPU {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    std::vector<VertexGPU> gpuVertices;
    gpuVertices.reserve(mesh.vertices.size());

    for (const Vertex& vertex : mesh.vertices) {
        gpuVertices.push_back({
            vertex.position,
            vertex.normal,
            vertex.uv
        });
    }

    /*
     * Verify that every triangle has a texture-index entry.
     */
    const std::size_t faceCount = mesh.indices.size() / 3;

    if (mesh.indices.size() % 3 != 0) {
        throw std::runtime_error(
            "Mesh index count is not divisible by three"
        );
    }

    if (mesh.faceTextureIndices.size() != faceCount) {
        throw std::runtime_error(
            "faceTextureIndices count does not match triangle count"
        );
    }

    /*
     * Group triangle indices by texture index.
     *
     * -1 represents triangles with no texture.
     */
    glMesh->textureToFaceIndices.clear();

    for (std::size_t face = 0; face < faceCount; ++face) {
        const int textureIndex =
            mesh.faceTextureIndices[face];

        if (
            textureIndex < -1 ||
            textureIndex >=
                static_cast<int>(asset.textures.size())
        ) {
            throw std::runtime_error(
                "Mesh contains an invalid texture index"
            );
        }

        auto& batch =
            glMesh->textureToFaceIndices[textureIndex];

        const std::size_t firstIndex = face * 3;

        batch.push_back(mesh.indices[firstIndex + 0]);
        batch.push_back(mesh.indices[firstIndex + 1]);
        batch.push_back(mesh.indices[firstIndex + 2]);
    }

    /*
     * Flatten the texture batches into one EBO.
     *
     * Also record where each texture's index range begins.
     */
    glMesh->drawBatches.clear();

    std::vector<std::uint32_t> allIndices;
    allIndices.reserve(mesh.indices.size());

    for (const auto& [textureIndex, batchIndices] :
         glMesh->textureToFaceIndices) {

        GLMesh::DrawBatch drawBatch{};

        drawBatch.textureIndex = textureIndex;
        drawBatch.indexOffset = allIndices.size();
        drawBatch.indexCount = batchIndices.size();

        allIndices.insert(
            allIndices.end(),
            batchIndices.begin(),
            batchIndices.end()
        );

        glMesh->drawBatches.push_back(drawBatch);
    }

    glMesh->indexCount = allIndices.size();

    // Upload vertices.
    glBindBuffer(GL_ARRAY_BUFFER, glMesh->vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(
            gpuVertices.size() * sizeof(VertexGPU)
        ),
        gpuVertices.data(),
        GL_STATIC_DRAW
    );

    // Upload indices.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glMesh->ebo);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(
            allIndices.size() * sizeof(std::uint32_t)
        ),
        allIndices.data(),
        GL_STATIC_DRAW
    );

    // Vertex positions.
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VertexGPU),
        reinterpret_cast<void*>(
            offsetof(VertexGPU, position)
        )
    );

    // Vertex normals.
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VertexGPU),
        reinterpret_cast<void*>(
            offsetof(VertexGPU, normal)
        )
    );

    // Vertex UVs.
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VertexGPU),
        reinterpret_cast<void*>(
            offsetof(VertexGPU, uv)
        )
    );

    /*
     * Upload each unique asset texture exactly once.
     */
    glMesh->textures.resize(asset.textures.size(), 0);

    if (!asset.textures.empty()) {
        glGenTextures(
            static_cast<GLsizei>(glMesh->textures.size()),
            glMesh->textures.data()
        );
    }

    for (std::size_t textureIndex = 0;
         textureIndex < asset.textures.size();
         ++textureIndex) {

        const Texture& texture =
            asset.textures[textureIndex];

        if (
            texture.width <= 0 ||
            texture.height <= 0 ||
            texture.data.empty()
        ) {
            throw std::runtime_error(
                "Cannot upload an empty texture"
            );
        }

        const GLuint openGLTexture =
            glMesh->textures[textureIndex];

        glBindTexture(
            GL_TEXTURE_2D,
            openGLTexture
        );

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA8,
            texture.width,
            texture.height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            texture.data.data()
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            GL_LINEAR_MIPMAP_LINEAR
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER,
            GL_LINEAR
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_S,
            GL_REPEAT
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_T,
            GL_REPEAT
        );

        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    meshes.push_back(std::move(glMesh));

    return MeshId{
        static_cast<std::uint32_t>(
            meshes.size() - 1
        )
    };
}



Renderer::Renderer(int w, int h) {

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // 24-bit depth buffer

    width = w;
    height = h;

        // -- SDL2 + OpenGL Window ---
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return;
    }


    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);



    window = SDL_CreateWindow("League of Legends 2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    glContext = SDL_GL_CreateContext(window);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return;
    }

    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "OpenGL version: " << version << std::endl;

    // Compile shaders
    std::string vertCode = readFileToString("assets/Shaders/vertex_shader.glsl");
    std::string fragCode = readFileToString("assets/Shaders/fragment_shader.glsl");

    shaderProgram = createShaderProgram(vertCode.c_str(), fragCode.c_str());

    glUseProgram(shaderProgram);
    uModel = glGetUniformLocation(shaderProgram, "u_Model");
    uView  = glGetUniformLocation(shaderProgram, "u_View");
    uProj  = glGetUniformLocation(shaderProgram, "u_Projection");
    uTex   = glGetUniformLocation(shaderProgram, "u_Texture");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


}

void Renderer::beginRender() {

    view = camera->getView();
    proj = camera->getProjection((float)width/(float)height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glUseProgram(shaderProgram);

    glUniformMatrix4fv(uView, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(uProj, 1, GL_FALSE, &proj[0][0]);
    
}


void Renderer::drawMesh(
    const MeshId mesh_id,
    const glm::mat4& model
) {
    if (mesh_id.id >= meshes.size()) {
        return;
    }

    GLMesh* mesh = meshes[mesh_id.id].get();

    if (mesh == nullptr) {
        return;
    }

    glUseProgram(shaderProgram);
    glBindVertexArray(mesh->vao);

    glUniformMatrix4fv(
        uModel,
        1,
        GL_FALSE,
        &model[0][0]
    );

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(uTex, 0);

    std::size_t indexOffset = 0;

    for (const auto& [textureIndex, indices] :
         mesh->textureToFaceIndices) {

        const std::size_t indexCount = indices.size();

        if (indexCount == 0) {
            continue;
        }

        if (
            textureIndex >= 0 &&
            static_cast<std::size_t>(textureIndex) <
                mesh->textures.size()
        ) {
            glBindTexture(
                GL_TEXTURE_2D,
                mesh->textures[
                    static_cast<std::size_t>(
                        textureIndex
                    )
                ]
            );
        } else {
            /*
             * This batch had no valid texture.
             * Bind 0, or bind a fallback white texture here.
             */
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        const std::size_t byteOffset =
            indexOffset * sizeof(std::uint32_t);

        glDrawElements(
            GL_TRIANGLES,
            static_cast<GLsizei>(indexCount),
            GL_UNSIGNED_INT,
            reinterpret_cast<const void*>(
                byteOffset
            )
        );

        indexOffset += indexCount;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}


void Renderer::endRender() {
    SDL_GL_SwapWindow(window);
}

void Renderer::testMesh(glm::vec3 translation) {

}