#include "first_app.h"
#include "lve_camera.h"
#include "keyboard_movement_controller.h"
#include "lve_buffer.h"
#include "point_light_system.h"
#include "ModelingData.h"
//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
//std
#include <stdexcept>
#include <array>
#include <chrono>
#include <numeric>
#define FMT_HEADER_ONLY 
#include <format.h>
const float MAX_FRAME_TIME = 1;
static VkRenderPass renderPass;
namespace lve
{
    //check alignment rules whenever making changes to buffer object structures
    struct GlobalUbo
    {
        glm::mat4 projection{1.f};
        glm::mat4 view{1.f};
        glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f};
        glm::vec3 lightPosition{-1.f};
        alignas(16) glm::vec4 lightColor{1.f};//w is light intensity
    };

	FirstApp::FirstApp() 
    { 
        globalPool = LveDescriptorPool::Builder(lveDevice)
            .setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
        loadGameObjects();
    }

	FirstApp::~FirstApp() {}

	void FirstApp::run()
	{
        //LveBuffer globalUboBuffer{ lveDevice, sizeof(GlobalUbo), LveSwapChain::MAX_FRAMES_IN_FLIGHT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, lveDevice.properties.limits.minUniformBufferOffsetAlignment, };
        //globalUboBuffer.map();
        std::vector < std::unique_ptr<LveBuffer> > uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); ++i)
        {
            uboBuffers[i] = std::make_unique<LveBuffer>(lveDevice, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();
        }

        auto globalSetLayout = LveDescriptorSetLayout::Builder(lveDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); ++i)
        {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            LveDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

		SimpleRenderSystem simpleRenderSystem{ lveDevice, lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        PointLightSystem pointLightSystem{ lveDevice, lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
        LveCamera camera;
        init_imgui(lveWindow.getGLFWwindow(), VK_FORMAT_B8G8R8A8_UNORM, globalPool->getDescriptorPool());
        auto viewerObject = LveGameObject::createGameObject();
        viewerObject.transform.translation.z = -2.5f;
        KeyboardMovementController cameraController{};
        cameraController.initMouse(lveWindow.getGLFWwindow());

        auto currentTime = std::chrono::high_resolution_clock::now();
       

        while (!lveWindow.shouldClose())
		{
			glfwPollEvents();

           
            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;
            //to avoid auto move when changing window size
            frameTime = glm::min(frameTime, MAX_FRAME_TIME);
            //keyboard
            cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), frameTime, viewerObject);
            //mouse
            if(cameraController.state)
                cameraController.ProcessMouseMovement(lveWindow.getGLFWwindow(), frameTime, viewerObject);
            //camera
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = lveRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);
            //beginframe will renturn a nullptr if the swap chain needs to be recreated
			if (auto commandBuffer = lveRenderer.beginFrame())
			{
                int frameIndex = lveRenderer.getFrameIndex();
                FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex], gameObjects};
				//begin offscreen shadow pass
				//render shadow casting objects
				//end offscreen shadow pass
                //update
                GlobalUbo ubo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();
                //render
				lveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo);
                pointLightSystem.render(frameInfo);
				lveRenderer.endSwapChainRenderPass(commandBuffer);
				lveRenderer.endFrame();
			}
            draw_imgui();
		}

		vkDeviceWaitIdle(lveDevice.device());
	}
    
    std::unique_ptr<LveModel> createCubeModel(LveDevice& device, glm::vec3 offset) {
        modelingData sample;
        std::vector<float> v = sample.printv();
        int vertices_count = v.size();
        LveModel::Builder modelBuilder{};
        for (size_t i = 0; i < vertices_count; i += 6)
        {
            LveModel::Vertex temp{ glm::vec3(v[i], v[i + 1], v[i + 2]), glm::vec3(v[i + 3], v[i + 4], v[i + 5]) };
            modelBuilder.vertices.push_back(temp);
        }
        for (auto& v : modelBuilder.vertices) {
            v.position += offset;
        }

        /*       modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                                       12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };*/

        return std::make_unique<LveModel>(device, modelBuilder);
    }
	void FirstApp::loadGameObjects()
	{
        gameObjects.clear();
        std::shared_ptr<LveModel> lveModel = createCubeModel(lveDevice, { .0f, .0f, .0f });
        auto cube = LveGameObject::createGameObject();
        cube.model = lveModel;
        cube.transform.translation = { 0.f, -0.5f, 0.f };
        cube.transform.scale = { 1.f, 1.f, 1.f };
        gameObjects.emplace(cube.getId(), std::move(cube));

        lveModel = LveModel::createModelFromFile(lveDevice, "quad.obj");
        auto floor = LveGameObject::createGameObject();
        floor.model = lveModel;
        floor.transform.translation = { 0.f, .5f, 0.f };
        floor.transform.scale = { 2.5f, 2.5f, 2.5f };
        gameObjects.emplace(floor.getId(), std::move(floor));
	}
}

