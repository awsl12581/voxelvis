#if !defined(_TEA_INCLUDED_UI_IM_IMAGE_H_)
#define _TEA_INCLUDED_UI_IM_IMAGE_H_

#include "render_base.h"

namespace vis
{
namespace im
{

class Image : public RenderBase<std::vector<std::string>>
{
public:
    Image(const std::string& component_name) :
        RenderBase<std::vector<std::string>>(component_name)
    {
    }

    virtual ~Image() = default;
    virtual void render(const std::vector<std::string>& data) const override;
};
} // namespace im
} // namespace vis

#endif // _TEA_INCLUDED_UI_IM_IMAGE_H_
