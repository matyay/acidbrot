#include <utils/stringf.hh>

#include "acidbrot_app.hh"

#include <gl/utils.hh>
#include <gl/primitives.hh>

// ============================================================================

AcidbrotApp::AcidbrotApp () :
    GLFWApp()
{
    // Try initializing the app
    int res = initialize();
    if (res) {
        throw std::runtime_error(
            stringf("Error initializing application! (%d)", res)
        );
    }
}

// ============================================================================

int AcidbrotApp::initialize () {
    m_Logger->info("Init");

    // Hints
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_DOUBLEBUFFER, true);

    glfwWindowHint(GLFW_RESIZABLE, false);

    // Create the window
    m_Window = glfwCreateWindow(1280, 700, "Acid-brot", nullptr, nullptr);
    if (m_Window == nullptr) {
        m_Logger->error("Error creating window!");
        return -1;
    }

    addWindow(m_Window);

    // Set OpenGL context
    glfwMakeContextCurrent(m_Window);    
    glfwSwapInterval(1);

    // FIXME: This should be a generic wrapper
    // Initialize GLEW
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK) {
        m_Logger->error("glewInit() Failed! {} ({})", glewGetErrorString(glewStatus), glewStatus);
        return -1;
    }

    // Print OpenGL renderer info
    GL::dumpRendererInfo();
    // Print OpenGL capabilities
    GL::dumpRendererCaps();

    m_HaveFp64 = GL::isExtensionAvailable("GL_ARB_gpu_shader_fp64");
    m_Logger->info("m_HaveFp64 {}", m_HaveFp64);

    // ..........................................

    m_Fonts["generic"] = std::unique_ptr<GL::Font>(new GL::Font("media/fonts/Roboto-Regular.ttf"));

    // ..........................................

    GL::Shader vshGenericF     ("shaders/generic2d.vsh",     GL_VERTEX_SHADER);
    GL::Shader fshMandelbrot   ("shaders/mandelbrot32.fsh",    GL_FRAGMENT_SHADER);

    m_Shaders["font"]          = std::unique_ptr<GL::ShaderProgram>(new GL::GenericFontShader());

    m_Shaders["mandelbrot"]    = std::unique_ptr<GL::ShaderProgram>(new GL::ShaderProgram(vshGenericF,    fshMandelbrot, "mandelbrot"));

    // ..........................................

    m_Textures["colormap"]  = std::unique_ptr<GL::Texture>(
        new GL::Texture("media/colormap.png", GL_LINEAR, GL_MIRRORED_REPEAT)
    );

    // ..........................................

    // Initialize the viewport
    for (size_t i=0; i<6; ++i) {
        m_Viewport.position[i] = 0.0;
        m_Viewport.velocity[i] = 0.0;
    }

    m_Viewport.position[2] = -1.0;

    return 0;
}

// ============================================================================

