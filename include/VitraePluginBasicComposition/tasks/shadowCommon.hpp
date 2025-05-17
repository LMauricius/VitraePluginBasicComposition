#pragma once

#include "Vitrae/Assets/FrameStore.hpp"
#include "Vitrae/Collections/ComponentRoot.hpp"
#include "Vitrae/Collections/MethodCollection.hpp"
#include "Vitrae/Pipelines/Compositing/AdaptTasks.hpp"
#include "Vitrae/Pipelines/Compositing/ClearRender.hpp"
#include "Vitrae/Pipelines/Compositing/FrameToTexture.hpp"
#include "Vitrae/Pipelines/Compositing/SceneRender.hpp"

#include "dynasma/standalone.hpp"

namespace VitraePluginBasicComposition
{
using namespace Vitrae;

inline void setupShadowCommon(ComponentRoot &root)
{
    MethodCollection &methodCollection = root.getComponent<MethodCollection>();

    methodCollection.registerShaderTask(
        root.getComponent<ShaderSnippetKeeper>().new_asset({ShaderSnippet::StringParams{
            .inputSpecs =
                {
                    {"position_world", TYPE_INFO<glm::vec4>},
                    {"mat_shadow_view", TYPE_INFO<glm::mat4>},
                    {"mat_shadow_proj", TYPE_INFO<glm::mat4>},
                },
            .outputSpecs =
                {
                    {"position_shadow", TYPE_INFO<glm::vec3>},
                    {"position_shadow_view", TYPE_INFO<glm::vec4>},
                },
            .snippet = R"glsl(
                    position_shadow_view = mat_shadow_proj * mat_shadow_view * position_world;
                    position_shadow = position_shadow_view.xyz / position_shadow_view.w * 0.5 + 0.5;
                )glsl",
        }}),
        ShaderStageFlag::Vertex | ShaderStageFlag::Compute);

    methodCollection.registerComposeTask(
        dynasma::makeStandalone<ComposeFrameToTexture>(ComposeFrameToTexture::SetupParams{
            .root = root,
            .inputTokenNames = {"scene_silhouette_rendered"},
            .outputs = {{
                .textureName = "tex_shadow_adapted",
                .shaderComponent = FixedRenderComponent::Depth,
                .format = BufferFormat::DEPTH_STANDARD,
                .clearColor = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f},
                .filtering =
                    {
                        .horWrap = WrappingType::BORDER_COLOR,
                        .verWrap = WrappingType::BORDER_COLOR,
                        .minFilter = FilterType::NEAREST,
                        .magFilter = FilterType::NEAREST,
                        .useMipMaps = false,
                        .borderColor = {1.0f, 1.0f, 1.0f, 1.0f},
                    },
            }},
            .size{String("ShadowMapSize"), {1024, 1024}},
        }));

    methodCollection.registerComposeTask(
        dynasma::makeStandalone<ComposeAdaptTasks>(ComposeAdaptTasks::SetupParams{
            .root = root,
            .adaptorAliases =
                {
                    {"fs_target", "fs_shadow"},
                    {"position_view", "position_shadow_view"},
                    {"mat_view", "mat_shadow_view"},
                    {"mat_proj", "mat_shadow_proj"},
                    {"tex_shadow", "tex_shadow_adapted"},
                },
            .desiredOutputs =
                {
                    {
                        "tex_shadow",
                        TYPE_INFO<dynasma::FirmPtr<Texture>>,
                    },
                },
            .friendlyName = "Render shadows",
        }));
}
} // namespace VitraePluginBasicComposition