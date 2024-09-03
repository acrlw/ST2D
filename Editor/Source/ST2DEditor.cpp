#include "ST2DEditor.h"

#include "imgui-SFML.h"
#include "RenderSFMLImpl.h"


namespace STEditor
{
	ST2DEditor::ST2DEditor()
	{
		m_sceneList = {
			{
				[&](const SceneSettings& settings)
				{ return std::make_unique<HelloWorldScene>(settings); },
				[&](const SceneSettings& settings)
				{ return std::make_unique<CurveScene>(settings); },
				[&](const SceneSettings& settings)
				{ return std::make_unique<BroadphaseScene>(settings); },
				[&](const SceneSettings& settings)
				{ return std::make_unique<NarrowphaseScene>(settings); },
				[&](const SceneSettings& settings)
				{ return std::make_unique<EmptyScene>(settings); },
				[&](const SceneSettings& settings)
				{ return std::make_unique<SplineScene>(settings); },
				[&](const SceneSettings& settings)
				{ return std::make_unique<SpiralScene>(settings); }
			}
		};

		m_camera.setViewport(Viewport(Vector2(0, 0), Vector2(1920, 1080)));

		switchScene(m_currentSceneIndex);
	}

	void ST2DEditor::exec()
	{
		glfwInit();

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(1920, 1080, "Testbed", NULL, NULL);
		if (m_window == NULL)
		{
			APP_ERROR("Failed to create GLFW window");
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(m_window);
		glfwSwapInterval(1); // Enable vsync

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			APP_ERROR("Failed to initialize GLAD");
			return;
		}
		glViewport(0, 0, 1920, 1080);
		//set frame buffer size callback and use type trait to adapt

		glfwSetWindowUserPointer(m_window, this);
		glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
			{
				if (auto app = static_cast<ST2DEditor*>(glfwGetWindowUserPointer(window)))
					app->onFrameBufferResize(window, width, height);
			});
		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos)
			{
				if (auto app = static_cast<ST2DEditor*>(glfwGetWindowUserPointer(window)))
					app->onMouseMoved(window, xpos, ypos);
			});
		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				if (auto app = static_cast<ST2DEditor*>(glfwGetWindowUserPointer(window)))
					app->onMouseScroll(window, xoffset, yoffset);
			});
		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
			{
				if (auto app = static_cast<ST2DEditor*>(glfwGetWindowUserPointer(window)))
					app->onMouseButton(window, button, action, mods);
			});
		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				if (auto app = static_cast<ST2DEditor*>(glfwGetWindowUserPointer(window)))
					app->onKeyButton(window, key, scancode, action, mods);
			});



		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		io.Fonts->AddFontFromFileTTF("./Resource/Fonts/MiSans-Medium.ttf", 18);
		// Setup Dear ImGui style
		styleUI();

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(m_window, true);

		ImGui_ImplOpenGL3_Init("#version 330");

		m_renderer2D = std::make_unique<Renderer2D>();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		glDepthFunc(GL_LEQUAL);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		while (!glfwWindowShouldClose(m_window))
		{
			// polling and handling events
			glfwPollEvents();

			double currentTime = glfwGetTime();
			double deltaTime = currentTime - m_previousTime;
			m_previousTime = currentTime;

			onUpdate(deltaTime);

			// clear

			glClearColor(0.16f, 0.16f, 0.16f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//render
			onRender();

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			onRenderUI();

			m_renderer2D->flush();

			// ImGUI Rendering
			ImGui::Render();

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(m_window);
		}

		onDestroy();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwTerminate();

		//// create the window
		//sf::ContextSettings settings;
		//settings.antialiasingLevel = 8;
		//m_renderWindow = std::make_unique<sf::RenderWindow>(sf::VideoMode(1920, 1080), "Testbed", sf::Style::Default,
		//	settings);
		//bool success = ImGui::SFML::Init(*m_renderWindow);
		//APP_ASSERT(success, "Failed to create window")

		//m_renderWindow->setActive(false);
		//m_renderWindow->setFramerateLimit(120);

		//auto& io = ImGui::GetIO();
		//io.Fonts->AddFontFromFileTTF("Resource/Fonts/MiSans-Medium.ttf", 18.0f);
		//io.FontDefault = io.Fonts->Fonts[1];
		//success = ImGui::SFML::UpdateFontTexture();
		//APP_ASSERT(success, "Failed to create font asset")

		//if (!m_font.loadFromFile("Resource/Fonts/MiSans-Medium.ttf"))
		//{
		//	APP_ERROR("Cannot load font for SFML")
		//	return;
		//}

		//m_renderWindow->setVerticalSyncEnabled(true);

		//m_camera.setFont(&m_font);

		//styleUI();

		//sf::Clock deltaClock;
		//while (m_renderWindow->isOpen())
		//{
		//	sf::Time elapsed = deltaClock.restart();
		//	float deltaTime = elapsed.asSeconds();

		//	sf::Event event{};
		//	while (m_renderWindow->pollEvent(event))
		//	{
		//		ImGui::SFML::ProcessEvent(event);

		//		switch (event.type)
		//		{
		//		case sf::Event::Closed:
		//		{
		//			onClosed(event);
		//			break;
		//		}
		//		case sf::Event::KeyReleased:
		//		{
		//			onKeyReleased(event);
		//			break;
		//		}
		//		case sf::Event::MouseButtonPressed:
		//		{
		//			onMousePressed(event);
		//			break;
		//		}
		//		case sf::Event::MouseButtonReleased:
		//		{
		//			onMouseReleased(event);
		//			break;
		//		}
		//		case sf::Event::MouseMoved:
		//		{
		//			onMouseMoved(event);
		//			break;
		//		}
		//		case sf::Event::MouseWheelScrolled:
		//		{
		//			onWheelScrolled(event);
		//			break;
		//		}
		//		case sf::Event::Resized:
		//		{
		//			onResized(event);
		//			break;
		//		}
		//		case sf::Event::KeyPressed:
		//		{
		//			onKeyPressed(event);
		//			break;
		//		}
		//		default:
		//			break;
		//		}
		//	}

		//	onUpdate(deltaTime);
		//	m_camera.onUpdate(deltaTime);

		//	//draw background
		//	m_renderWindow->clear(sf::Color(40, 40, 40));

		//	m_camera.onDraw(*m_renderWindow);

		//	const bool show = m_currentScene != nullptr && m_userDrawVisible;

		//	if (show)
		//		m_currentScene->onDraw(*m_renderWindow);

		//	render(*m_renderWindow);

		//	renderGUI(*m_renderWindow, deltaClock);

		//	m_renderWindow->display();
		//}
		//ImGui::SFML::Shutdown();
	}

	void ST2DEditor::styleUI()
	{
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
		colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(8.00f, 8.00f);
		style.FramePadding = ImVec2(5.00f, 2.00f);
		style.CellPadding = ImVec2(6.00f, 6.00f);
		style.ItemSpacing = ImVec2(6.00f, 6.00f);
		style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
		style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
		style.IndentSpacing = 25;
		style.ScrollbarSize = 15;
		style.GrabMinSize = 10;
		style.WindowBorderSize = 1;
		style.ChildBorderSize = 1;
		style.PopupBorderSize = 1;
		style.FrameBorderSize = 1;
		style.TabBorderSize = 1;
		style.WindowRounding = 7;
		style.ChildRounding = 4;
		style.FrameRounding = 3;
		style.PopupRounding = 4;
		style.ScrollbarRounding = 9;
		style.GrabRounding = 3;
		style.LogSliderDeadzone = 4;
		style.TabRounding = 4;

	}

	//void ST2DEditor::onResized(sf::Event& event)
	//{
	//	Viewport viewport = m_camera.viewport();
	//	viewport.set(static_cast<real>(event.size.width), static_cast<real>(event.size.height));
	//	m_camera.setViewport(viewport);
	//	m_renderWindow->setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
	//}

	//void ST2DEditor::onClosed(sf::Event& event)
	//{
	//	m_renderWindow->close();
	//}

	//void ST2DEditor::onKeyReleased(sf::Event& event)
	//{
	//	//combo
	//	if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::LControl && m_onDistanceCheck)
	//	{
	//		m_onDistanceCheck = false;
	//		m_mouseArray[0].clear();
	//		m_mouseArray[1].clear();
	//	}

	//	if (m_currentScene != nullptr)
	//		m_currentScene->onKeyRelease(event);
	//}

	//void ST2DEditor::onKeyPressed(sf::Event& event)
	//{
	//	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) &&
	//		m_enableDistanceCheck)
	//		m_onDistanceCheck = true;

	//	if (m_currentScene != nullptr)
	//		m_currentScene->onKeyPressed(event);
	//}

	//void ST2DEditor::onMouseReleased(sf::Event& event)
	//{
	//	Vector2 pos(event.mouseButton.x, event.mouseButton.y);
	//	m_mousePos = m_camera.screenToWorld(pos);
	//	m_screenMousePos = pos;

	//	if (m_currentScene != nullptr)
	//		m_currentScene->onMouseRelease(event);

	//	m_cameraViewportMovement = false;
	//}

	//void ST2DEditor::onMouseMoved(sf::Event& event)
	//{
	//	Vector2 pos(event.mouseMove.x, event.mouseMove.y);
	//	m_screenMousePos = pos;

	//	Vector2 cameraPos = m_camera.screenToWorld(pos);

	//	Vector2 tf = cameraPos - m_mousePos;
	//	if (m_cameraViewportMovement)
	//	{
	//		tf *= m_camera.meterToPixel();
	//		m_camera.setTransform(m_camera.transform() + tf);
	//		cameraPos = m_camera.screenToWorld(pos);
	//	}
	//	m_mousePos = cameraPos;

	//	if (m_onDistanceCheck)
	//	{
	//		if (m_mouseArray[0].isOrigin())
	//			m_mouseArray[0] = m_mousePos;
	//		else
	//			m_mouseArray[1] = m_mousePos;
	//	}

	//	if (m_currentScene != nullptr)
	//		m_currentScene->onMouseMove(event);
	//}

	//void ST2DEditor::onMousePressed(sf::Event& event)
	//{
	//	Vector2 pos(event.mouseButton.x, event.mouseButton.y);
	//	m_screenMousePos = pos;
	//	m_mousePos = m_camera.screenToWorld(pos);

	//	if (event.mouseButton.button == sf::Mouse::Right)
	//		m_cameraViewportMovement = true;

	//	if (m_currentScene != nullptr)
	//		m_currentScene->onMousePress(event);
	//}

	//void ST2DEditor::onWheelScrolled(sf::Event& event)
	//{
	//	auto x = event.mouseWheelScroll.x;
	//	auto y = event.mouseWheelScroll.y;
	//	m_camera.setPreScrollScreenMousePos(Vector2(static_cast<float>(x), static_cast<float>(y)));
	//	if (event.mouseWheelScroll.delta > 0)
	//		m_camera.setMeterToPixel(m_camera.meterToPixel() + m_camera.meterToPixel() * m_zoomFactor);
	//	else
	//		m_camera.setMeterToPixel(m_camera.meterToPixel() - m_camera.meterToPixel() * m_zoomFactor);
	//	
	//}

	void ST2DEditor::onFrameBufferResize(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);

		if (m_currentScene != nullptr)
			m_currentScene->onFrameBufferResize(width, height);
	}

	void ST2DEditor::onKeyButton(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (m_currentScene != nullptr)
			m_currentScene->onKeyButton(key, scancode, action, mods);
	}

	void ST2DEditor::onMouseButton(GLFWwindow* window, int button, int action, int mods)
	{
		if (m_currentScene != nullptr)
			m_currentScene->onMouseButton(button, action, mods);
	}

	void ST2DEditor::onMouseMoved(GLFWwindow* window, double xpos, double ypos)
	{
		if (m_currentScene != nullptr)
			m_currentScene->onMouseMoved(xpos, ypos);
	}

	void ST2DEditor::onMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (m_currentScene != nullptr)
			m_currentScene->onMouseScroll(xoffset, yoffset);
	}

	//void ST2DEditor::render(sf::RenderWindow& window)
	//{
	//	if (m_onDistanceCheck)
	//	{
	//		if (m_mouseArray[0].isOrigin() || m_mouseArray[1].isOrigin())
	//			return;
	//		Vector2 v = m_mouseArray[1] - m_mouseArray[0];
	//		real length = v.length();
	//		if (realEqual(length, 0))
	//			return;
	//		Vector2 normal = v / length;
	//		RenderSFMLImpl::renderPoint(window, m_camera, m_mouseArray[0], RenderConstant::Green);
	//		RenderSFMLImpl::renderPoint(window, m_camera, m_mouseArray[1], RenderConstant::Green);
	//		RenderSFMLImpl::renderLine(window, m_camera, m_mouseArray[0], m_mouseArray[1], RenderConstant::Green);
	//		std::string str = std::format("{:.7f}", length);
	//		sf::Text text;
	//		text.setFont(m_font);
	//		text.setString(str);
	//		text.setCharacterSize(16);
	//		text.setFillColor(RenderConstant::Green);
	//		sf::FloatRect text_rect = text.getLocalBounds();
	//		text.setOrigin(text_rect.left + text_rect.width / 2.0f, text_rect.top + text_rect.height / 2.0f);
	//		const Vector2 t = normal.perpendicular();
	//		Vector2 half(text_rect.width / 2.0f, text_rect.height / 2.0f);
	//		half *= m_camera.pixelToMeter();
	//		Vector2 offset = t * half.x * 1.5f - normal * half.y * 1.5f;
	//		text.setPosition(RenderSFMLImpl::toVector2f(m_camera.worldToScreen(m_mouseArray[1] + offset)));
	//		text.rotate(Math::degree(-t.theta()));
	//		window.draw(text);
	//	}
	//}

	void ST2DEditor::onRenderUI()
	{
		ImGui::SetWindowPos("Panel", ImVec2(0, 0));

		ImGui::Begin("Panel", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 5.0f;
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.55f);

		ImGui::SeparatorText("Scenes");

		int oldItem = m_currentSceneIndex;
		ImGui::Combo("Current Scene", &m_currentSceneIndex, m_sceneName.data(), m_sceneName.size());
		if (oldItem != m_currentSceneIndex)
			switchScene(m_currentSceneIndex);

		ImGui::SeparatorText("Camera");
		ImGui::Columns(2, nullptr);

		ImGui::Checkbox("Grid Lines", &m_camera.gridScaleLineVisible());
		ImGui::Checkbox("Show Numbers", &m_camera.coordinateScale());
		ImGui::Checkbox("User Draw", &m_userDrawVisible);
		ImGui::NextColumn();
		ImGui::Checkbox("Smooth Zooming", &m_camera.smoothZoom());
		ImGui::SliderFloat("Zoom", &m_zoomFactor, 0.1f, 0.8f, "%.1f");
		ImGui::Checkbox("Distance Check", &m_enableDistanceCheck);

		ImGui::NextColumn();
		ImGui::Columns(1, nullptr);

		ImGui::SeparatorText("Buttons");

		if (ImGui::Button("Restart", ImVec2(-FLT_MIN, 0.0f)))
			restart();

		ImGui::SeparatorText("Help");


		ImGui::Text("Press LCtrl+D to check distance.");
		ImGui::Text("Hold mouse right to move camera.");
		ImGui::Text("Scroll down to zoom.");

		ImGui::End();


		if (m_currentScene != nullptr && m_userDrawVisible)
			m_currentScene->onRenderUI();
	}

	void ST2DEditor::onRender()
	{
		m_renderer2D->onRenderStart();
		m_referenceLayer.onRender(m_renderer2D.get());
		
		if (m_currentScene != nullptr)
			m_currentScene->onRender(m_window, m_renderer2D.get());

		m_renderer2D->onRenderEnd();
		
	}

	void ST2DEditor::restart()
	{
		if(m_currentScene != nullptr)
		{
			m_currentScene->onUnLoad();
			m_currentScene.reset();
			switchScene(m_currentSceneIndex);
		}
	}

	void ST2DEditor::onUpdate(float deltaTime) const
	{
		if (m_currentScene != nullptr)
			m_currentScene->onUpdate(deltaTime);
	}

	void ST2DEditor::switchScene(int index)
	{
		clearAll();

		SceneSettings settings;
		settings.camera = &m_camera;
		settings.font = &m_font;

		m_currentScene = m_sceneList[index](settings);
		m_currentScene->onLoad();

	}
	void ST2DEditor::clearAll()
	{
		if (m_currentScene != nullptr)
		{
			m_currentScene->onUnLoad();
			m_currentScene.reset();
		}
	}

	void ST2DEditor::onDestroy()
	{
		clearAll();
		m_renderer2D.reset();
	}
}
