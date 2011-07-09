#pragma once

#include "Foundation/Automation/Event.h"
#include "Rendering/Renderer.h"
#include "SceneGraph/API.h"

namespace Helium
{
    HELIUM_DECLARE_RPTR( RRenderContext );
}

namespace Helium
{
    enum
    {
        INIT_FLAG_REFRAST = 0x00000001,
    };

    enum
    {
        VERTEX_SHADER_SCREENSPACE = 0,
        VERTEX_SHADER_WORLD_SPACE,
        VERTEX_SHADER_OBJECT_SPACE,
        VERTEX_SHADER_MESH_NORMAL,
        VERTEX_SHADER_MESH_DEBUG_COLOR,
        VERTEX_SHADER_MESH_DEBUG_CONSTCOLOR,
        VERTEX_SHADER_MESH_DEBUG_NORMAL,
        VERTEX_SHADER_MESH_DEBUG_TANGENT,
        VERTEX_SHADER_MESH_DEBUG_UV,

        __VERTEX_SHADER_LAST__
    };

    enum
    {
        PIXEL_SHADER_DIFFUSE = 0,
        PIXEL_SHADER_DIFFUSE_GPI,
        PIXEL_SHADER_COLOR,
        PIXEL_SHADER_TEXTURE,
        PIXEL_SHADER_TEXTURE_GREEN,
        PIXEL_SHADER_TEXTURE_ALPHA,
        PIXEL_SHADER_SKY,

        __PIXEL_SHADER_LAST__
    };

    enum
    {
        VERTEX_DECL_DEBUG = 0,
        VERTEX_DECL_MESH,
        VERTEX_DECL_SCREENSPACE,

        __VERTEX_DECL_LAST__
    };

    enum
    {
        DEFPOOL_RELEASE,
        DEFPOOL_CREATE,
    };

#define __MAX_CLIENTS__ 32

    namespace DeviceStates
    {
        enum DeviceState
        {
            Found,
            Lost
        };
    }
    typedef DeviceStates::DeviceState DeviceState;

    struct DeviceStateArgs
    {
        DeviceState m_DeviceState;

        DeviceStateArgs( DeviceState state )
            : m_DeviceState( state )
        {
        }
    };
    typedef Helium::Signature< const DeviceStateArgs& > DeviceStateSignature;

    // all rendering classes should be derived this
    class SCENE_GRAPH_API DeviceManager : NonCopyable
    {
    public:
        DeviceManager();
        virtual ~DeviceManager();

        static void SetUnique(); // call before init
        bool Init( HWND hwnd, uint32_t back_buffer_width, uint32_t back_buffer_height, uint32_t init_flags = 0 );

    private:
        bool ResizeSwapChain( uint32_t width, uint32_t height );
        bool ResizeDevice( uint32_t width, uint32_t height );
    public:
        bool Resize( uint32_t width, uint32_t height );
        bool Swap();

        bool TestDeviceReady();

        bool SaveTGA(const tchar_t* fname);

        inline uint32_t GetWidth()
        {
            return m_width;
        }

        inline uint32_t GetHeight()
        {
            return m_height;
        }

    private:
        /// Window to which rendering is performed.
        HWND m_hWnd;
        /// Render context.
        Helium::RRenderContextPtr m_spRenderContext;

        bool                                  m_using_swapchain;
        uint32_t                              m_width;
        uint32_t                              m_height;

        /// Main render context.
        static Helium::RRenderContextPtr sm_spMainRenderContext;
        /// Main render context window handle.
        static HWND sm_hMainRenderContextWnd;
        /// Main render context width.
        static uint32_t sm_mainRenderContextWidth;
        /// Main render context heigth.
        static uint32_t sm_mainRenderContextHeight;

        static bool                           m_unique;
        static uint32_t                       m_master_count;
        static DeviceManager*                 m_clients[__MAX_CLIENTS__];
    };
}