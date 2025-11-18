#pragma once

#include "Vitrae/Assets/FrameStore.hpp"
#include "Vitrae/Assets/Material.hpp"
#include "Vitrae/Assets/Texture.hpp"
#include "Vitrae/Collections/ComponentRoot.hpp"
#include "Vitrae/Collections/MethodCollection.hpp"
#include "Vitrae/Params/Standard.hpp"
#include "Vitrae/Pipelines/Compositing/ClearRender.hpp"
#include "Vitrae/Pipelines/Compositing/SceneRender.hpp"

#include "dynasma/standalone.hpp"

namespace VitraePluginBasicComposition
{
    using namespace Vitrae;

    inline bool isOpaque(const Material &mat)
    {
        auto &matProperties = mat.getProperties();
        if (auto it_tex_diffuse = matProperties.find("tex_diffuse"); it_tex_diffuse != matProperties.end())
        {
            auto p_tex_diffuse =
                (*it_tex_diffuse).second.get<dynasma::FirmPtr<Texture2D<BufferType::REAL_VEC4>>>();

            if (p_tex_diffuse->getProperties()
                    .get(Vitrae::StandardParam::is_transparent.name, false)
                    .get<bool>() == true) {
                return false;
            }
        }
        return true;
    }
}