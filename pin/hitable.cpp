#include "stdafx.h"
#include "hitable.h"
//#pragma once
//#include "player.h"
//#include "math/vector.h"
#include "color.h"
#include "Shader.h"
//#include "extern.h"
GLuint Hitable::drawID = 0;
GLuint Hitable::bufferBaseVertex = 0;
GLuint Hitable::bufferBaseIndex = 0;
std::vector<Hitable::BindlessTexture> Hitable::_allBindlessTextures;

void Hitable::PrepareMultiDraw(std::vector<DrawElementsIndirectCommand>* m_commands,
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
    Texture* normalMap,
    int vertexCount, // these are used for specifying partial data from a buffer (only for Surface objects)
    int vertexOffset,
    int indexCount,
    int indexOffset
    )
{

    const int x = sizeof(MaterialProperties);
    const int a = sizeof(GLuint64);
    const int b = sizeof(vec4);
    const int c = sizeof(float);
    const int d = sizeof(bool);
    const int e = sizeof(GLboolean);
    DrawElementsIndirectCommand cmd = { 0,0,0,0,0 };


    //GLuint indexCount = 0, vertexCount = 0;
    if (vertexCount == 0) vertexCount = objVertexBuffer->getCount();
    if (indexCount == 0) indexCount = objIndexBuffer->getCount();
    //assert(vertexCount != 0);
    //assert(indexCount != 0);

    cmd.count = indexCount;  // specifies the number of elements to be rendered
    cmd.instanceCount = 1;         // 0 if hidden, 1 if visible? this is where we want to use stereo instancing later

    // is startIndex always 0? yes EXCEPT FOR SURFACE
    // startVertex is sometimes non-zero.

    cmd.firstIndex = bufferBaseIndex; // +objIndexBuffer->getOffset() + (objIndexBuffer->getIndexFormat() == IndexBuffer::FMT_INDEX16 ? 2 : 4) * startIndex;
    bufferBaseIndex += indexCount;
    //cmd.baseVertex = objVertexBuffer->getOffset() + startVertex;

    cmd.baseVertex = bufferBaseVertex;
    bufferBaseVertex += vertexCount;

    cmd.baseInstance = drawID++;

    m_commands->push_back(cmd);
    //const Material* mat = m_ptable->GetMaterial(m_d.m_szBaseMaterial); // not sure if this logic is consistent for all obj classes
    //const Texture* tex = &m_baseTexture;

    //if(objVertexBuffer->getFvf() == MY_D3DFVF_TEX)
     //   Vertex3D_TexelOnly* vb = (Vertex3D_TexelOnly*)objVertexBuffer->getBuffer(); // is this vertex format ALWAYS the same? may have to check if it is Vertex3D_TexelOnly
   // else


    const Vertex3D_NoTex2* vertData = static_cast<Vertex3D_NoTex2*>(objVertexBuffer->getBuffer()); // all objects except FLASHERS use this? would need separate draw for them?
    //std::unique_ptr<Vertex3D_NoTex2> vertData = static_cast<Vertex3D_NoTex2*>(objVertexBuffer->getBuffer()); 

    const unsigned int* indexData = static_cast<unsigned int*>(objIndexBuffer->getBuffer());

    // copy the array into a safer vector
    std::vector<Vertex3D_NoTex2> vertVector(vertData + vertexOffset, vertData + vertexOffset + vertexCount);
    std::vector<unsigned int> indexVector(indexData + indexOffset, indexData + indexOffset + indexCount);

    _allVertices->insert(_allVertices->end(), vertVector.begin(), vertVector.end());
    _allIndices->insert(_allIndices->end(), indexVector.begin(), indexVector.end());

    //for (GLuint i = 0; i < objVertexBuffer->getCount(); i++) {
    //    _allVertices->push_back(vertData[i]);
    //}
    //
    //for (GLuint i = 0; i < objIndexBuffer->getCount(); i++) {
    //    _allIndices->push_back(indexData[i]);
    //}
    // Tex Handle and Material
    {
        colorFormat format = RGBA;
        int width = 0, height = 0;
        GLuint texName = 0;
        GLenum col_type = 0, col_format = 0;
        void* texData = nullptr;

        if (tex == nullptr || tex->m_pdsBuffer->m_data.size() == 0) {
            //if (true) {
                //tex = pd3dDevice->CreateTexture(2, 2, 0, STATIC, RGBA, &data, 0);
                // CreateTex


                // colourful debug texture              
                // unsigned int data[4] = { 0xff0000ff, 0xffffff00, 0xffff0000, 0xff00ff00 };

                // blank invisible tex 
                // unsigned int data[4] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

                // grey tex
            unsigned int data[4] = { 0xff888888, 0xff888888, 0xff888888, 0xff888888 };


            //GLubyte data[] = { 255,255,255,255 };
            texData = &data[0];
            format = RGBA; // therefore...
            col_type = GL_UNSIGNED_BYTE;
            col_format = GL_BGRA;
            width = 2;
            height = 2;
        }
        else {
            BaseTexture* baseTex = tex->m_pdsBuffer;
            format = (baseTex->m_format == BaseTexture::RGB_FP ? RGB32F : RGBA);
            col_type = ((format == RGBA32F) || (format == RGBA16F) || (format == RGB32F) || (format == RGB16F)) ? GL_FLOAT : GL_UNSIGNED_BYTE;
            col_format = (format == GREY) ? GL_RED : (format == GREY_ALPHA) ? GL_RG : ((format == RGB) || (format == RGB5) || (format == RGB10) || (format == RGB16F) || (format == RGB32F)) ? GL_BGR : GL_BGRA;

            width = baseTex->width();
            height = baseTex->height();

            texData = baseTex->m_data.data();

            //std::vector<BYTE> debugTex(baseTex->m_data.begin(), baseTex->m_data.end());
            //texData = baseTex->m_data.data();
        }


        glCreateTextures(GL_TEXTURE_2D, 1, &texName);

        //TEX PARAMETERS
        {
            const bool clampToEdge = true; // ASSUME FALSE FOR NOW, SHOULD TECHNICALLY GET THIS INFO FROM basicShader->SetTexture calls!!!
            if (clampToEdge)
            {
                CHECKD3D(glTextureParameteri(texName, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
                CHECKD3D(glTextureParameteri(texName, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            }
            else
            {
                CHECKD3D(glTextureParameteri(texName, GL_TEXTURE_WRAP_S, GL_REPEAT));
                CHECKD3D(glTextureParameteri(texName, GL_TEXTURE_WRAP_T, GL_REPEAT));
            }

            CHECKD3D(glTextureParameteri(texName, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR)); // Use mipmap filtering GL_LINEAR_MIPMAP_LINEAR
            CHECKD3D(glTextureParameteri(texName, GL_TEXTURE_MAG_FILTER, GL_LINEAR)); // MAG Filter does not support mipmaps

            if (format == GREY) {//Hack so that GL_RED behaves as GL_GREY
                CHECKD3D(glTextureParameteri(texName, GL_TEXTURE_SWIZZLE_R, GL_RED));
                CHECKD3D(glTextureParameteri(texName, GL_TEXTURE_SWIZZLE_G, GL_RED));
                CHECKD3D(glTextureParameteri(texName, GL_TEXTURE_SWIZZLE_B, GL_RED));
                CHECKD3D(glTextureParameteri(texName, GL_TEXTURE_SWIZZLE_A, GL_ALPHA));
                format = RGB8;
            }
            else if (format == GREY_ALPHA) {//Hack so that GL_RG behaves as GL_GREY_ALPHA
                CHECKD3D(glTextureParameteri(texName, GL_TEXTURE_SWIZZLE_R, GL_RED));
                CHECKD3D(glTextureParameteri(texName, GL_TEXTURE_SWIZZLE_G, GL_RED));
                CHECKD3D(glTextureParameteri(texName, GL_TEXTURE_SWIZZLE_B, GL_RED));
                CHECKD3D(glTextureParameteri(texName, GL_TEXTURE_SWIZZLE_A, GL_GREEN));
                format = RGB8;
            }
            else {//Default
                CHECKD3D(glTextureParameteri(texName, GL_TEXTURE_SWIZZLE_R, GL_RED));
                CHECKD3D(glTextureParameteri(texName, GL_TEXTURE_SWIZZLE_G, GL_GREEN));
                CHECKD3D(glTextureParameteri(texName, GL_TEXTURE_SWIZZLE_B, GL_BLUE));
                CHECKD3D(glTextureParameteri(texName, GL_TEXTURE_SWIZZLE_A, GL_ALPHA));

                // Do Anisotropic filtering for large textures
                //RenderDevice* const pd3dDevice = g_pplayer->m_pin3d.m_pd3dPrimaryDevice;
                //if (pd3dDevice->m_maxaniso > 0 && width >= 1536 && height >= 1536)
                //    CHECKD3D(glTextureParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, pd3dDevice->m_maxaniso));
            }
        }


        const int num_mips = static_cast<int>(std::log2(static_cast<float>(std::max(width, height)))) + 1;

        glTextureStorage2D(texName, GLint{ num_mips }, GL_RGBA8, GLsizei{ width }, GLsizei{ height });
        glTextureSubImage2D(texName, 0, 0, 0, GLsizei{ width }, GLsizei{ height }, col_format, col_type, texData);
        //glClearTexImage(texName, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
        glGenerateTextureMipmap(texName);
        //glFinish();

        // SHADER MEMORY std140 LAYOUT

        // https://www.khronos.org/registry/OpenGL/specs/gl/glspec46.core.pdf PAGE 146
        // https://docs.microsoft.com/en-us/cpp/build/reference/zp-struct-member-alignment?view=msvc-160 note the diff between 32/64-bit defaults
        // https://stackoverflow.com/questions/38172696/should-i-ever-use-a-vec3-inside-of-a-uniform-buffer-or-shader-storage-buffer-o

        /*
        *  MEMBER                                                                           base alignment          offset          aligned offset
           typedef struct {
               vec4 cClearcoat_EdgeAlpha;                                                   16                      0               0
               vec4 cGlossy_ImageLerp;                                                      16                      16              16
               vec4 cBase_Alpha; // declared in material.fxh                                16                      32              32
               vec4 Roughness_WrapL_Edge_Thickness; // declared in material.fxh             16                      48              48
               GLuint64 texHandle; // treated as a uvec2 I think                            8                       64              64
               //GLuint64 paddingHandle

               float alphaTestValue;                                                        4                       72              72
               float padding1;                                                              4                       76              76

               bool hdrEnvTextures;                                                         4                       80              80
               bool objectSpaceNormalMap;                                                   4                       84              84
               bool is_metal;                                                               4                       88              88
               bool doNormalMapping;                                                        4                       92              92


               // STRUCT HAS base alignment of 16, so padding must get it to a multiple of 16

           } MaterialProperties;
        */

        // structure has a base offset and base alignment 
        // STRUCT   base alignment                                      offset          aligned offset
        //          16
        // each member also has a base offset and base alignment



        RenderDevice* const pd3dDevice = g_pplayer->m_pin3d.m_pd3dPrimaryDevice;
        GLuint normalMapName = 0;
        if (normalMap != nullptr) {
            normalMapName = pd3dDevice->m_texMan.LoadTexture(normalMap->m_pdsBuffer, false, false)->texture;
        }

        // default
        // Shader::SetMaterial 
        MaterialProperties shaderMat = {
            glGetTextureHandleARB(texName),
            normalMap != nullptr ? glGetTextureHandleARB(normalMapName) : 0,
            0,
            1.0f / 255.0f,
            0.0f,
            convertColor(0,1.0f),
            convertColor(0,1.0f),
            convertColor(0xB469FF,1.0f),
            vec4(exp2f(10.0f * 0.0f + 1.0f), 0.0f, 1.0f, 0.05f),
            vec4(0.0f,1.0f,0.0f,0.0f), // fDisableLighting_top_below - default y = 1, x not sure, z and w unused
            0.0f,
            0.0f,
            0.0f,
            0.0f
        };


        if (mat) {

            //shaderMat.texHandle = glGetTextureHandleARB(texName);
            //shaderMat.paddingHandle = 0;

            shaderMat.AZDO_alphaTestValue = (1.0 / 255.0); // correct for bumpers for now
            shaderMat.padding1 = 0.0f;

            // /*
            const vec4 cClearcoatF = convertColor(mat->m_cClearcoat, mat->m_fEdgeAlpha);
            shaderMat.AZDO_cClearcoat_EdgeAlpha = cClearcoatF;

            if (!mat->m_bIsMetal) {
                const vec4 cGlossyF = convertColor(mat->m_cGlossy, mat->m_fGlossyImageLerp);
                shaderMat.AZDO_cGlossy_ImageLerp = cGlossyF;
            }
            else {
                shaderMat.AZDO_cGlossy_ImageLerp = convertColor(0, 1.0f);
            }

            const float alpha = mat->m_bOpacityActive ? mat->m_fOpacity : 1.0f;
            const vec4 cBaseF = convertColor(mat->m_cBase, alpha);
            shaderMat.AZDO_cBase_Alpha = cBaseF;

            const vec4 rwem(exp2f(10.0f * mat->m_fRoughness + 1.0f), mat->m_fWrapLighting, mat->m_fEdge, mat->m_fThickness);
            shaderMat.AZDO_Roughness_WrapL_Edge_Thickness = rwem;

            shaderMat.AZDO_fDisableLighting_top_below = mat->m_fDisableLighting_top_below;

            shaderMat.AZDO_hdrEnvTextures = (g_pplayer->m_pin3d.m_envTexture ? g_pplayer->m_pin3d.m_envTexture : &g_pplayer->m_pin3d.envTexture)->IsHDR();
            shaderMat.AZDO_objectSpaceNormalMap = mat->m_objectSpaceNormalMap; // only checked for primitives?
            shaderMat.AZDO_is_metal = mat->m_bIsMetal ? 1.0f : 0.0f;
            shaderMat.AZDO_doNormalMapping = mat->m_bDoNormalMapping; // dummy info
            // */
            //shaderMat.padding2 = 0.0f;
            //shaderMat.padding3 = 0.0f;

            //Shader::SetMaterial;
        }
        /*
        else { // default material

            shaderMat.texHandle = 0;// glGetTextureHandleARB(texName);
            //shaderMat.paddingHandle = 0;

            shaderMat.AZDO_alphaTestValue = (float)(1.0 / 255.0); // correct for bumpers for now
            shaderMat.padding1 = 0.0f;

            // /*
            const vec4 cClearcoatF = convertColor(0, 1.0f);
            shaderMat.AZDO_cClearcoat_EdgeAlpha = cClearcoatF;


            const vec4 cGlossyF = convertColor(0, 1.0f);
            shaderMat.AZDO_cGlossy_ImageLerp = cGlossyF;


            const float alpha = 1.0f;
            const vec4 cBaseF = convertColor(0xB469FF, alpha); // first param:  g_pvp->dummyMaterial.m_cBase is more correct, not accessible here atm though
            shaderMat.AZDO_cBase_Alpha = cBaseF;

            shaderMat.AZDO_Roughness_WrapL_Edge_Thickness = vec4(4.0, 0.5, 1.0, 0.05); // DUMMY VALUE



            shaderMat.AZDO_hdrEnvTextures = 0.0f; // (g_pplayer->m_pin3d.m_envTexture ? g_pplayer->m_pin3d.m_envTexture : &g_pplayer->m_pin3d.envTexture)->IsHDR();
            shaderMat.AZDO_objectSpaceNormalMap = 0.0f; // only checked for primitives?
            shaderMat.AZDO_is_metal = 0.0f;
            shaderMat.AZDO_doNormalMapping = 0.0f; // dummy info
            //
            //shaderMat.padding2 = 0.0f;
            //shaderMat.padding3 = 0.0f;

        }
        */

        glMakeTextureHandleResidentARB(shaderMat.texHandle); // remember to make unresident on app shutdown
        if (normalMap != nullptr) glMakeTextureHandleResidentARB(shaderMat.normalMapHandle); // remember to make unresident on app shutdown

        _allMaterials->push_back(shaderMat);

        BindlessTexture bTex = { 0,0 };
        bTex.name = texName;
        bTex.handle = shaderMat.texHandle;

        _allBindlessTextures.push_back(bTex);

        if (normalMap != nullptr) {
            BindlessTexture nTex = { 0,0 };
            nTex.name = normalMapName;
            nTex.handle = shaderMat.normalMapHandle;

            _allBindlessTextures.push_back(nTex);
        }



    }


    // MATRICES
    ObjMatrices matrices{};
    Matrix3D viewMatrix{}, projMatrix[2]{};

    Shader::GetTransform(TRANSFORMSTATE_VIEW, &viewMatrix, 1);
    Shader::GetTransform(TRANSFORMSTATE_PROJECTION, &projMatrix[0], 2);
    //m_pin3d.m_pd3dPrimaryDevice->basicShaderMultiDraw.GetTransform(TRANSFORMSTATE_VIEW, &viewMatrix, 1);
    //g_pplayer->m_pin3d.m_pd3dPrimaryDevice.basicShaderMultiDraw;//GetTransform();

    if (objWorldMatrix == nullptr) { // identity matrix
        SetWorldMatrix(objWorldMatrix, Vertex3Ds(1.0f, 1.0f, 1.0f), Vertex3Ds(1.0f, 1.0f, 1.0f), Vertex3Ds(1.0f, 1.0f, 1.0f));
    }

    matrices.matWorldView = *objWorldMatrix * viewMatrix;
    //memcpy(matrices.matWorldViewInverseTranspose.m, matrices.matWorldView.m, 4 * 4 * sizeof(float));
    matrices.matWorldViewInverseTranspose = matrices.matWorldView; // a copy
    matrices.matWorldViewInverseTranspose.Invert();
    matrices.matWorldViewInverseTranspose.Transpose();
    matrices.matWorldViewProj[0] = matrices.matWorldView * projMatrix[0];
    matrices.matWorldViewProj[1] = matrices.matWorldView * projMatrix[1];

    _allWorldMatrices->push_back(*objWorldMatrix);
    _allMatrices->push_back(matrices);

}

void Hitable::SetWorldMatrix(Matrix3D* worldMatrix, Vertex3Ds objScaling, Vertex3Ds objRotation, Vertex3Ds objTranslation) {
    Matrix3D orientationMatrix, translationMatrix, rotationMatrix, rotationMatrixX, rotationMatrixY, rotationMatrixZ, scalingMatrix;

    // Matrix3D is column-major?
    // SCALE then ROTATE then TRANSLATE

    scalingMatrix.SetScaling(objScaling.x, objScaling.y, objScaling.z);

    rotationMatrixZ.RotateZMatrix(ANGTORAD(objRotation.z));
    rotationMatrixY.RotateYMatrix(ANGTORAD(objRotation.y));
    rotationMatrixX.RotateXMatrix(ANGTORAD(objRotation.x)); // passing any rotation angle as 0 = identity/no rotation
    //rotationMatrix = rotationMatrixZ * rotationMatrixY * rotationMatrixX;
    rotationMatrix = rotationMatrixX * rotationMatrixY * rotationMatrixZ;

    translationMatrix.SetTranslation(objTranslation);

    //temp = 
    //*worldMatrix = translationMatrix * rotationMatrix * scalingMatrix;

    *worldMatrix = scalingMatrix * rotationMatrix * translationMatrix;

}