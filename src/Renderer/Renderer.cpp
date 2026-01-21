#include "Renderer/Renderer.hpp"

Renderer::~Renderer() {
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
void Renderer::uploadAssetMesh(Asset* asset) {

    auto glMesh = std::make_unique<GLMesh>();

    if (!asset || !asset->mesh) {
        std::cout<<"[Renderer] Failed to load mesh"<<std::endl;
        return;
    }

    MeshData* mesh = asset->mesh.get();

    glGenVertexArrays(1, &glMesh->vao);
    glBindVertexArray(glMesh->vao);

    glGenBuffers(1, &glMesh->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, glMesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(Vertex),
                 mesh->vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &glMesh->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glMesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(uint32_t),
                 mesh->indices.data(), GL_STATIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, position));

    // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, normal));

    // UVs
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, uv));

    glMesh->indexCount = mesh->indices.size();

    // Upload first texture if available
    if (!mesh->textures.empty()) {
        auto& tex = mesh->textures[0];
        glGenTextures(1, &glMesh->texture);
        glBindTexture(GL_TEXTURE_2D, glMesh->texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, tex->data.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    glBindVertexArray(0);

    // Store mesh
    mesh_map[asset->name] = std::move(glMesh);
    return;
}


Renderer::Renderer(ResourceManager* resManager, int w, int h) {

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

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

}

void Renderer::drawMesh(const std::string mesh_name) {
    GLMesh* mesh = mesh_map.at(mesh_name).get();
    glBindVertexArray(mesh->vao);
    if (mesh->texture != 0) {
        glBindTexture(GL_TEXTURE_2D, mesh->texture);
    }
    glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    glUseProgram(shaderProgram);

    // VERY IMPORTANT
    glm::mat4 model = glm::mat4(0.5f);
    glm::mat4 view  = glm::lookAt(
        glm::vec3(-3, -5, -3),   // camera position
        glm::vec3(0, 0, 0),   // look at origin
        glm::vec3(0, 1, 0)
    );
    glm::mat4 proj = glm::perspective(
        glm::radians(60.0f),
        (float)width / (float)height,
        0.1f,
        100.0f
    );

    glm::mat4 mvp = proj * view * model;

    GLuint loc = glGetUniformLocation(shaderProgram, "u_MVP");
    glUniformMatrix4fv(loc, 1, GL_FALSE, &mvp[0][0]);


    Asset* asset = resourceManager->getAsset("Naren");
    Asset* map = resourceManager->getAsset("Map");
    if (!asset) return;

    uploadAssetMesh(asset); // only upload once
    drawMesh("Naren");

    SDL_GL_SwapWindow(window);
}