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
	}

	void ST2DEditor::exec()
	{
		glfwInit();

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 8);
		m_window = glfwCreateWindow(1920, 1080, "Testbed", NULL, NULL);
		if (m_window == nullptr)
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

		switchScene(m_currentSceneIndex);

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

			onUpdate(static_cast<float>(deltaTime));

			// clear

			glClearColor(0.16f, 0.16f, 0.16f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//render
			onRender();

			m_renderer2D->onRender();

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			onRenderUI();

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

	void ST2DEditor::onFrameBufferResize(GLFWwindow* window, int width, int height)
	{
		if (m_renderer2D != nullptr)
			m_renderer2D->onFrameBufferResize(window, width, height);

		if (m_currentScene != nullptr)
			m_currentScene->onFrameBufferResize(window, width, height);
	}

	void ST2DEditor::onKeyButton(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (m_renderer2D != nullptr)
			m_renderer2D->onKeyButton(window, key, scancode, action, mods);

		if (m_currentScene != nullptr)
			m_currentScene->onKeyButton(window, key, scancode, action, mods);
	}

	void ST2DEditor::onMouseButton(GLFWwindow* window, int button, int action, int mods)
	{
		if (m_renderer2D != nullptr)
			m_renderer2D->onMouseButton(window, button, action, mods);

		if (m_currentScene != nullptr)
			m_currentScene->onMouseButton(window, button, action, mods);
	}

	void ST2DEditor::onMouseMoved(GLFWwindow* window, double xpos, double ypos)
	{
		if (m_renderer2D != nullptr)
			m_renderer2D->onMouseMoved(window, xpos, ypos);

		if (m_currentScene != nullptr)
			m_currentScene->onMouseMoved(window, xpos, ypos);
	}

	void ST2DEditor::onMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (m_renderer2D != nullptr)
			m_renderer2D->onMouseScroll(window, xoffset, yoffset);

		if (m_currentScene != nullptr)
			m_currentScene->onMouseScroll(window, xoffset, yoffset);
	}

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
		m_referenceLayer.onRender(*m_renderer2D);
		
		if (m_currentScene != nullptr)
			m_currentScene->onRender(*m_renderer2D);
		
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
		settings.renderer = m_renderer2D.get();

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
