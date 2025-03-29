#include "VitraePluginBasicComposition/Setup.hpp"

#include "VitraePluginBasicComposition/tasks/displayCamera.hpp"
#include "VitraePluginBasicComposition/tasks/displayNormals.hpp"
#include "VitraePluginBasicComposition/tasks/helpers.hpp"
#include "VitraePluginBasicComposition/tasks/renderForward.hpp"
#include "VitraePluginBasicComposition/tasks/renderSilhouette.hpp"
#include "VitraePluginBasicComposition/tasks/shadingTransform.hpp"
#include "VitraePluginBasicComposition/tasks/shadowCommon.hpp"

namespace VitraePluginBasicComposition
{

void setup(Vitrae::ComponentRoot &root)
{
    setupDisplayCamera(root);
    setupDisplayNormals(root);
    setupRenderForward(root);
    setupRenderSilhouette(root);
    setupShadingTransform(root);
    setupShadowCommon(root);
}

} // namespace VitraePluginBasicComposition