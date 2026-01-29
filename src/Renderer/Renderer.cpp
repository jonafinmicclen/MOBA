#include "Renderer/Renderer.hpp"

Renderer::~Renderer() {
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
void Renderer::uploadAssetMesh(Asset* asset) {
    if (!asset || !asset->mesh) {
        std::cout << "[Renderer] Failed to load mesh" << std::endl;
        return;
    }

    MeshData* mesh = asset->mesh.get();
    auto glMesh = std::make_unique<GLMesh>();

    glGenVertexArrays(1, &glMesh->vao);
    glBindVertexArray(glMesh->vao);

    glGenBuffers(1, &glMesh->vbo);
    glGenBuffers(1, &glMesh->ebo);

    // Vertex struct now includes texID
    struct VertexGPU {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
        int texID;
    };

    // Duplicate vertices per-face to assign correct texID
    std::vector<VertexGPU> gpuVertices;
    std::vector<uint32_t> gpuIndices;

    for (size_t i = 0; i < mesh->indices.size(); i += 3) {
        int faceTexID = (i / 3 < mesh->faceTextureIndices.size()) ? mesh->faceTextureIndices[i / 3] : 0;

        for (int j = 0; j < 3; ++j) {
            uint32_t idx = mesh->indices[i + j];
            Vertex& v = mesh->vertices[idx];

            VertexGPU vg{};
            vg.position = v.position;
            vg.normal = v.normal;
            vg.uv = v.uv;
            vg.texID = faceTexID;

            gpuIndices.push_back(static_cast<uint32_t>(gpuVertices.size()));
            gpuVertices.push_back(vg);
        }
    }

    // Upload vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, glMesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, gpuVertices.size() * sizeof(VertexGPU), gpuVertices.data(), GL_STATIC_DRAW);

    // Upload index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glMesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gpuIndices.size() * sizeof(uint32_t), gpuIndices.data(), GL_STATIC_DRAW);

    // Vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexGPU), (void*)offsetof(VertexGPU, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexGPU), (void*)offsetof(VertexGPU, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexGPU), (void*)offsetof(VertexGPU, uv));

    // Per-vertex texID
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 1, GL_INT, sizeof(VertexGPU), (void*)offsetof(VertexGPU, texID));

    glMesh->indexCount = gpuIndices.size();

    // Upload textures
    glMesh->textures.resize(mesh->textures.size());
    for (size_t i = 0; i < mesh->textures.size(); ++i) {
        auto& tex = mesh->textures[i];
        GLuint texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->data.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glMesh->textures[i] = texID;
    }

    glBindVertexArray(0);
    mesh_map[asset->name] = std::move(glMesh);
}

void Renderer::drawMesh(const std::string& mesh_name, const glm::mat4& model) {
    GLMesh* mesh = mesh_map.at(mesh_name).get();
    if (!mesh) return;

    glBindVertexArray(mesh->vao);
    glUseProgram(shaderProgram);

    // Upload matrices
    GLuint locModel = glGetUniformLocation(shaderProgram, "u_Model");
    glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);

    GLuint locView = glGetUniformLocation(shaderProgram, "u_View");
    glUniformMatrix4fv(locView, 1, GL_FALSE, &view[0][0]);

    GLuint locProj = glGetUniformLocation(shaderProgram, "u_Projection");
    glUniformMatrix4fv(locProj, 1, GL_FALSE, &proj[0][0]);

    // Bind all textures
    for (size_t i = 0; i < mesh->textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, mesh->textures[i]);

        std::string samplerName = "u_Textures[" + std::to_string(i) + "]";
        GLuint locSampler = glGetUniformLocation(shaderProgram, samplerName.c_str());
        glUniform1i(locSampler, static_cast<int>(i));
    }

    // Draw
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->indexCount), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}



Renderer::Renderer(ResourceManager* resManager, int w, int h) {

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // 24-bit depth buffer

    width = w;
    height = h;

    resourceManager = resManager;

        // -- SDL2 + OpenGL Window ---
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return;
    }


    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


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

        // Camera/view/projection
    view = glm::lookAt(
        glm::vec3(-3, -5, -3),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0)
    );

    proj = glm::perspective(
        glm::radians(60.0f),
        (float)width / (float)height,
        0.1f,
        100.0f
    );

    glEnable(GL_DEPTH_TEST);           // Enable depth testing
    glDepthFunc(GL_LESS);              // Accept fragment if it is closer than the current depth


}

void Renderer::beginRender() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glUseProgram(shaderProgram);

    GLuint locView = glGetUniformLocation(shaderProgram, "u_View");
    glUniformMatrix4fv(locView, 1, GL_FALSE, &view[0][0]);

    GLuint locProj = glGetUniformLocation(shaderProgram, "u_Projection");
    glUniformMatrix4fv(locProj, 1, GL_FALSE, &proj[0][0]);
}

void Renderer::endRender() {
    SDL_GL_SwapWindow(window);
}

void Renderer::testMesh(glm::vec3 translation) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    model = glm::rotate(model, (float)translation.x, glm::vec3(0,1,0));
    model = glm::scale(model, glm::vec3(2.0f));
    drawMesh("Naren", model);

    model = glm::mat4(1.0f);
    model = glm::translate(model, translation);
    model = glm::scale(model, glm::vec3(1.0f));
    drawMesh("Summoners Rift", model);
}