void check_vk_result(VkResult err)
{
    if (err == 0)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}

void lve::FirstApp::init_imgui(GLFWwindow* window, VkFormat format, VkDescriptorPool descriptorPool)
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorReference{};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;

    VkSubpassDependency supassDependency{};
    supassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    supassDependency.dstSubpass = 0;
    supassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    supassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    supassDependency.srcAccessMask = 0;
    supassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &colorAttachment;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &supassDependency;

    auto&& device = lveDevice.device();

    vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass);

    //这里使用了imgui的一个分支docking
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;        // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platfor

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = lveDevice.getVKInstance();
    init_info.PhysicalDevice = lveDevice.getPhysicalDevice();
    init_info.Device = lveDevice.device();
    init_info.QueueFamily = lveDevice.findPhysicalQueueFamilies().graphicsFamily;//device.queueFamilyIndices.graphicsQueue.value();
    init_info.Queue = lveDevice.graphicsQueue(); // device.getGraphicsQueue();
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = descriptorPool;
    init_info.Subpass = 0;
    init_info.MinImageCount = 2;
    init_info.ImageCount = 2;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = VK_NULL_HANDLE;
    init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info, renderPass);
    // Upload Fonts
    {
        // Use any command queue
        lveDevice.excuteCmd(ImGui_ImplVulkan_CreateFontsTexture);
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

}

void lve::FirstApp::draw_imgui()
{
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    static bool show_demo_window = false;
    static bool show_another_window = false;
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
    {
        if (show_demo_window)
        {
            ImGui::ShowDemoWindow(&show_demo_window);
        }
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        {
            static float f1 = 1.0f, f2 = 1.0f;

            ImGui::Begin("controller");                          // Create a window and append into it.

            ImGui::Text("click to use");                        // Display some text (you can use a format strings too)

            //set to scale obj
            static const TransformComponent modelValue1 = gameObjects.begin()->second.transform;
            if (ImGui::SliderFloat("Obj.scale", &f1, 0.0f, 1.0f))
            {
                gameObjects.begin()->second.transform.scale = modelValue1.scale * f1;
            }
            //rotation set
            if (ImGui::TreeNode("rotation"))
            {
                if (ImGui::Button("Obj rotate x"))
                    ++gameObjects.begin()->second.transform.rotation.x;
                if (ImGui::Button("Obj rotate y"))
                    ++gameObjects.begin()->second.transform.rotation.z;
                if (ImGui::Button("Obj rotate z"))
                    ++gameObjects.begin()->second.transform.rotation.y;
                ImGui::Text("Obj Rotation Values:");
                static float inputValues[3] = { 0.0f, 0.0f, 0.0f };
                for (int i = 0; i < IM_ARRAYSIZE(inputValues); i++) {
                    if (ImGui::InputFloat(fmt::format("##InputValue{}", i).c_str(), &inputValues[i])) {
                        // 如果用户输入了数值，则更新模型的值
                        gameObjects.begin()->second.transform.rotation[i] = inputValues[i];
                    }
                }
                ImGui::TreePop();
            }
            
            //transform
            if (ImGui::TreeNode("transform")) 
            {
                ImGui::Text("Obj Position Values:");
                static float inputValues[3] = { 0.0f, 0.0f, 0.0f };
                for (int i = 0; i < IM_ARRAYSIZE(inputValues); i++) {
                    if (ImGui::InputFloat(fmt::format("##InputValue{}", i).c_str(), &inputValues[i])) {
                        // 如果用户输入了数值，则更新模型的值
                        gameObjects.begin()->second.transform.translation[i] = inputValues[i];
                    }
                }
                ImGui::TreePop();
            }
            
            //reset
            if (ImGui::Button("reset Obj"))
                gameObjects.begin()->second.transform = modelValue1;

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }
    }
    ImGui::Render();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}
