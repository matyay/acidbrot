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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
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

    GL::Shader vshGeneric      ("shaders/generic2d.vsh",       GL_VERTEX_SHADER);
    GL::Shader fshMandelbrot   ("shaders/mandelbrot32.fsh",    GL_FRAGMENT_SHADER, {{"MANDELBROT", "1"}});
    GL::Shader fshJulia        ("shaders/mandelbrot32.fsh",    GL_FRAGMENT_SHADER, {{"JULIA", "1"}});
    GL::Shader fshColorizer    ("shaders/colorizer.fsh",       GL_FRAGMENT_SHADER);
    GL::Shader fshDespeckle    ("shaders/despeckle.fsh",       GL_FRAGMENT_SHADER);

    m_Shaders["font"]       = std::unique_ptr<GL::ShaderProgram>(new GL::GenericFontShader());

    m_Shaders["mandelbrot"] = std::unique_ptr<GL::ShaderProgram>(new GL::ShaderProgram(
        vshGeneric,
        fshMandelbrot,
        "mandelbrot"
        ));

    m_Shaders["julia"]      = std::unique_ptr<GL::ShaderProgram>(new GL::ShaderProgram(
        vshGeneric,
        fshJulia,
        "julia"
        ));

    m_Shaders["colorizer"]  = std::unique_ptr<GL::ShaderProgram>(new GL::ShaderProgram(
        vshGeneric,
        fshColorizer,
        "colorizer"
        ));

    m_Shaders["despeckle"]  = std::unique_ptr<GL::ShaderProgram>(new GL::ShaderProgram(
        vshGeneric,
        fshDespeckle,
        "despeckle"
        ));

    //GL::Shader vshGeometry ("shaders/temp/geometry.vsh", GL_VERTEX_SHADER);
    //GL::Shader gshGeometry ("shaders/temp/geometry.gsh", GL_GEOMETRY_SHADER);
    //GL::Shader fshGeometry ("shaders/temp/geometry.fsh", GL_FRAGMENT_SHADER);

/*    m_Shaders["geometry"] = std::unique_ptr<GL::ShaderProgram>(new GL::ShaderProgram(
        vshGeometry,
        fshGeometry,
        "geometry"
        ));
*/

    // ..........................................

    m_Framebuffers["fractalRaw"] = std::unique_ptr<GL::Framebuffer>(
        new GL::Framebuffer(1280, 700, GL_RGBA, 1, false)
    );

    m_Framebuffers["fractalFlt"] = std::unique_ptr<GL::Framebuffer>(
        new GL::Framebuffer(1280, 700, GL_RGBA, 1, false)
    );

    // ..........................................

    m_Textures["colormap"]  = std::unique_ptr<GL::Texture>(
        new GL::Texture("media/colormap.png", GL_LINEAR, GL_MIRRORED_REPEAT)
    );

    // ..........................................

    // Initialize the viewport
    size_t paramCount = sizeof(Viewport) / sizeof(double);
    for (size_t i=0; i<paramCount; ++i) {
        m_Viewport.position.param[i] = 0.0;
        m_Viewport.velocity.param[i] = 0.0;
    }

    m_Viewport.position.zoom = -1.0f;

    m_Viewport.position.julia[0] = 0.7885f;
    m_Viewport.position.julia[1] = 0.0f;

    return 0;
}

// ============================================================================

