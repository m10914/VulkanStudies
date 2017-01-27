#include <iostream>
#include <vector>
#include <map>

#include <Windows.h>

#define VK_USE_PLATFORM_WIN32_KHR 1

#include "vulkan/vulkan.h"
#pragma comment(lib, "vulkan-1.lib")

void main()
{
	auto handleInstance = GetModuleHandleA(nullptr);
	auto handleWindow = [&]()
	{
		auto windowClassName = "window class";
		{
			WNDCLASSA windowClassInfo;
			{
				windowClassInfo.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
				windowClassInfo.lpfnWndProc = DefWindowProcA;
				windowClassInfo.cbClsExtra = 0;
				windowClassInfo.cbWndExtra = 0;
				windowClassInfo.hInstance = handleInstance;
				windowClassInfo.hIcon = nullptr;
				windowClassInfo.hCursor = nullptr;
				windowClassInfo.hbrBackground = nullptr;
				windowClassInfo.lpszMenuName = nullptr;
				windowClassInfo.lpszClassName = windowClassName;
			}
			if(!RegisterClassA(&windowClassInfo))
			{
				throw std::exception("failed to register window class");
			}
		}

		auto handle = CreateWindowA(
			windowClassName,
			"window",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			0, 0, 800, 600,
			nullptr,
			nullptr,
			handleInstance,
			nullptr
		);

		if(!handle)
		{
			throw std::exception("failed to create window");
		}

		return handle;
	}();

	VkInstance vk_instance;
	{
		std::vector<const char*> layerNames = {
			"VK_LAYER_LUNARG_api_dump",
			"VK_LAYER_LUNARG_core_validation",
			"VK_LAYER_LUNARG_image",
			"VK_LAYER_LUNARG_object_tracker",
			"VK_LAYER_LUNARG_parameter_validation",
			"VK_LAYER_LUNARG_screenshot",
			"VK_LAYER_LUNARG_swapchain",
			"VK_LAYER_GOOGLE_threading",
			"VK_LAYER_GOOGLE_unique_objects",
			// "VK_LAYER_LUNARG_vktrace",
			"VK_LAYER_RENDERDOC_Capture",
			"VK_LAYER_NV_optimus",
			"VK_LAYER_VALVE_steam_overlay",
			"VK_LAYER_LUNARG_standard_validation",
		};

		std::vector<const char*> extensionNames = {
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
		};

		VkApplicationInfo vk_applicationInfo;
		{
			vk_applicationInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO;
			vk_applicationInfo.pNext = nullptr;
			vk_applicationInfo.pApplicationName = "Application name";
			vk_applicationInfo.applicationVersion = 1;
			vk_applicationInfo.pEngineName = "Engine name";
			vk_applicationInfo.engineVersion = 1;
			vk_applicationInfo.apiVersion = VK_MAKE_VERSION(1,0,21);
		}
		VkInstanceCreateInfo vk_instanceCreateInfo;
		{
			vk_instanceCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			vk_instanceCreateInfo.pNext = nullptr;
			vk_instanceCreateInfo.flags = 0;
			vk_instanceCreateInfo.pApplicationInfo = &vk_applicationInfo;
			vk_instanceCreateInfo.enabledLayerCount = layerNames.size();
			vk_instanceCreateInfo.ppEnabledLayerNames = layerNames.data();
			vk_instanceCreateInfo.enabledExtensionCount = extensionNames.size();
			vk_instanceCreateInfo.ppEnabledExtensionNames = extensionNames.data();
		}

		auto result = vkCreateInstance(&vk_instanceCreateInfo, nullptr, &vk_instance);
		if(vkCreateInstance(&vk_instanceCreateInfo, nullptr, &vk_instance) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to create vk instance");
		}

		auto vk_vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(vk_instance, "vkCreateDebugReportCallbackEXT");

		VkDebugReportCallbackEXT vk_debugReportCallbackEXT;
		{
			VkDebugReportCallbackCreateInfoEXT vk_debugReportCallbackCreateInfoEXT;
			{
				vk_debugReportCallbackCreateInfoEXT.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
				vk_debugReportCallbackCreateInfoEXT.pNext = nullptr;
				vk_debugReportCallbackCreateInfoEXT.flags =
					VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
					VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_WARNING_BIT_EXT |
					VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
					VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_ERROR_BIT_EXT |
					VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_DEBUG_BIT_EXT;
				vk_debugReportCallbackCreateInfoEXT.pfnCallback = [](
					VkDebugReportFlagsEXT                       flags,
					VkDebugReportObjectTypeEXT                  objectType,
					uint64_t                                    object,
					size_t                                      location,
					int32_t                                     messageCode,
					const char*                                 pLayerPrefix,
					const char*                                 pMessage,
					void*                                       pUserData) -> VkBool32
				{
					std::cout << "(";
					if((flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_INFORMATION_BIT_EXT) != 0) std::cout << "INFO";
					if((flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_WARNING_BIT_EXT) != 0) std::cout << "WARNING";
					if((flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) != 0) std::cout << "PERFORMANCE";
					if((flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_DEBUG_BIT_EXT) != 0) std::cout << "DEBUG";
					if((flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_ERROR_BIT_EXT) != 0) std::cout << "ERROR";
					std::cout << ")";
					std::cout << "{" << pLayerPrefix << "} " << pMessage << std::endl;
					return VK_FALSE;
				};
				vk_debugReportCallbackCreateInfoEXT.pUserData = nullptr;
			}
			if(vk_vkCreateDebugReportCallbackEXT(vk_instance, &vk_debugReportCallbackCreateInfoEXT, nullptr, &vk_debugReportCallbackEXT) != VkResult::VK_SUCCESS)
			{
				throw std::exception("failed to create debug collback");
			}
		}
	}

	VkPhysicalDevice vk_physicalDevice;
	{
		uint32_t vk_physicalDevicesCount;
		if(vkEnumeratePhysicalDevices(vk_instance, &vk_physicalDevicesCount, nullptr) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to get physical devices count");
		}
		std::vector<VkPhysicalDevice> vk_physicalDevices(vk_physicalDevicesCount);
		if(vkEnumeratePhysicalDevices(vk_instance, &vk_physicalDevicesCount, vk_physicalDevices.data()) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to get physical devices");
		}
		if(vk_physicalDevices.empty())
		{
			throw std::exception("no physical devices");
		}
		vk_physicalDevice = vk_physicalDevices[0];
	}
	std::vector<VkQueueFamilyProperties> vk_physicalDeviceQueueFamilyProperties;
	{
		uint32_t vk_physicalDeviceQueueFamilyPropertiesCount;
		vkGetPhysicalDeviceQueueFamilyProperties(vk_physicalDevice, &vk_physicalDeviceQueueFamilyPropertiesCount, nullptr);

		vk_physicalDeviceQueueFamilyProperties.resize(vk_physicalDeviceQueueFamilyPropertiesCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vk_physicalDevice, &vk_physicalDeviceQueueFamilyPropertiesCount, vk_physicalDeviceQueueFamilyProperties.data());
	}

	VkDevice vk_device;
	{
		std::vector<const char*> layerNames = {
			"VK_LAYER_NV_optimus",
			"VK_LAYER_LUNARG_api_dump",
			"VK_LAYER_LUNARG_screenshot",
			"VK_LAYER_RENDERDOC_Capture",
			"VK_LAYER_VALVE_steam_overlay",
			"VK_LAYER_LUNARG_standard_validation",
		};
		std::vector<const char*> extensionNames = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		};

		std::vector<VkDeviceQueueCreateInfo> vk_deviceQueueCreateInfos(1);
		std::vector<std::vector<float>> vk_deviceQueuesPriorities(vk_deviceQueueCreateInfos.size(), std::vector<float>(1, 0.0f));
		{
			for(size_t i = 0; i < vk_deviceQueueCreateInfos.size(); ++i)
			{
				auto &vk_deviceQueueCreateInfo = vk_deviceQueueCreateInfos[i];
				auto &vk_deviceQueuePriorities = vk_deviceQueuesPriorities[i];

				vk_deviceQueueCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				vk_deviceQueueCreateInfo.pNext = nullptr;
				vk_deviceQueueCreateInfo.flags = 0;
				vk_deviceQueueCreateInfo.queueFamilyIndex = i;
				vk_deviceQueueCreateInfo.queueCount = vk_deviceQueuePriorities.size();
				vk_deviceQueueCreateInfo.pQueuePriorities = vk_deviceQueuePriorities.data();
			}
		}

		VkPhysicalDeviceFeatures vk_physicalDeviceFeatures;
		{
			vk_physicalDeviceFeatures.robustBufferAccess = VK_FALSE;
			vk_physicalDeviceFeatures.fullDrawIndexUint32 = VK_FALSE;
			vk_physicalDeviceFeatures.imageCubeArray = VK_FALSE;
			vk_physicalDeviceFeatures.independentBlend = VK_FALSE;
			vk_physicalDeviceFeatures.geometryShader = VK_FALSE;
			vk_physicalDeviceFeatures.tessellationShader = VK_FALSE;
			vk_physicalDeviceFeatures.sampleRateShading = VK_FALSE;
			vk_physicalDeviceFeatures.dualSrcBlend = VK_FALSE;
			vk_physicalDeviceFeatures.logicOp = VK_FALSE;
			vk_physicalDeviceFeatures.multiDrawIndirect = VK_FALSE;
			vk_physicalDeviceFeatures.drawIndirectFirstInstance = VK_FALSE;
			vk_physicalDeviceFeatures.depthClamp = VK_FALSE;
			vk_physicalDeviceFeatures.depthBiasClamp = VK_FALSE;
			vk_physicalDeviceFeatures.fillModeNonSolid = VK_FALSE;
			vk_physicalDeviceFeatures.depthBounds = VK_FALSE;
			vk_physicalDeviceFeatures.wideLines = VK_FALSE;
			vk_physicalDeviceFeatures.largePoints = VK_FALSE;
			vk_physicalDeviceFeatures.alphaToOne = VK_FALSE;
			vk_physicalDeviceFeatures.multiViewport = VK_FALSE;
			vk_physicalDeviceFeatures.samplerAnisotropy = VK_FALSE;
			vk_physicalDeviceFeatures.textureCompressionETC2 = VK_FALSE;
			vk_physicalDeviceFeatures.textureCompressionASTC_LDR = VK_FALSE;
			vk_physicalDeviceFeatures.textureCompressionBC = VK_FALSE;
			vk_physicalDeviceFeatures.occlusionQueryPrecise = VK_FALSE;
			vk_physicalDeviceFeatures.pipelineStatisticsQuery = VK_FALSE;
			vk_physicalDeviceFeatures.vertexPipelineStoresAndAtomics = VK_FALSE;
			vk_physicalDeviceFeatures.fragmentStoresAndAtomics = VK_FALSE;
			vk_physicalDeviceFeatures.shaderTessellationAndGeometryPointSize = VK_FALSE;
			vk_physicalDeviceFeatures.shaderImageGatherExtended = VK_FALSE;
			vk_physicalDeviceFeatures.shaderStorageImageExtendedFormats = VK_FALSE;
			vk_physicalDeviceFeatures.shaderStorageImageMultisample = VK_FALSE;
			vk_physicalDeviceFeatures.shaderStorageImageReadWithoutFormat = VK_FALSE;
			vk_physicalDeviceFeatures.shaderStorageImageWriteWithoutFormat = VK_FALSE;
			vk_physicalDeviceFeatures.shaderUniformBufferArrayDynamicIndexing = VK_FALSE;
			vk_physicalDeviceFeatures.shaderSampledImageArrayDynamicIndexing = VK_FALSE;
			vk_physicalDeviceFeatures.shaderStorageBufferArrayDynamicIndexing = VK_FALSE;
			vk_physicalDeviceFeatures.shaderStorageImageArrayDynamicIndexing = VK_FALSE;
			vk_physicalDeviceFeatures.shaderClipDistance = VK_FALSE;
			vk_physicalDeviceFeatures.shaderCullDistance = VK_FALSE;
			vk_physicalDeviceFeatures.shaderFloat64 = VK_FALSE;
			vk_physicalDeviceFeatures.shaderInt64 = VK_FALSE;
			vk_physicalDeviceFeatures.shaderInt16 = VK_FALSE;
			vk_physicalDeviceFeatures.shaderResourceResidency = VK_FALSE;
			vk_physicalDeviceFeatures.shaderResourceMinLod = VK_FALSE;
			vk_physicalDeviceFeatures.sparseBinding = VK_FALSE;
			vk_physicalDeviceFeatures.sparseResidencyBuffer = VK_FALSE;
			vk_physicalDeviceFeatures.sparseResidencyImage2D = VK_FALSE;
			vk_physicalDeviceFeatures.sparseResidencyImage3D = VK_FALSE;
			vk_physicalDeviceFeatures.sparseResidency2Samples = VK_FALSE;
			vk_physicalDeviceFeatures.sparseResidency4Samples = VK_FALSE;
			vk_physicalDeviceFeatures.sparseResidency8Samples = VK_FALSE;
			vk_physicalDeviceFeatures.sparseResidency16Samples = VK_FALSE;
			vk_physicalDeviceFeatures.sparseResidencyAliased = VK_FALSE;
			vk_physicalDeviceFeatures.variableMultisampleRate = VK_FALSE;
			vk_physicalDeviceFeatures.inheritedQueries = VK_FALSE;
		}

		VkDeviceCreateInfo vk_deviceCreateInfo;
		{
			vk_deviceCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			vk_deviceCreateInfo.pNext = nullptr;
			vk_deviceCreateInfo.flags = 0;
			vk_deviceCreateInfo.queueCreateInfoCount = vk_deviceQueueCreateInfos.size();
			vk_deviceCreateInfo.pQueueCreateInfos = vk_deviceQueueCreateInfos.data();
			vk_deviceCreateInfo.enabledLayerCount = layerNames.size();
			vk_deviceCreateInfo.ppEnabledLayerNames = layerNames.data();
			vk_deviceCreateInfo.enabledExtensionCount = extensionNames.size();
			vk_deviceCreateInfo.ppEnabledExtensionNames = extensionNames.data();
			vk_deviceCreateInfo.pEnabledFeatures = &vk_physicalDeviceFeatures;
		};

		if(vkCreateDevice(vk_physicalDevice, &vk_deviceCreateInfo, nullptr, &vk_device) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to create device");
		}
	}

	VkSurfaceKHR vk_surfaceKHR;
	{
		VkWin32SurfaceCreateInfoKHR vk_win32SurfaceCreateInfoKHR;
		{
			vk_win32SurfaceCreateInfoKHR.sType = VkStructureType::VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			vk_win32SurfaceCreateInfoKHR.pNext = nullptr;
			vk_win32SurfaceCreateInfoKHR.flags = 0;
			vk_win32SurfaceCreateInfoKHR.hinstance = handleInstance;
			vk_win32SurfaceCreateInfoKHR.hwnd = handleWindow;
		}
		if(vkCreateWin32SurfaceKHR(vk_instance, &vk_win32SurfaceCreateInfoKHR, nullptr, &vk_surfaceKHR) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to create surface");
		}

		VkBool32 isSupported;
		if(vkGetPhysicalDeviceSurfaceSupportKHR(vk_physicalDevice, 0, vk_surfaceKHR, &isSupported) != VkResult::VK_SUCCESS || isSupported == VK_FALSE)
		{
			throw std::exception("surface not supported");
		}
	}
	std::vector<VkSurfaceFormatKHR> vk_surfaceFormats;
	{
		uint32_t vk_surfaceFormatsCount;
		if(vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physicalDevice, vk_surfaceKHR, &vk_surfaceFormatsCount, nullptr) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to get surface formats count");
		}

		vk_surfaceFormats.resize(vk_surfaceFormatsCount);
		if(vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physicalDevice, vk_surfaceKHR, &vk_surfaceFormatsCount, vk_surfaceFormats.data()) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to get surface formats");
		}
	}
	VkSurfaceCapabilitiesKHR vk_surfaceCapabilitiesKHR;
	{
		if(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_physicalDevice, vk_surfaceKHR, &vk_surfaceCapabilitiesKHR) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to get surface capabilities");
		}
	}

	VkSwapchainKHR vk_swapchainKHR;
	{
		VkSwapchainCreateInfoKHR vk_SwapchainCreateInfoKHR;
		{
			vk_SwapchainCreateInfoKHR.sType = VkStructureType::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			vk_SwapchainCreateInfoKHR.pNext = nullptr;
			vk_SwapchainCreateInfoKHR.flags = 0;
			vk_SwapchainCreateInfoKHR.surface = vk_surfaceKHR;
			vk_SwapchainCreateInfoKHR.minImageCount = 2;
			vk_SwapchainCreateInfoKHR.imageFormat = vk_surfaceFormats[0].format;
			vk_SwapchainCreateInfoKHR.imageColorSpace = vk_surfaceFormats[0].colorSpace;
			vk_SwapchainCreateInfoKHR.imageExtent = VkExtent2D{
				vk_surfaceCapabilitiesKHR.currentExtent.width,
				vk_surfaceCapabilitiesKHR.currentExtent.height
			};
			vk_SwapchainCreateInfoKHR.imageArrayLayers = 1;
			vk_SwapchainCreateInfoKHR.imageUsage = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			vk_SwapchainCreateInfoKHR.imageSharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
			vk_SwapchainCreateInfoKHR.queueFamilyIndexCount = 0;
			vk_SwapchainCreateInfoKHR.pQueueFamilyIndices = nullptr;
			vk_SwapchainCreateInfoKHR.preTransform = VkSurfaceTransformFlagBitsKHR::VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
			vk_SwapchainCreateInfoKHR.compositeAlpha = VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			vk_SwapchainCreateInfoKHR.presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
			vk_SwapchainCreateInfoKHR.clipped = VK_TRUE;
			vk_SwapchainCreateInfoKHR.oldSwapchain = VK_NULL_HANDLE;
		}
		if(vkCreateSwapchainKHR(vk_device, &vk_SwapchainCreateInfoKHR, nullptr, &vk_swapchainKHR) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to create swapchain");
		}
	}
	std::vector<VkImage> vk_swapchainImages;
	{
		uint32_t vk_swapchainImagesCount;
		if(vkGetSwapchainImagesKHR(vk_device, vk_swapchainKHR, &vk_swapchainImagesCount, nullptr) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to get swapchain images count");
		}

		vk_swapchainImages.resize(vk_swapchainImagesCount);
		if(vkGetSwapchainImagesKHR(vk_device, vk_swapchainKHR, &vk_swapchainImagesCount, vk_swapchainImages.data()) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to get swapchain images");
		}
	}

	VkRenderPass vk_renderPass;
	{
		VkAttachmentDescription vk_attachmentDescription;
		{
			vk_attachmentDescription.flags = 0;
			vk_attachmentDescription.format = vk_surfaceFormats[0].format;
			vk_attachmentDescription.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
			vk_attachmentDescription.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
			vk_attachmentDescription.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
			vk_attachmentDescription.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			vk_attachmentDescription.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
			vk_attachmentDescription.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
			vk_attachmentDescription.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		VkAttachmentReference vk_attachmentReference;
		{
			vk_attachmentReference.attachment = 0;
			vk_attachmentReference.layout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
		VkSubpassDescription vk_subpassDescription;
		{
			vk_subpassDescription.flags = 0;
			vk_subpassDescription.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
			vk_subpassDescription.inputAttachmentCount = 0;
			vk_subpassDescription.pInputAttachments = nullptr;
			vk_subpassDescription.colorAttachmentCount = 1;
			vk_subpassDescription.pColorAttachments = &vk_attachmentReference;
			vk_subpassDescription.pResolveAttachments = nullptr;
			vk_subpassDescription.pDepthStencilAttachment = nullptr;
			vk_subpassDescription.preserveAttachmentCount = 0;
			vk_subpassDescription.pPreserveAttachments = nullptr;
		}

		VkRenderPassCreateInfo vk_renderPassCreateInfo;
		{
			vk_renderPassCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			vk_renderPassCreateInfo.pNext = nullptr;
			vk_renderPassCreateInfo.flags = 0;
			vk_renderPassCreateInfo.attachmentCount = 1;
			vk_renderPassCreateInfo.pAttachments = &vk_attachmentDescription;
			vk_renderPassCreateInfo.subpassCount = 1;
			vk_renderPassCreateInfo.pSubpasses = &vk_subpassDescription;
			vk_renderPassCreateInfo.dependencyCount = 0;
			vk_renderPassCreateInfo.pDependencies = nullptr;
		}
		
		if(vkCreateRenderPass(vk_device, &vk_renderPassCreateInfo, nullptr, &vk_renderPass) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to create render pass");
		}
	}

	std::vector<VkFramebuffer> vk_framebuffers(vk_swapchainImages.size());
	std::vector<VkImageView> vk_imageViews(vk_swapchainImages.size());
	{
		for(uint32_t i = 0; i < vk_swapchainImages.size(); ++i)
		{
			auto &vk_imageView = vk_imageViews[i];
			{
				VkImageViewCreateInfo vk_imageViewCreateInfo;
				{
					vk_imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
					vk_imageViewCreateInfo.pNext = nullptr;
					vk_imageViewCreateInfo.flags = 0;
					vk_imageViewCreateInfo.image = vk_swapchainImages[i];
					vk_imageViewCreateInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
					vk_imageViewCreateInfo.format = vk_surfaceFormats[0].format;
					vk_imageViewCreateInfo.components = {
						VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R,
						VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G,
						VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B,
						VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A
					};
					vk_imageViewCreateInfo.subresourceRange;
					{
						vk_imageViewCreateInfo.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
						vk_imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
						vk_imageViewCreateInfo.subresourceRange.levelCount = 1;
						vk_imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
						vk_imageViewCreateInfo.subresourceRange.layerCount = 1;
					}
				}

				if(vkCreateImageView(vk_device, &vk_imageViewCreateInfo, nullptr, &vk_imageView) != VkResult::VK_SUCCESS)
				{
					throw std::exception("failed to create image view");
				}
			}

			auto &vk_framebuffer = vk_framebuffers[i];
			{
				VkFramebufferCreateInfo vk_framebufferCreateInfo;
				{
					vk_framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
					vk_framebufferCreateInfo.pNext = nullptr;
					vk_framebufferCreateInfo.flags = 0;
					vk_framebufferCreateInfo.renderPass = vk_renderPass;
					vk_framebufferCreateInfo.attachmentCount = 1;
					vk_framebufferCreateInfo.pAttachments = &vk_imageView;
					vk_framebufferCreateInfo.width = vk_surfaceCapabilitiesKHR.currentExtent.width;
					vk_framebufferCreateInfo.height = vk_surfaceCapabilitiesKHR.currentExtent.height;
					vk_framebufferCreateInfo.layers = 1;
				}

				if(vkCreateFramebuffer(vk_device, &vk_framebufferCreateInfo, nullptr, &vk_framebuffer) != VkResult::VK_SUCCESS)
				{
					throw std::exception("failed to create framebuffer");
				}
			}
		}
	}

	VkQueue vk_queue;
	{
		vkGetDeviceQueue(vk_device, 0, 0, &vk_queue);
	}
	VkCommandPool vk_commandPool;
	{
		VkCommandPoolCreateInfo vk_commandPoolCreateInfo;
		{
			vk_commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vk_commandPoolCreateInfo.pNext = nullptr;
			vk_commandPoolCreateInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			vk_commandPoolCreateInfo.queueFamilyIndex = 0;
		}

		if(vkCreateCommandPool(vk_device, &vk_commandPoolCreateInfo, nullptr, &vk_commandPool) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to create command pool");
		}

		if(vkResetCommandPool(vk_device, vk_commandPool, VkCommandPoolResetFlagBits::VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to reset command pool");
		}
	}
	std::vector<VkCommandBuffer> vk_commandBuffers(vk_swapchainImages.size());
	{
		VkCommandBufferAllocateInfo vk_CommandBufferAllocateInfo;
		{
			vk_CommandBufferAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vk_CommandBufferAllocateInfo.pNext = nullptr;
			vk_CommandBufferAllocateInfo.commandPool = vk_commandPool;
			vk_CommandBufferAllocateInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			vk_CommandBufferAllocateInfo.commandBufferCount = vk_swapchainImages.size();
		}

		if(vkAllocateCommandBuffers(vk_device, &vk_CommandBufferAllocateInfo, vk_commandBuffers.data()) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to allocate command buffers");
		}

		for(uint32_t i = 0; i < vk_swapchainImages.size(); ++i)
		{
			auto &vk_commandBuffer = vk_commandBuffers[i];
			auto &vk_image = vk_swapchainImages[i];
			auto &vk_framebuffer = vk_framebuffers[i];

			if(vkResetCommandBuffer(vk_commandBuffer, VkCommandBufferResetFlagBits::VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT) != VkResult::VK_SUCCESS)
			{
				throw std::exception("failed to reset command buffer");
			}

			VkCommandBufferInheritanceInfo vk_commandBufferInheritanceInfo;
			{
				vk_commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
				vk_commandBufferInheritanceInfo.pNext = nullptr;
				vk_commandBufferInheritanceInfo.renderPass = vk_renderPass;
				vk_commandBufferInheritanceInfo.subpass = 0;
				vk_commandBufferInheritanceInfo.framebuffer = vk_framebuffer;
				vk_commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
				vk_commandBufferInheritanceInfo.queryFlags = 0;
				vk_commandBufferInheritanceInfo.pipelineStatistics = 0;
			}
			VkCommandBufferBeginInfo vk_commandBufferBeginInfo;
			{
				vk_commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				vk_commandBufferBeginInfo.pNext = nullptr;
				vk_commandBufferBeginInfo.flags = 0;
				vk_commandBufferBeginInfo.pInheritanceInfo = &vk_commandBufferInheritanceInfo;
			}
			vkBeginCommandBuffer(vk_commandBuffer, &vk_commandBufferBeginInfo);
			{
				VkClearValue vk_clearValue;
				{
					vk_clearValue.color.float32[0] = 1.0f;
					vk_clearValue.color.float32[1] = 0.0f;
					vk_clearValue.color.float32[2] = 0.0f;
					vk_clearValue.color.float32[3] = 1.0f;
				}
				VkRenderPassBeginInfo vk_renderPassBeginInfo;
				{
					vk_renderPassBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
					vk_renderPassBeginInfo.pNext = nullptr;
					vk_renderPassBeginInfo.renderPass = vk_renderPass;
					vk_renderPassBeginInfo.framebuffer = vk_framebuffers[i];
					vk_renderPassBeginInfo.renderArea = VkRect2D{
						VkOffset2D{0, 0},
						VkExtent2D{
							vk_surfaceCapabilitiesKHR.currentExtent.width,
							vk_surfaceCapabilitiesKHR.currentExtent.height
						}
					};
					vk_renderPassBeginInfo.clearValueCount = 1;
					vk_renderPassBeginInfo.pClearValues = &vk_clearValue;
				};

				vkCmdBeginRenderPass(
					vk_commandBuffer,
					&vk_renderPassBeginInfo,
					VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE
				);
				vkCmdEndRenderPass(vk_commandBuffer);

				VkImageMemoryBarrier vk_imageMemoryBarrier;
				{
					vk_imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
					vk_imageMemoryBarrier.pNext = nullptr;
					vk_imageMemoryBarrier.srcAccessMask = 0;
					vk_imageMemoryBarrier.dstAccessMask =
						VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
						VkAccessFlagBits::VK_ACCESS_MEMORY_READ_BIT;
					vk_imageMemoryBarrier.oldLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
					vk_imageMemoryBarrier.newLayout = VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
					vk_imageMemoryBarrier.srcQueueFamilyIndex = 0;
					vk_imageMemoryBarrier.dstQueueFamilyIndex = 0;
					vk_imageMemoryBarrier.image = vk_image;
					vk_imageMemoryBarrier.subresourceRange = {VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
				}
				vkCmdPipelineBarrier(
					vk_commandBuffer,
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					0,
					0, nullptr,
					0, nullptr,
					1, &vk_imageMemoryBarrier
				);
			}
			vkEndCommandBuffer(vk_commandBuffer);
		}
	}

	VkFence vk_fence;
	{
		VkFenceCreateInfo vk_fenceCreateInfo;
		{
			vk_fenceCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			vk_fenceCreateInfo.pNext = nullptr;
			vk_fenceCreateInfo.flags = 0;
		}

		if(vkCreateFence(vk_device, &vk_fenceCreateInfo, nullptr, &vk_fence) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to create fence");
		}
	}

	while(true)
	{
		std::cout << "------------------------------- loop -------------------------------" << std::endl;

		MSG msg;
		{
			while(PeekMessage(&msg, handleWindow, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		uint32_t vk_swapchainImage;
		{
			if(vkAcquireNextImageKHR(vk_device, vk_swapchainKHR, UINT64_MAX, VK_NULL_HANDLE, vk_fence, &vk_swapchainImage) != VkResult::VK_SUCCESS)
			{
				throw std::exception("failed to get next swapchain image");
			}
		}

		if(vkWaitForFences(vk_device, 1, &vk_fence, VK_TRUE, UINT64_MAX) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to wait for fence");
		}
		if(vkResetFences(vk_device, 1, &vk_fence) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to reset fence");
		}

		auto &vk_commandBuffer = vk_commandBuffers[vk_swapchainImage];

		VkPipelineStageFlags vk_waitMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		VkSubmitInfo vk_submitInfo;
		{
			vk_submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			vk_submitInfo.pNext = nullptr;
			vk_submitInfo.waitSemaphoreCount = 0;
			vk_submitInfo.pWaitSemaphores = nullptr;
			vk_submitInfo.pWaitDstStageMask = &vk_waitMask;
			vk_submitInfo.commandBufferCount = 1;
			vk_submitInfo.pCommandBuffers = &vk_commandBuffer;
			vk_submitInfo.signalSemaphoreCount = 0;
			vk_submitInfo.pSignalSemaphores = nullptr;
		}
		if(vkQueueSubmit(vk_queue, 1, &vk_submitInfo, VK_NULL_HANDLE) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to submit command buffer");
		}

		if(vkQueueWaitIdle(vk_queue) != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to wait for queue");
		}

		VkResult vk_result;
		VkPresentInfoKHR vk_presentInfoKHR;
		{
			vk_presentInfoKHR.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			vk_presentInfoKHR.pNext = nullptr;
			vk_presentInfoKHR.waitSemaphoreCount = 0;
			vk_presentInfoKHR.pWaitSemaphores = nullptr;
			vk_presentInfoKHR.swapchainCount = 1;
			vk_presentInfoKHR.pSwapchains = &vk_swapchainKHR;
			vk_presentInfoKHR.pImageIndices = &vk_swapchainImage;
			vk_presentInfoKHR.pResults = &vk_result;
		}
		if(vkQueuePresentKHR(vk_queue, &vk_presentInfoKHR) != VkResult::VK_SUCCESS || vk_result != VkResult::VK_SUCCESS)
		{
			throw std::exception("failed to present swapchain");
		}
	}

	system("pause");
}