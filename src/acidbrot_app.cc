#include <utils/stringf.hh>

#include "acidbrot_app.hh"

#include <utils/savepng.hh>

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

void AcidbrotApp::_keyCallback (GLFWwindow* a_Window,
                                int a_Key, 
                                int a_Scancode, 
                                int a_Action, 
                                int a_Mods)
{
    // Retreive window's user pointer.
    AcidbrotApp* app = (AcidbrotApp*)glfwGetWindowUserPointer(a_Window);
    if (app == nullptr) {
        return;
    }

    // Invoke the member method
    app->keyCallback(a_Window, a_Key, a_Scancode, a_Action, a_Mods);
}

// ============================================================================

int AcidbrotApp::initialize () {
    m_Logger->info("Initializing app...");

    // Hints
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_DOUBLEBUFFER, true);

    glfwWindowHint(GLFW_RESIZABLE, false);

    // Create the window
    m_Window = glfwCreateWindow(Resolutions[0][0], Resolutions[0][1], 
                                "Acid-brot", nullptr, nullptr);

    if (m_Window == nullptr) {
        m_Logger->error("Error creating window!");
        return -1;
    }

    center(m_Window, getBestMonitor(m_Window));

    addWindow(m_Window);
    glfwSetKeyCallback(m_Window, AcidbrotApp::_keyCallback);

    // Set OpenGL context
    glfwMakeContextCurrent(m_Window);    
    glfwSwapInterval(1);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        m_Logger->error("gladLoadGL() Failed!");
        return -1;
    }

    m_Logger->info("OpenGL v{}.{}", GLVersion.major, GLVersion.minor);

    // Print OpenGL renderer info
    GL::dumpRendererInfo();
    // Print OpenGL capabilities
    GL::dumpRendererCaps();
    
    m_HaveFp64 = GL::isExtensionAvailable("GL_ARB_gpu_shader_fp64");
    m_Logger->info("m_HaveFp64 {}", m_HaveFp64);

    // ..........................................

    m_Fonts["generic"] = std::unique_ptr<GL::Font>(new GL::Font("media/fonts/Roboto-Regular.ttf"));

    // ..........................................

    std::string mandelbrotShader = (m_HaveFp64) ? "shaders/mandelbrot64.fsh" :
                                                  "shaders/mandelbrot32.fsh";

    GL::Shader vshGeneric      ("shaders/generic2d.vsh", GL_VERTEX_SHADER);
    GL::Shader fshMandelbrot   (mandelbrotShader,        GL_FRAGMENT_SHADER, {{"MANDELBROT", "1"}});
    GL::Shader fshJulia        (mandelbrotShader,        GL_FRAGMENT_SHADER, {{"JULIA", "1"}});
    GL::Shader fshColorizer    ("shaders/colorizer.fsh", GL_FRAGMENT_SHADER);
    GL::Shader fshDespeckle    ("shaders/despeckle.fsh", GL_FRAGMENT_SHADER);
    GL::Shader fshFir3x3Abs    ("shaders/edges.fsh",     GL_FRAGMENT_SHADER, {{"TAPS", "9"}});
    GL::Shader fshHalo         ("shaders/halo.fsh",      GL_FRAGMENT_SHADER);

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

    m_Shaders["edges"]      = std::unique_ptr<GL::ShaderProgram>(new GL::ShaderProgram(
        vshGeneric,
        fshFir3x3Abs,
        "edges"
        ));

    m_Shaders["halo"]       = std::unique_ptr<GL::ShaderProgram>(new GL::ShaderProgram(
        vshGeneric,
        fshHalo,
        "halo"
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

    auto addParameter = [&](const Parameter& a_Parameter) {
        m_Parameters.insert({a_Parameter.name, a_Parameter});
    };

    addParameter(Parameter("fractalIter", true,  256.0,  10.0, 512.0, 100.000));
    addParameter(Parameter("colorGamma",  true,  2.0000, 1.0,  5.0, 1.000));
    addParameter(Parameter("colorCycles", true,  4.0000, 1.0,  6.0, 1.000));
    addParameter(Parameter("haloStepFac", true,  0.9875, 0.5,  1.0, 0.025));
    addParameter(Parameter("haloAttnFac", true,  0.9250, 0.5,  1.0, 0.100));
    addParameter(Parameter("haloGain",    true,  1.0000, 0.5,  5.0, 1.000));
    addParameter(Parameter("motionBlur",  false, 0.8500, 0.05, 1.0, 0.500));

    m_CurrParam = m_Parameters.end();

    // ..........................................

    FilterMask* mask = nullptr;

    mask = new FilterMask(3, 3);
    mask->w(0, 0) = 0.25f;
    mask->w(0, 1) = 1.00f;
    mask->w(0, 2) = 0.25f;
    mask->w(1, 0) = 1.00f;
    mask->w(1, 1) = 1.00f;
    mask->w(1, 2) = 1.00f;
    mask->w(2, 0) = 0.25f;
    mask->w(2, 1) = 1.00f;
    mask->w(2, 2) = 0.25f;
    mask->normalizeWeights();
    m_Masks["despeckle"].reset(mask);

    mask = new FilterMask(3, 3);
    mask->w(0, 0) = -0.5f;
    mask->w(0, 1) = -1.0f;
    mask->w(0, 2) = -0.5f;
    mask->w(1, 0) = -1.0f;
    mask->w(1, 1) = +6.0f;
    mask->w(1, 2) = -1.0f;
    mask->w(2, 0) = -0.5f;
    mask->w(2, 1) = -1.0f;
    mask->w(2, 2) = -0.5f;
    mask->normalizeWeights();
    m_Masks["edges"].reset(mask);

    // ..........................................

    m_Textures["colormap"]  = std::unique_ptr<GL::Texture>(
        new GL::Texture("media/colormap.png", GL_LINEAR, GL_MIRRORED_REPEAT)
    );

    // ..........................................

    int res = initializeFramebuffers();
    if (res) {
        return res;
    }

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

int AcidbrotApp::initializeFramebuffers () {

    // Get the main framebuffer size
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(m_Window, &fbWidth, &fbHeight);
    //glfwGetWindowSize(m_Window, &fbWidth, &fbHeight);

    m_Logger->info("Framebuffer size ({}, {})", fbWidth, fbHeight);

    // ..........................................

    m_Framebuffers["fractalRaw"] = std::unique_ptr<GL::Framebuffer>(
        new GL::Framebuffer(fbWidth, fbHeight, GL_RGBA, 1, false)
    );

    m_Framebuffers["fractalFlt"] = std::unique_ptr<GL::Framebuffer>(
        new GL::Framebuffer(fbWidth, fbHeight, GL_RGBA, 1, false)
    );

    m_Framebuffers["fractalEdges"] = std::unique_ptr<GL::Framebuffer>(
        new GL::Framebuffer(fbWidth, fbHeight, GL_RGBA, 1, false)
    );

    m_Framebuffers["fractalColor"] = std::unique_ptr<GL::Framebuffer>(
        new GL::Framebuffer(fbWidth, fbHeight, GL_RGBA, 1, false)
    );

    m_Framebuffers["master"] = std::unique_ptr<GL::Framebuffer>(
        new GL::Framebuffer(fbWidth, fbHeight, GL_RGBA, 1, false)
    );

    // ..........................................

    for (auto& pair : m_Masks) {
        pair.second->computeOffsets(fbWidth, fbHeight);
    }

    return 0;
}

void AcidbrotApp::setUniforms () {

    // Get current shader
    GLint shader;
    GL_CHECK(glGetIntegerv(GL_CURRENT_PROGRAM, &shader));

    // Set uniforms if available
    for (auto& pair : m_Parameters) {

        auto& param = pair.second;
        if (!param.forShader) {
            continue;
        }

        // Get location
        GLint loc = glGetUniformLocation(shader, param.name.c_str());
        if (loc == -1) {
            continue;
        }

        // Set value
        glUniform1f(loc, param.value);
    }
}

// ============================================================================
#define MAX_FILE_INDEX 9999

size_t getNextFileIndex(const std::string& a_Format, size_t a_Begin) {

    auto fileExists = [](const std::string& fileName) {
        struct stat buffer;   
        return (stat(fileName.c_str(), &buffer) == 0);
    };

    // Limit
    if (a_Begin > MAX_FILE_INDEX) {
        a_Begin = MAX_FILE_INDEX;
    }

    // Find a first free file name
    for (size_t i=a_Begin; i<MAX_FILE_INDEX; ++i) {
        std::string fileName = stringf(a_Format, i);

        // File does not exist
        if (!fileExists(fileName)) {
            return i;
        }
    }

    // Maximum number of files reached
    return MAX_FILE_INDEX;
}

void AcidbrotApp::takeScreenshot () {
    const std::string nameFormat = "screenshot_%04d.png";

    // Download the framebuffer
    GL::Framebuffer* fb = m_Framebuffers.at("master").get();
    auto data = fb->readPixels(0);

    // Determine file name
    size_t index = getNextFileIndex(nameFormat, 0);
    std::string fileName = stringf(nameFormat, index);

    // Save it to PNG
    m_Logger->info("Saving screenshot '{}'", fileName);
    savePNG(fileName,
            fb->getWidth(),
            fb->getHeight(),
            data.get(),
            true);
}

// ============================================================================

void AcidbrotApp::keyCallback(GLFWwindow* a_Window,
                           int a_Key, 
                           int a_Scancode, 
                           int a_Action, 
                           int a_Mods) 
{
    (void)a_Scancode;

    // Manually select resolution
    if (!isFullscreen(a_Window) && a_Action == GLFW_PRESS) {
        const std::vector<int> keys = {
            GLFW_KEY_F1,
            GLFW_KEY_F2,
            GLFW_KEY_F3,
            GLFW_KEY_F4,
            GLFW_KEY_F5,
            GLFW_KEY_F6,
            GLFW_KEY_F7,
            GLFW_KEY_F8,
        };

        for (size_t i=0; i<keys.size(); ++i) {
            if (i >= Resolutions.size()) {
                break;
            }

            if (a_Key == keys[i]) {
                glfwSetWindowSize(a_Window, Resolutions[i][0], Resolutions[i][1]);
                center(a_Window, getBestMonitor(a_Window));
                initializeFramebuffers();
                break;
            }
        }
    }

    // Fullscreen / windowed
    if ( a_Key == GLFW_KEY_ENTER && 
        (a_Mods & GLFW_MOD_ALT) && 
         a_Action == GLFW_PRESS)
    {
        setFullscreen(a_Window, !isFullscreen(a_Window));
    }

    // Screenshot
    if (a_Key == GLFW_KEY_F12 && a_Action == GLFW_PRESS) {
        m_DoScreenshot = true;
    }

    // Switch fractal
    if (a_Key == GLFW_KEY_F && a_Action == GLFW_PRESS) {
        if (m_Fractal == Fractal::Mandelbrot) {
            m_Fractal = Fractal::Julia;
        }
        else if (m_Fractal == Fractal::Julia) {
            m_Fractal = Fractal::Mandelbrot;
        }
    }

    // Switch modified parameter
    if (a_Key == GLFW_KEY_HOME && a_Action == GLFW_PRESS) {
        if (m_CurrParam == m_Parameters.end()) {
            m_CurrParam = m_Parameters.begin();
        }
        else {
            m_CurrParam++;
        }
    }

    if (a_Key == GLFW_KEY_END && a_Action == GLFW_PRESS) {
        m_CurrParam--;
    }
}

std::array<float, 2> splitDouble (double x) {
    float a = (float)x;
    float b = x - (double)a;

    return std::array<float, 2>({a, b});
}

std::array<double, 2> splitLongDouble (long double x) {
    double a = (double)x;
    double b = x - (long double)a;

    return std::array<double, 2>({a, b});
}

int AcidbrotApp::loop (double dt) {

    // Escape
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        return 1;
    }

    // Window size changed
    if (sizeChanged(m_Window)) {
        initializeFramebuffers();
    }

    // ................................
    // Shader parameters

    if (m_CurrParam != m_Parameters.end()) {
        auto& pair  = *m_CurrParam;
        auto& param = pair.second;

        if (glfwGetKey(m_Window, GLFW_KEY_PAGE_UP)   == GLFW_PRESS) {
            param.value += dt * param.speed;
            if (param.value > param.max) {
                param.value = param.max;
            }
        }
        if (glfwGetKey(m_Window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
            param.value -= dt * param.speed;
            if (param.value < param.min) {
                param.value = param.min;
            }
        }
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
        control.rotation *= 0.75f;

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

        if (m_Viewport.position.zoom < -2.0f) {
            m_Viewport.position.zoom = -2.0f;
        }

        // No negative abs(C)
        if (m_Viewport.position.julia[0] < 0.0f) {
            m_Viewport.position.julia[0] = 0.0f;
        }
    }

    // ................................
    // Initialize GL rendering

    // Get the main framebuffer size
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(m_Window, &fbWidth, &fbHeight);
    //glfwGetWindowSize(m_Window, &fbWidth, &fbHeight);

    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CHECK(glViewport(0, 0, fbWidth, fbHeight));

    //GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    //GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    // ................................
    // Generate the fractal data
    {
        const std::map<Fractal, std::string> shaderName = {
            {Fractal::Mandelbrot, "mandelbrot"},
            {Fractal::Julia, "julia"}
        };

        GL::Framebuffer* framebuffer = m_Framebuffers.at("fractalRaw").get();
        framebuffer->enable();

        GL::ShaderProgram* shader = m_Shaders.at(shaderName.at(m_Fractal)).get();
        GL_CHECK(glUseProgram(shader->get()));

        float juliaC[2] = {
            (float)m_Viewport.position.julia[0] * cosf(m_Viewport.position.julia[1]),
            (float)m_Viewport.position.julia[0] * sinf(m_Viewport.position.julia[1])
        };

        GL_CHECK(glUniform1i(shader->getUniformLocation("fractalIter"),
                    int(m_Parameters.at("fractalIter").value)
                    ));

        if (m_HaveFp64) {

            GL_CHECK(glUniform2d(shader->getUniformLocation("fractalPosition"),
                        m_Viewport.position.position[0],
                        m_Viewport.position.position[1]
                        ));

            GL_CHECK(glUniform1d(shader->getUniformLocation("fractalScale"),
                        pow(2.0, m_Viewport.position.zoom)
                        ));
        }
        else {

            GL_CHECK(glUniform2f(shader->getUniformLocation("fractalPosition"),
                        m_Viewport.position.position[0],
                        m_Viewport.position.position[1]
                        ));

            GL_CHECK(glUniform1f(shader->getUniformLocation("fractalScale"),
                        pow(2.0, m_Viewport.position.zoom)
                        ));
        }


        GL_CHECK(glUniform1f(shader->getUniformLocation("fractalRotation"),
                    m_Viewport.position.rotation
                    ));

        GL_CHECK(glUniform2f(shader->getUniformLocation("fractalCoeff"),
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

        // Setup
        fbDst->enable();
        GL_CHECK(glUseProgram(shader->get()));

        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, fbSrc->getTexture()));

        GL_CHECK(glUniform1fv(shader->getUniformLocation("filterWeights"), 9, 
                    m_Masks.at("despeckle")->getWeights()
                    ));
        GL_CHECK(glUniform2fv(shader->getUniformLocation("filterOffsets"), 9,
                    m_Masks.at("despeckle")->getOffsets()
                    ));

        // Render
        GL::drawFullscreenRect();

        // Cleanup
        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

        GL_CHECK(glUseProgram(0));
        fbDst->disable();
    }

    // ................................
    // Detect edges in the fractal image
    {
        GL::ShaderProgram* shader = m_Shaders.at("edges").get();
        GL::Framebuffer*   fbSrc  = m_Framebuffers.at("fractalFlt").get();
        GL::Framebuffer*   fbDst  = m_Framebuffers.at("fractalEdges").get();

        // Setup
        fbDst->enable();
        GL_CHECK(glUseProgram(shader->get()));

        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, fbSrc->getTexture()));

        GL_CHECK(glUniform1fv(shader->getUniformLocation("filterWeights"), 9, 
                    m_Masks.at("edges")->getWeights()
                    ));
        GL_CHECK(glUniform2fv(shader->getUniformLocation("filterOffsets"), 9,
                    m_Masks.at("edges")->getOffsets()
                    ));

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
        GL::ShaderProgram* shader = m_Shaders.at("colorizer").get();
        GL::Framebuffer*   fbSrc  = m_Framebuffers.at("fractalFlt").get();
        GL::Framebuffer*   fbDst  = m_Framebuffers.at("fractalColor").get();

        // Setup
        fbDst->enable();
        GL_CHECK(glUseProgram(shader->get()));

        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, fbSrc->getTexture()));
        GL_CHECK(glUniform1i(shader->getUniformLocation("fractal"), 0));

        GL_CHECK(glActiveTexture(GL_TEXTURE1));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_Textures.at("colormap")->get()));
        GL_CHECK(glUniform1i(shader->getUniformLocation("colormap"), 1));

        GL_CHECK(glUniform1f(shader->getUniformLocation("colormapPos"), m_Viewport.position.color));
        setUniforms();

        // Render
        GL::drawFullscreenRect();

        // Cleanup
        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
        GL_CHECK(glActiveTexture(GL_TEXTURE1));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

        GL_CHECK(glUseProgram(0));
        fbDst->disable();
    }

    // ................................
    // Render with halo effct
    {
        GL::ShaderProgram* shader   = m_Shaders.at("halo").get();
        GL::Framebuffer*   fbColor  = m_Framebuffers.at("fractalColor").get();
        GL::Framebuffer*   fbMask   = m_Framebuffers.at("fractalEdges").get();
        GL::Framebuffer*   fbMaster = m_Framebuffers.at("master").get();

        // Setup
        GL_CHECK(glUseProgram(shader->get()));

        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, fbColor->getTexture()));
        GL_CHECK(glUniform1i(shader->getUniformLocation("texture"), 0));

        GL_CHECK(glActiveTexture(GL_TEXTURE1));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, fbMask->getTexture()));
        GL_CHECK(glUniform1i(shader->getUniformLocation("haloMask"), 1));

        setUniforms();

        fbMaster->enable();

        GL_CHECK(glEnable(GL_BLEND));
        GL_CHECK(glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD));
        GL_CHECK(glBlendFuncSeparate(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA, GL_ONE, GL_ZERO));
        GL_CHECK(glBlendColor(0.0f, 0.0f, 0.0f, m_Parameters.at("motionBlur").value));

        // Render
        GL::drawFullscreenRect();

        // Cleanup
        fbMaster->disable();

        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
        GL_CHECK(glActiveTexture(GL_TEXTURE1));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

        GL_CHECK(glDisable(GL_BLEND));

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
    // Copy the master framebuffer
    {
        GL::Framebuffer* fbMaster = m_Framebuffers.at("master").get();

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbMaster->get());
        glDrawBuffer(GL_BACK);

        glBlitFramebuffer(0, 0, fbWidth, fbHeight,
                          0, 0, fbWidth, fbHeight,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    // Screenshot
    if (m_DoScreenshot) {
        takeScreenshot();
        m_DoScreenshot = false;
    }

    // ................................
    // Text
    {
        GL::ShaderProgram* shaderProgram = m_Shaders.at("font").get();

        GL_CHECK(glEnable(GL_BLEND));
        GL_CHECK(glBlendEquation(GL_FUNC_ADD));
        GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        GL_CHECK(glUseProgram(shaderProgram->get()));

        float viewport[4];
        GL_CHECK(glGetFloatv(GL_VIEWPORT, viewport));
        GL_CHECK(glUniform4fv(shaderProgram->getUniformLocation("viewport"), 1, viewport));

        // Frame rate
        GL_CHECK(glUniform4f(shaderProgram->getUniformLocation("color"), 1, 0, 0, 0.75f));
        m_Fonts.at("generic")->drawText(2, viewport[3] - 16-2, "Frame rate: %.1f FPS", getFrameRate());

        // Parameter
        if (m_CurrParam != m_Parameters.end()) {
            auto& font  = m_Fonts.at("generic");
            auto& pair  = *m_CurrParam;
            auto& param = pair.second;
    
            GL_CHECK(glUniform4f(shaderProgram->getUniformLocation("color"), 1, 1, 1, 0.75f));
            font->drawText(2, viewport[3] - 32-2, stringf(
                           "%s = %.4f", param.name.c_str(), param.value
                          ));
        }

/*        GL_CHECK(glUniform4f(shaderProgram->getUniformLocation("color"), 1, 1, 1, 0.75f));
        m_Fonts.at("generic")->drawText(2, viewport[3] - 32-2, stringf("X:%.15f Y:%.15f Z:%.3f C:%.15f (%.15f, %.15f)",
            m_Viewport.position[0], m_Viewport.position[1], m_Viewport.position[2], m_Viewport.position[3], m_Viewport.position[4], m_Viewport.position[5]));
*/

        GL_CHECK(glDisable(GL_BLEND));

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

