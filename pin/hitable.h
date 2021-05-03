#include "stdafx.h"
#pragma once
//#include "player.h"
//#include "math/vector.h"
#include "color.h"
#include "Shader.h"
//#include "pin3d.h"
//#include "extern.h"

//Player* g_pplayer;

class Hitable
{
public:
   virtual void GetHitShapes(vector<HitObject*> &pvho) = 0;
   virtual void GetHitShapesDebug(vector<HitObject*> &pvho) = 0;
   virtual void GetTimers(vector<HitTimer*> &pvht) = 0;
   virtual EventProxyBase *GetEventProxyBase() = 0;
   virtual void EndPlay() = 0;
   virtual void PreRenderStatic() { } // only used for kickers, due to special playfield cutout
   virtual void RenderStatic() = 0;
   virtual void RenderDynamic() = 0;
   virtual void RenderSetup() = 0;
   virtual void UpdateWorldMatrix(std::vector<Matrix3D>* _allWorldMatrices) { }; // update world matrix

   typedef struct {
       //DrawElementsIndirectCommand() : count(0), instanceCount(0), firstIndex(0), baseVertex(0), baseInstance(0) {};
       GLuint count;
       GLuint instanceCount;
       GLuint firstIndex;
       GLuint baseVertex;
       GLuint baseInstance;  //baseInstance can be used for our own drawID setup (faster than using gl_DrawIDARB)
   } DrawElementsIndirectCommand;
   
   typedef struct
   {
       GLuint      name;
       GLuint64    handle;
   } BindlessTexture;

   typedef struct {
       GLuint64 texHandle; // treated as a vec2 I think (8 bytes)
       GLuint64 normalMapHandle; 
       GLuint64 paddingHandle;

       float AZDO_alphaTestValue;
       float padding1;

       // /*
       vec4 AZDO_cClearcoat_EdgeAlpha;
       vec4 AZDO_cGlossy_ImageLerp;
       vec4 AZDO_cBase_Alpha; // declared in material.fxh
       vec4 AZDO_Roughness_WrapL_Edge_Thickness; // declared in material.fxh
       vec4 AZDO_fDisableLighting_top_below;


       // these are bools but for packing reasons, using floats is simpler. This could be memory-optimised by packing the bools into one float or uint and unpacking in shader.
       float AZDO_hdrEnvTextures;
       float AZDO_objectSpaceNormalMap;
       float AZDO_is_metal;
       float AZDO_doNormalMapping;
       // */
       //float padding2;
       //float padding3;
       
   } MaterialProperties;   
   /*
   typedef struct {
       vec4 cClearcoat_EdgeAlpha;
       vec4 cGlossy_ImageLerp;
       vec4 cBase_Alpha; // declared in material.fxh
       vec4 Roughness_WrapL_Edge_Thickness; // declared in material.fxh
       GLuint64 texHandle; // treated as a vec2 I think
       bool hdrEnvTextures;
       bool padding1[3];
       bool objectSpaceNormalMap;
       bool is_metal;
       bool doNormalMapping;
       //float alphaTestValue;
       //float padding1;
       //float padding2;
       //float padding3;
       
   } MaterialPropertiesTest;
   */
   typedef struct
   {
       Matrix3D matWorldView;
       Matrix3D matWorldViewInverseTranspose;
       //Matrix3D unusedFillerFormatWorldViewInverseTranspose; // still needed? be aware of this structs size
       Matrix3D matWorldViewProj[2]; // remember there's 2 of these in VR
   } ObjMatrices;

   
   static GLuint drawID;
   static GLuint bufferBaseVertex;
   static GLuint bufferBaseIndex;  
   static std::vector<BindlessTexture> _allBindlessTextures; // a list to allow us to delete everything later
   
   virtual void MultiDrawSetup(std::vector<DrawElementsIndirectCommand>* m_commands,
       std::vector<Vertex3D_NoTex2>* _allVertices,
       std::vector<unsigned int>* _allIndices,
       std::vector<MaterialProperties>* _allMaterials,
       std::vector<ObjMatrices>* _allMatrices,
       std::vector<Matrix3D>* _allWorldMatrices)
   {
       // default nothing
   };

   static void Cleanup() {
       for (const auto& i : _allBindlessTextures) {
           if (i.handle != 0) {
               glMakeTextureHandleNonResidentARB(i.handle);
           }
           glDeleteTextures(1, &i.name);
       }

       _allBindlessTextures.clear();
       
       drawID = 0;
       bufferBaseVertex = 0;
       bufferBaseIndex = 0;
   }

   virtual ItemTypeEnum HitableGetItemType() const = 0;

   virtual bool IsTransparent() const { return false; }
   virtual float GetDepth(const Vertex3Ds& viewDir) const { return 0.0f; }
   virtual unsigned long long GetMaterialID() const { return 0; }
   virtual unsigned long long GetImageID() const { return 0; }
   virtual bool RenderToLightBuffer() const { return false; }
   virtual bool IsDMD() const { return false; }



   void PrepareMultiDraw(std::vector<DrawElementsIndirectCommand>* m_commands,
       std::vector<Vertex3D_NoTex2>* _allVertices,
       std::vector<unsigned int>* _allIndices,
       std::vector<MaterialProperties>* _allMaterials,
       std::vector<ObjMatrices>* _allMatrices,
       std::vector<Matrix3D>* _allWorldMatrices,
       const VertexBuffer* const  objVertexBuffer,
       const IndexBuffer* const  objIndexBuffer,
       const Material* const  mat,
       Texture* tex,
       Matrix3D* const objWorldMatrix,
       Texture* normalMap = nullptr, // only potentially used in Primitive objects
       int vertexCount = 0, // these are used for specifying partial data from a buffer (only for Surface objects)
       int vertexOffset = 0,
       int indexCount = 0,
       int indexOffset = 0
   );


   void SetWorldMatrix(Matrix3D* worldMatrix, Vertex3Ds objScaling, Vertex3Ds objRotation, Vertex3Ds objTranslation);

};