int AcidbrotApp::loop (double dt) {

    // Escape
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        return 1;
    }

    // ................................
    // Input
    size_t   paramCount = sizeof(Viewport) / sizeof(double);
    Viewport control;

    for (size_t i=0; i<paramCount; ++i) {
        control.param[i] = 0.0;
    }

    // Lateral navigation
    if (glfwGetKey(m_Window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        control.position[0] = -1.0;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        control.position[0] = +1.0;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        control.position[1] = -1.0;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_UP) == GLFW_PRESS) {
        control.position[1] = +1.0;
    }

    // Rotation
    if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS) {
        control.rotation = -1.0;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS) {
        control.rotation = +1.0;
    }

    // Zooming
    if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS) {
        control.zoom = -1.0;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS) {
        control.zoom = +1.0;
    }

    // Colorization
    if (glfwGetKey(m_Window, GLFW_KEY_Z) == GLFW_PRESS) {
        control.color = -1.0;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_C) == GLFW_PRESS) {
        control.color = +1.0;
    }

    // Julia set abs(C)
    if (glfwGetKey(m_Window, GLFW_KEY_1) == GLFW_PRESS) {
        control.julia[0] = -1.0;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_3) == GLFW_PRESS) {
        control.julia[0] = +1.0;
    }

    // Julia set angle(C)
    if (glfwGetKey(m_Window, GLFW_KEY_Q) == GLFW_PRESS) {
        control.julia[1] = -1.0;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_E) == GLFW_PRESS) {
        control.julia[1] = +1.0;
    }

    // ................................
    // Viewport motion
    {
        const double k = 0.90;

        // More zoom -> slower lateral motion
        float zoom = powf(2.0, m_Viewport.position.zoom);
        control.position[0] /= zoom;
        control.position[1] /= zoom;

        // Rotate lateral motion vector
        float s = sinf(m_Viewport.position.rotation);
        float c = cosf(m_Viewport.position.rotation);

        std::array<double, 2> newPosition;
        newPosition[0] = c * control.position[0] - s * control.position[1];
        newPosition[1] = s * control.position[0] + c * control.position[1];
        control.position = newPosition;

        // Rotation speed
        control.rotation *= 0.5f;

        // Zooming speed
        control.zoom  *= 2.0f;

        // Colorization speed
        control.color *= 0.2f;

        // Julia set abs(C), angle(C)
        control.julia[0] *= 0.2f;
        control.julia[1] *= 0.2f;

        // Filter velocity
        for (size_t i=0; i<paramCount; ++i) {
            m_Viewport.velocity.param[i] = m_Viewport.velocity.param[i] * k +
                                           control.param[i] * (1.0 - k);
        }

        // Move the viewport
        for (size_t i=0; i<paramCount; ++i) {
            m_Viewport.position.param[i] += m_Viewport.velocity.param[i] * dt;
        }

        // Zooming more makes no sense due to precision
        double maxZoom = (m_HaveFp64) ? 44.0f : 15.0f;
        if (m_Viewport.position.zoom > maxZoom) {
            m_Viewport.position.zoom = maxZoom;
        }

        if (m_Viewport.position.zoom < -1.0f) {
            m_Viewport.position.zoom = -1.0f;
        }

        // No negative abs(C)
        if (m_Viewport.position.julia[0] < 0.0f) {
            m_Viewport.position.julia[0] = 0.0f;
        }
    }

    // ................................
    // Initialize GL rendering

    int winWidth, winHeight;
    glfwGetFramebufferSize(m_Window, &winWidth, &winHeight);

    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CHECK(glViewport(0, 0, winWidth, winHeight));

    GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    // ................................
    // Generate the fractal data
    {
        GL::Framebuffer* framebuffer = m_Framebuffers.at("fractalRaw").get();
        framebuffer->enable();

        GL::ShaderProgram* shaderProgram = m_Shaders.at("julia").get();
        GL_CHECK(glUseProgram(shaderProgram->get()));

        float juliaC[2] = {
            m_Viewport.position.julia[0] * cosf(m_Viewport.position.julia[1]),
            m_Viewport.position.julia[0] * sinf(m_Viewport.position.julia[1])
        };

        GL_CHECK(glUniform2f(shaderProgram->getUniformLocation("fractalPosition"),
                    m_Viewport.position.position[0],
                    m_Viewport.position.position[1]
                    ));

        GL_CHECK(glUniform1f(shaderProgram->getUniformLocation("fractalRotation"),
                    m_Viewport.position.rotation
                    ));

        GL_CHECK(glUniform1f(shaderProgram->getUniformLocation("fractalScale"),
                    pow(2.0, m_Viewport.position.zoom)
                    ));

        GL_CHECK(glUniform2f(shaderProgram->getUniformLocation("fractalCoeff"),
                    juliaC[0],
                    juliaC[1]
                    ));

        GL_CHECK(glDisable(GL_BLEND));

        float viewport[4];
        GL_CHECK(glGetFloatv(GL_VIEWPORT, viewport));

        float aspect = viewport[2] / viewport[3];

        float u0 = -1.0f;
        float u1 = +1.0f;
        float v0 = -1.0f / aspect;
        float v1 = +1.0f / aspect;

        GL_CHECK(GL::drawRectangle(-1.0f, -1.0f, +1.0f, +1.0f, u0, v0, u1, v1));

        GL_CHECK(glUseProgram(0));
        framebuffer->disable();
    }

    // ................................
    // Filter the fractal
    {
        GL::ShaderProgram* shader = m_Shaders.at("despeckle").get();
        GL::Framebuffer*   fbSrc  = m_Framebuffers.at("fractalRaw").get();
        GL::Framebuffer*   fbDst  = m_Framebuffers.at("fractalFlt").get();

        float weights [9] = {
            0.25f, 1.00f, 0.25f,
            1.00f, 1.00f, 1.00f,
            0.25f, 1.00f, 0.25f
        };

        float offsets [9*2];
        for (int j=0; j<=2; ++j) {
            for (int i=0; i<=2; ++i) {
                int ii = 2 * (j*3 + i);
                offsets[ii+0] = (float)(i - 1) / (float)winWidth;
                offsets[ii+1] = (float)(j - 1) / (float)winHeight;
            }
        }

        // Setup
        fbDst->enable();
        GL_CHECK(glUseProgram(shader->get()));

        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, fbSrc->getTexture()));

        GL_CHECK(glUniform1fv(shader->getUniformLocation("filterWeights"), 9, weights));
        GL_CHECK(glUniform2fv(shader->getUniformLocation("filterOffsets"), 9, offsets));

        // Render
        GL::drawFullscreenRect();

        // Cleanup
        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

        GL_CHECK(glUseProgram(0));
        fbDst->disable();
    }

    // ................................
    // Colorize the fractal
    {
        GL::Framebuffer*   framebuffer = m_Framebuffers.at("fractalFlt").get();
        GL::ShaderProgram* shader      = m_Shaders.at("colorizer").get();

        // Setup
        GL_CHECK(glUseProgram(shader->get()));

        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, framebuffer->getTexture()));
        GL_CHECK(glUniform1i(shader->getUniformLocation("fractal"), 0));

        GL_CHECK(glActiveTexture(GL_TEXTURE1));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_Textures.at("colormap")->get()));
        GL_CHECK(glUniform1i(shader->getUniformLocation("colormap"), 1));

        GL_CHECK(glUniform1f(shader->getUniformLocation("colormapPos"), m_Viewport.position.color));

        // Render
        GL::drawFullscreenRect();

        // Cleanup
        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
        GL_CHECK(glActiveTexture(GL_TEXTURE1));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

        GL_CHECK(glUseProgram(0));
    }


    // ................................
    // Geometry
/*    {
        float points[] = {
          -0.45f,  0.45f,
           0.45f,  0.45f,
           0.45f, -0.45f,
          -0.45f, -0.45f,
        };

        GL::ShaderProgram* shader = m_Shaders.at("geometry").get();
        GL_CHECK(glUseProgram(shader->get()));

        // Setup rendering
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, points);

        // Render
        glDrawArrays(GL_POINTS, 0, 4);

        // Cleanup
        glDisableVertexAttribArray(0);
        GL_CHECK(glUseProgram(0));
    }*/

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

/*        GL_CHECK(glUniform4f(shaderProgram->getUniformLocation("color"), 1, 1, 1, 0.75f));
        m_Fonts.at("generic")->drawText(2, viewport[3] - 32-2, stringf("X:%.15f Y:%.15f Z:%.3f C:%.15f (%.15f, %.15f)",
            m_Viewport.position[0], m_Viewport.position[1], m_Viewport.position[2], m_Viewport.position[3], m_Viewport.position[4], m_Viewport.position[5]));
*/
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

