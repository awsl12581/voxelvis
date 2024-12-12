#if !defined(_TEA_INCLUDED_UI_RENDER_BASE_H_)
#define _TEA_INCLUDED_UI_RENDER_BASE_H_

#include <imgui.h>

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace vis
{
namespace im
{
template <typename T>
class RenderBase
{
public:
    RenderBase(const std::string& component_name) :
        m_component_name(component_name)
    {
    }

    virtual ~RenderBase() = default;

    virtual void render(const T& data) const = 0;

private:
    std::string m_component_name;
};
} // namespace im
} // namespace vis

#endif // _TEA_INCLUDED_UI_RENDER_BASE_H_