int AcidbrotApp::loop (double dt) {

    // Escape
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        return 1;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_Q) == GLFW_PRESS) {
        return 1;
    }

    // ................................
    // Input

    std::array<double, 6> rawVelocity = {0.0, 0.0, 0.0, 0.0};

    if (glfwGetKey(m_Window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        rawVelocity[0] = -1.0;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        rawVelocity[0] = +1.0;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        rawVelocity[1] = -1.0;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_UP) == GLFW_PRESS) {
        rawVelocity[1] = +1.0;
    }

    if (glfwGetKey(m_Window, GLFW_KEY_MINUS) == GLFW_PRESS) {
        rawVelocity[2] = -1.0;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
        rawVelocity[2] = +1.0;
    }

    if (glfwGetKey(m_Window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
        rawVelocity[3] = -1.0f;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
        rawVelocity[3] = +1.0f;
    }

    if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS) {
        rawVelocity[4] = -1.0;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS) {
        rawVelocity[4] = +1.0;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS) {
        rawVelocity[5] = -1.0;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS) {
        rawVelocity[5] = +1.0;
    }

    // ................................
    // Viewport motion
    {
        const double k = 0.90;

        // More zoom = slower
        float zoom = powf(2.0, m_Viewport.position[2]);
        rawVelocity[0] /= zoom;
        rawVelocity[1] /= zoom;

        rawVelocity[3] *= 0.2f;
        rawVelocity[2] *= 2.0f;
        rawVelocity[4] *= 0.1f;
        rawVelocity[5] *= 0.1f;

        // Filter velocity
        for (size_t i=0; i<6; ++i) {
            m_Viewport.velocity[i] = m_Viewport.velocity[i] * k + rawVelocity[i] * (1.0 - k);
        }
        m_Viewport.velocity[3] = rawVelocity[3];

        // Move viewport
        for (size_t i=0; i<6; ++i) {
            m_Viewport.position[i] += m_Viewport.velocity[i] * dt;
        }

        // Zooming more makes no sense due to precision
        if (m_Viewport.position[2] > 44.0f) {
            m_Viewport.position[2] = 44.0f;
        }
    }

    // ................................
    // Initialize GL rendering

    int winWidth, winHeight;
    glfwGetFramebufferSize(m_Window, &winWidth, &winHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GL_CHECK(glViewport(0, 0, winWidth, winHeight));

    GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    // ................................
    // Render fracta
    {
        float u0, v0, u1, v1;

        GL::ShaderProgram* shaderProgram = m_Shaders.at("mandelbrot").get();
        GL_CHECK(glUseProgram(shaderProgram->get()));

//        GL_CHECK(glActiveTexture(GL_TEXTURE0));
//        GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_Textures.at("colormap")->get()));
//        GL_CHECK(glUniform1f(shaderProgram->getUniformLocation("colormapPos"), m_Viewport.position[3]));

        GL_CHECK(glUniform2f(shaderProgram->getUniformLocation("fractalPos"),  m_Viewport.position[0], m_Viewport.position[1]));
        GL_CHECK(glUniform1f(shaderProgram->getUniformLocation("fractalScale"), pow(2.0, m_Viewport.position[2])));
        //GL_CHECK(glUniform2d(shaderProgram->getUniformLocation("juliaC"), m_Viewport.position[4], m_Viewport.position[5]));

        GL_CHECK(glDisable(GL_BLEND));

        float viewport[4];
        GL_CHECK(glGetFloatv(GL_VIEWPORT, viewport));

        float aspect = viewport[2] / viewport[3];

        u0 = -1.0f;
        u1 = +1.0f;
        v0 = -1.0f / aspect;
        v1 = +1.0f / aspect;

        GL_CHECK(GL::drawRectangle(-1.0f, -1.0f, +1.0f, +1.0f, u0, v0, u1, v1));
    }

    // ................................
    // Text
    {
        GL::ShaderProgram* shaderProgram = m_Shaders.at("font").get();

        GL_CHECK(glEnable(GL_BLEND));
        GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        GL_CHECK(glUseProgram(shaderProgram->get()));

        float viewport[4];
        GL_CHECK(glGetFloatv(GL_VIEWPORT, viewport));
        GL_CHECK(glUniform4fv(shaderProgram->getUniformLocation("viewport"), 1, viewport));

        // Frame rate
        GL_CHECK(glUniform4f(shaderProgram->getUniformLocation("color"), 1, 0, 0, 0.75f));
        m_Fonts.at("generic")->drawText(2, viewport[3] - 16-2, "Frame rate: %.1f FPS", getFrameRate());

        GL_CHECK(glUniform4f(shaderProgram->getUniformLocation("color"), 1, 1, 1, 0.75f));
        m_Fonts.at("generic")->drawText(2, viewport[3] - 32-2, stringf("X:%.15f Y:%.15f Z:%.3f C:%.15f (%.15f, %.15f)",
            m_Viewport.position[0], m_Viewport.position[1], m_Viewport.position[2], m_Viewport.position[3], m_Viewport.position[4], m_Viewport.position[5]));

        GL_CHECK(glUseProgram(0));
    }

    // ................................
    // Present

    // Show
    GL_CHECK(glFlush());

    // Swap buffers
    glfwSwapBuffers(m_Window);
    frameDone();

    return 0;
}

