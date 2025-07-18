#pragma once

#include "Vitrae/Assets/FrameStore.hpp"
#include "Vitrae/Collections/ComponentRoot.hpp"
#include "Vitrae/Collections/MethodCollection.hpp"
#include "Vitrae/Params/Standard.hpp"
#include "Vitrae/Pipelines/Compositing/AdaptTasks.hpp"
#include "Vitrae/Pipelines/Compositing/ClearRender.hpp"
#include "Vitrae/Pipelines/Compositing/Function.hpp"
#include "Vitrae/Pipelines/Compositing/SceneRender.hpp"

#include "dynasma/standalone.hpp"

namespace VitraePluginBasicComposition
{
using namespace Vitrae;

inline void setupDisplayCamera(ComponentRoot &root)
{
    MethodCollection &methodCollection = root.getComponent<MethodCollection>();

    // shadow matrices extractor
    auto p_extractLightProperties =
        dynasma::makeStandalone<ComposeFunction>(ComposeFunction::SetupParams{
            .inputSpecs = {{
                ParamSpec{.name = "scene", .typeInfo = TYPE_INFO<dynasma::FirmPtr<Scene>>},
                ParamSpec{.name = "fs_shadow", .typeInfo = TYPE_INFO<dynasma::FirmPtr<FrameStore>>},
                ParamSpec{.name = "shadow_distance",
                          .typeInfo = TYPE_INFO<float>,
                          .defaultValue = 100.0f},
                ParamSpec{
                    .name = "shadow_above", .typeInfo = TYPE_INFO<float>, .defaultValue = 80.0f},
                ParamSpec{
                    .name = "shadow_below", .typeInfo = TYPE_INFO<float>, .defaultValue = 80.0f},
            }},
            .outputSpecs = {{
                ParamSpec{.name = "mat_shadow_view", .typeInfo = TYPE_INFO<glm::mat4>},
                ParamSpec{.name = "mat_shadow_proj", .typeInfo = TYPE_INFO<glm::mat4>},
                ParamSpec{.name = "light_direction", .typeInfo = TYPE_INFO<glm::vec3>},
                ParamSpec{.name = "light_color_primary", .typeInfo = TYPE_INFO<glm::vec3>},
                ParamSpec{.name = "light_color_ambient", .typeInfo = TYPE_INFO<glm::vec3>},
            }},
            .p_function =
                [](const RenderComposeContext &context) {
                    try {
                        auto p_scene =
                            context.properties.get("scene").get<dynasma::FirmPtr<Scene>>();
                        auto p_shadowFrame =
                            context.properties.get("fs_shadow").get<dynasma::FirmPtr<FrameStore>>();
                        auto shadow_distance =
                            context.properties.get("shadow_distance").get<float>();
                        auto shadow_above = context.properties.get("shadow_above").get<float>();
                        auto shadow_below = context.properties.get("shadow_below").get<float>();

                        context.properties.set(
                            "mat_shadow_view",
                            p_scene->light.getViewMatrix(p_scene->camera, shadow_distance,
                                                         1.0 / p_shadowFrame->getSize().x));
                        context.properties.set("mat_shadow_proj",
                                               p_scene->light.getProjectionMatrix(
                                                   shadow_distance, shadow_above, shadow_below));
                        context.properties.set("light_direction",
                                               glm::normalize(p_scene->light.direction));
                        context.properties.set("light_color_primary", p_scene->light.color_primary);
                        context.properties.set("light_color_ambient", p_scene->light.color_ambient);
                    }
                    catch (const std::out_of_range &e) {
                    }
                },
            .friendlyName = "Light properties",
        });
    methodCollection.registerComposeTask(p_extractLightProperties);

    // camera matrices extractor
    auto p_extractCameraMatrices =
        dynasma::makeStandalone<ComposeFunction>(ComposeFunction::SetupParams{
            .inputSpecs = {{
                StandardParam::scene,
                StandardParam::fs_target,
            }},
            .outputSpecs = {{
                {"target_resolution", TYPE_INFO<glm::uvec2>},
                StandardParam::mat_proj,
                StandardParam::mat_view,
            }},
            .p_function =
                [](const RenderComposeContext &context) {
                    try {
                        auto p_scene =
                            context.properties.get("scene").get<dynasma::FirmPtr<Scene>>();
                        auto p_windowFrame =
                            context.properties.get("fs_target").get<dynasma::FirmPtr<FrameStore>>();

                        context.properties.set("target_resolution", p_windowFrame->getSize());
                        context.properties.set(
                            StandardParam::mat_proj.name,
                            p_scene->camera.getPerspectiveMatrix(p_windowFrame->getSize().x,
                                                                 p_windowFrame->getSize().y));
                        context.properties.set(StandardParam::mat_view.name,
                                               p_scene->camera.getViewMatrix());
                    }
                    catch (const std::out_of_range &e) {
                        throw;
                    }
                },
            .friendlyName = "Camera matrices",
        });
    methodCollection.registerComposeTask(p_extractCameraMatrices);

    // camera properties extractor
    auto p_extractCameraProperties =
        dynasma::makeStandalone<ComposeFunction>(ComposeFunction::SetupParams{
            .inputSpecs = {{
                StandardParam::scene,
                StandardParam::fs_target,
            }},
            .outputSpecs = {{
                ParamSpec{.name = "camera_position", .typeInfo = TYPE_INFO<glm::vec3>},
                ParamSpec{.name = "camera_direction", .typeInfo = TYPE_INFO<glm::vec3>},
            }},
            .p_function =
                [](const RenderComposeContext &context) {
                    try {
                        auto p_scene =
                            context.properties.get("scene").get<dynasma::FirmPtr<Scene>>();

                        context.properties.set("camera_position", p_scene->camera.position);
                        context.properties.set("camera_direction", p_scene->camera.rotation *
                                                                       glm::vec3{0.0f, 0.0f, 1.0f});
                    }
                    catch (const std::out_of_range &e) {
                        throw;
                    }
                },
            .friendlyName = "Camera properties",
        });
    methodCollection.registerComposeTask(p_extractCameraProperties);

    auto p_renderAdaptor = dynasma::makeStandalone<ComposeAdaptTasks>(
        ComposeAdaptTasks::SetupParams{.root = root,
                                       .adaptorAliases =
                                           {
                                               {"camera_displayed", "scene_rendered"},
                                               {"position_view", "position_camera_view"},
                                               {"fs_target", "fs_display"},
                                           },
                                       .desiredOutputs = {ParamSpec{
                                           "camera_displayed",
                                           TYPE_INFO<void>,
                                       }},
                                       .friendlyName = "Render camera"});
    methodCollection.registerComposeTask(p_renderAdaptor);

    methodCollection.registerPropertyOption(StandardParam::fragment_color.name,
                                            StandardParam::shade.name);

    methodCollection.registerCompositorOutput("camera_displayed");
}
} // namespace VitraePluginBasicComposition