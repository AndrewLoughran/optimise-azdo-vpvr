#pragma once

#ifndef ENABLE_SDL
#include "inc\gpuprofiler.h"
#endif
#include "RenderDevice.h"
#include "Texture.h"
#include "backGlass.h"
#include <EASTL/vector.h>

extern int NumVideoBytes;

enum
{
   TEXTURE_MODE_POINT,				// Point sampled (aka no) texture filtering.
   TEXTURE_MODE_BILINEAR,			// Bilinar texture filtering. 
   TEXTURE_MODE_TRILINEAR,			// Trilinar texture filtering. 
   TEXTURE_MODE_ANISOTROPIC		// Anisotropic texture filtering. 
};

class BackGlass;

class PinProjection
{
public:
   void ScaleView(const float x, const float y, const float z);
   void MultiplyView(const Matrix3D& mat);
   void RotateView(float x, float y, float z);
   void TranslateView(const float x, const float y, const float z);

   void FitCameraToVerticesFS(eastl::vector<Vertex3Ds>& pvvertex3D, float aspect, float rotation, float inclination, float FOV, float xlatez, float layback);
   void FitCameraToVertices(eastl::vector<Vertex3Ds>& pvvertex3D, float aspect, float rotation, float inclination, float FOV, float xlatez, float layback);
   void CacheTransform();      // compute m_matrixTotal = m_World * m_View * m_Proj
   void TransformVertices(const Vertex3Ds * const rgv, const WORD * const rgi, const int count, Vertex2D * const rgvout) const;

   void ComputeNearFarPlane(eastl::vector<Vertex3Ds>& verts);

   Matrix3D m_matWorld;
   Matrix3D m_matView;
   Matrix3D m_matProj[2];
   Matrix3D m_matrixTotal[2];
   int m_stereo3D;

   RECT m_rcviewport;

   float m_rznear, m_rzfar;
   Vertex3Ds m_vertexcamera;
   //float m_cameraLength;
};

class Pin3D
{
public:
   Pin3D();
   ~Pin3D();

   HRESULT InitPin3D(HWND* hwnd, const bool fullScreen, const int width, const int height, const int colordepth, int &refreshrate, const int VSync, const float AAfactor, const int stereo3D, const unsigned int FXAA, const bool useAO, const bool ss_refl);

   void InitLayoutFS();
   void InitLayout(const bool FSS_mode, const float xpixoff = 0.f, const float ypixoff = 0.f);

   void TransformVertices(const Vertex3D_NoTex2 * rgv, const WORD * rgi, int count, Vertex2D * rgvout) const;

   Vertex3Ds Unproject(const Vertex3Ds& point);
   Vertex3Ds Get3DPointFrom2D(const POINT& p);

   void Flip(bool vsync);
#ifdef ENABLE_SDL
   void SetRenderTarget(RenderDevice * const pd3dDevice, RenderTarget* pddsSurface, void* unused) const;
   void SetPrimaryRenderTarget(RenderTarget* pddsSurface, void* unused) const;
   void SetSecondaryRenderTarget(RenderTarget* pddsSurface, void* unused) const;
#else
   void SetRenderTarget(RenderDevice * const pd3dDevice, D3DTexture* pddsSurface, RenderTarget* pddsZ) const;
   void SetRenderTarget(RenderDevice * const pd3dDevice, D3DTexture* pddsSurface, D3DTexture* pddsZ) const;
   void SetRenderTarget(RenderDevice * const pd3dDevice, RenderTarget* pddsSurface, void* pddsZ) const;
   void SetRenderTarget(RenderDevice * const pd3dDevice, D3DTexture* pddsSurface, void* pddsZ) const;

   void SetPrimaryRenderTarget(D3DTexture* pddsSurface, RenderTarget* pddsZ) const;
   void SetPrimaryRenderTarget(D3DTexture* pddsSurface, D3DTexture* pddsZ) const;
   void SetPrimaryRenderTarget(RenderTarget* pddsSurface, void* pddsZ) const;
   void SetPrimaryRenderTarget(D3DTexture* pddsSurface, void* pddsZ) const;
   void SetSecondaryRenderTarget(D3DTexture* pddsSurface, RenderTarget* pddsZ) const;
   void SetSecondaryRenderTarget(D3DTexture* pddsSurface, D3DTexture* pddsZ) const;
   void SetSecondaryRenderTarget(RenderTarget* pddsSurface, void* pddsZ) const;
   void SetSecondaryRenderTarget(D3DTexture* pddsSurface, void* pddsZ) const;
#endif

   void SetTextureFilter(RenderDevice * const pd3dDevice, const int TextureNum, const int Mode) const;
   void SetPrimaryTextureFilter(const int TextureNum, const int Mode) const;
   void SetSecondaryTextureFilter(const int TextureNum, const int Mode) const;

   void EnableAlphaTestReference(const DWORD alphaRefValue) const;
   void EnableAlphaBlend(const bool additiveBlending, const bool set_dest_blend = true, const bool set_blend_op = true) const;

   void DrawBackground();
   void RenderPlayfieldGraphics(const bool depth_only);

   const Matrix3D& GetWorldTransform() const   { return m_proj.m_matWorld; }
   const Matrix3D& GetViewTransform() const    { return m_proj.m_matView; }
   void InitPlayfieldGraphics();
   void InitLights();
   void UpdateMatrices();

   BackGlass* backGlass;

private:
   void InitRenderState(RenderDevice * const pd3dDevice);
   void InitPrimaryRenderState();
   void InitSecondaryRenderState();

   HRESULT InitPrimary(HWND *hwnd, const bool fullScreen, const int colordepth, int &refreshrate, const int VSync, const int stereo3D, const unsigned int FXAA, const bool useAO, const bool ss_refl);

   int m_stereo3D;

   // Data members
public:
   RenderDevice* m_pd3dPrimaryDevice;
   RenderDevice* m_pd3dSecondaryDevice;

   RenderTarget* m_pddsBackBuffer;

   D3DTexture* m_pddsAOBackBuffer;
   D3DTexture* m_pddsAOBackTmpBuffer;

   D3DTexture* m_pdds3DZBuffer;

#ifdef FPS
#ifndef ENABLE_SDL
   CGpuProfiler m_gpu_profiler;
#endif
#endif
   void* m_pddsZBuffer; // D3DTexture* or RenderTarget*, depending on HW support

   Texture pinballEnvTexture; // loaded from Resources
   Texture envTexture; // loaded from Resources
   Texture aoDitherTexture; // loaded from Resources

   Texture* m_envTexture;
   BaseTexture* m_envRadianceTexture;

   PinProjection m_proj;

   // free-camera-mode-fly-around parameters
   float m_camx;
   float m_camy;
   float m_camz;
   float m_inc;
   HWND m_hwnd;

   //Vertex3Ds m_viewVec;        // direction the camera is facing

   ViewPort m_viewPort;
   float m_AAfactor;

private:
   VertexBuffer *tableVBuffer;
   bool firstRun;
#ifdef ENABLE_BAM
   public: void UpdateBAMHeadTracking();                 // #ravarcade: UpdateBAMHeadTracking will set proj/view matrix to add BAM view and head tracking
#endif
};

Matrix3D ComputeLaybackTransform(float layback);
