/*
#pragma once

// General includes
// ----------------------------------------------------------------
#include <windows.h>
#include <Tchar.h.>
#include <mmsystem.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "assert.h"
#include "stdio.h"

#include <Ogre.h>
#include "SharedData.h"

// Woody include
// ----------------------------------------------------------------
#include "F:\Woody3D Evaluation (64-bit) 1.1.1\c++\woody3d_evaluate_c++\woody3d_evaluate\source\woody3d_evaluate.h"

using namespace Ogre;
using namespace mySharedData;

#define SAFE_RELEASE(p)     if( p){ (p)->Release(); (p)=0;}

class WoodyListener : public FrameListener
{
private:

  // Camera and light
  Camera*             camera;
  //  D3DXVECTOR3           light_0_vector;
  Vector3             light_0_vector;
  //  D3DXVECTOR3           light_0_color;
  ColourValue           light_0_color;
  //  D3DXVECTOR3           ambient_color;
  ColourValue           ambient_color;

  // Woody objects
  wd_tree_object          woody_tree;
  wd_directional_wind_emitter   wind_emitter;
  wd_beaufort_scale_level_s   wind_scale_array[3];
  wd_real_32            tree_lod;
  wd_real_32            tree_y_rotation;

  // Textures, vertex buffers, and shaders
  IDirect3DTexture9*        branch_texture;
  IDirect3DTexture9*        branch_normal_texture;
  IDirect3DTexture9*        imposter_branch_texture;
  IDirect3DTexture9*        imposter_branch_normal_texture;
  IDirect3DTexture9*        leaf_texture;
  IDirect3DTexture9*        leaf_normal_texture;
  IDirect3DVertexBuffer9*     branch_vertex_buffer;
  IDirect3DIndexBuffer9*      branch_index_buffer;

  IDirect3DVertexBuffer9*     imposter_branch_vertex_buffer;
  IDirect3DIndexBuffer9*      imposter_branch_index_buffer;
  IDirect3DVertexBuffer9*     leaf_vertex_buffer;
  IDirect3DIndexBuffer9*      leaf_index_buffer;
  IDirect3DVertexDeclaration9*  branch_vertex_declaration;
  IDirect3DVertexDeclaration9*  leaf_vertex_declaration;

  IDirect3DVertexShader9*     branch_vertex_shader;
  ID3DXConstantTable*       branch_vertex_shader_constant_table;
  IDirect3DVertexShader9*     leaf_vertex_shader;
  ID3DXConstantTable*       leaf_vertex_shader_constant_table;
  IDirect3DPixelShader9*      tree_pixel_shader;
  ID3DXConstantTable*       tree_pixel_shader_constant_table;

  // Timer
  unsigned int          elapsed_time_ms;

  //ogre vars
  MaterialPtr *treeMaterial;

public:

  WoodyListener(Camera *cam)
  {
    camera = cam;
    //    initialize_resources();
    load_tree_mesh((wchar_t*)("E:\my project\Project\LI\Media\woodymedia\trees"));


    // Create tree geometry buffers
    create_tree_geometry_buffers();

    // Seed wind emitter
    wind_emitter.seed(timeGetTime());

    // Set wind emitter scale levels
    wind_scale_array[0].level       = 2;
    wind_scale_array[0].weight        = 0.8f;
    wind_scale_array[0].duration_min_in_ms  = 2500;
    wind_scale_array[0].duration_max_in_ms  = 4500;
    wind_scale_array[1].level       = 3;
    wind_scale_array[1].weight        = 0.8f;
    wind_scale_array[1].duration_min_in_ms  = 2500;
    wind_scale_array[1].duration_max_in_ms  = 4500;
    wind_scale_array[2].level       = 4;
    wind_scale_array[2].weight        = 0.4f;
    wind_scale_array[2].duration_min_in_ms  = 1000;
    wind_scale_array[2].duration_max_in_ms  = 2000;
    wind_emitter.set_wind_scale_levels(wind_scale_array, 3);

    // Set wind direction
    wind_emitter.set_wind_direction(wd_vector2(-1.0f, 0.0f), wd_degree_to_radian(1.0f));

  }

  ~WoodyListener()
  {

  }

  bool frameStarted(const FrameEvent& evt)
  {

    // Local data
    unsigned int      now_time_ms;
    static unsigned int   s_last_time_ms = timeGetTime();
    wd_real_32        wind_strength;
    wd_vector3        wd_camera_position, tree_position, tree_rotation, tree_scaling, wind_direction;
    unsigned int      buffer_size;
    void*         data_pointer;
    D3DXVECTOR3       sprite_center, sprite_position;

    // -----------------

    // Get current time
    now_time_ms = timeGetTime();

    // Store elapsed time
    elapsed_time_ms = now_time_ms - s_last_time_ms;

    // Store last time
    s_last_time_ms = now_time_ms;


    // Update the wind emitter with elapsed time
    wind_emitter.update(elapsed_time_ms);

    // Get current  wind strength
    wind_strength = wind_emitter.get_wind_intensity();

    // Get current wind direction
    wind_emitter.get_wind_direction(wind_direction);

    // Set orientation of tree to vectors
    tree_position = wd_vector3(0.0f, 0.0f, 0.0f);
    tree_rotation = wd_vector3(0.0f, tree_y_rotation, 0.0f);
    tree_scaling  = wd_vector3(1.0f, 1.0f, 1.0f);

    // Set the wind data to the tree
    woody_tree.update_wind(elapsed_time_ms, tree_rotation, wind_direction, wind_strength);

    // Render tree
    // Shader animation and rendering
    //    render_tree_using_gpu_animation();

    return TRUE;
  }

  // Resource functions
  // ----------------------------------------------------------------

  // Create resources and objects
  void initialize_resources()
  {
    / *
    // Local data
    ID3DXBuffer   *shader_code;
    ID3DXBuffer   *compile_errors;
    DWORD     shader_flags;


    shader_code   = 0;
    shader_flags  = 0;
    compile_errors  = 0;

    // Compile the branch vertex shader
    if( FAILED( D3DXCompileShaderFromFile(_T("..\\shared\\shaders\\woody3d_evaluate_branch.vs.hlsl"), 0, 0, "main",
      "vs_2_0", shader_flags, &shader_code,
      &compile_errors, &branch_vertex_shader_constant_table)))
    {
      if(compile_errors)
      {
        LPVOID error_pointer = compile_errors->GetBufferPointer();
        MessageBoxA( 0, (const char*)error_pointer, "Vertex Shader Compile Error", MB_OK|MB_ICONEXCLAMATION);
      }
      else
      {
        MessageBoxA( 0, _T("Vertex Shader Compile Error"), _T("In function initialize_resources(): Failed to load woody_branch.vs.hlsl."));
      }
    }

    // Create the branch vertex shader
    if( FAILED(d3d_device_pointer->CreateVertexShader((DWORD*)shader_code->GetBufferPointer(), &branch_vertex_shader)))
    {
      MessageBoxA( 0, _T("Failed to create branch vertex shader."), _T("Unexpected Error"), MB_OK|MB_ICONEXCLAMATION);
    }
    // Release code buffer
    SAFE_RELEASE( shader_code);

    // Compile the leaf vertex shader
    if( FAILED(D3DXCompileShaderFromFile(_T("..\\shared\\shaders\\woody3d_evaluate_leaf.vs.hlsl"), 0, 0, "main",
      "vs_2_0", shader_flags, &shader_code,
      &compile_errors, &leaf_vertex_shader_constant_table)))
    {
      if( compile_errors)
      {
        LPVOID error_pointer = compile_errors->GetBufferPointer();
        MessageBoxA( 0, (const char*)error_pointer, "Vertex Shader Compile Error", MB_OK|MB_ICONEXCLAMATION);
      }
      else
      {
        MessageBoxA( 0,_T("Vertex Shader Compile Error"), _T("In function initialize_resources(): Failed to load woody_leaf.vs.hlsl."));
      }

    }


    // Create the leaf vertex shader
    if( FAILED(d3d_device_pointer->CreateVertexShader((DWORD*)shader_code->GetBufferPointer(), &leaf_vertex_shader)))
    {
      MessageBoxA( 0, _T("Failed to create leaf vertex shader."), _T("Unexpected Error"), MB_OK|MB_ICONEXCLAMATION);
    }
    // Release code buffer
    SAFE_RELEASE(shader_code);

    // Compile the tree pixel shader
    if( FAILED( D3DXCompileShaderFromFile(_T("..\\shared\\shaders\\woody3d_evaluate.ps.hlsl"), 0, 0, "main",
      "ps_2_0", shader_flags, &shader_code,
      &compile_errors, &tree_pixel_shader_constant_table)))
    {
      if( compile_errors)
      {
        LPVOID error_pointer = compile_errors->GetBufferPointer();
        MessageBoxA( 0, (const char*)error_pointer, "Pixel Shader Compile Error", MB_OK|MB_ICONEXCLAMATION);
      }
      else
      {
        MessageBoxA( 0, _T("Vertex Shader Compile Error"), _T("In function initialize_resources(): Failed to load woody.ps.hlsl."));
      }
      return FALSE;
    }

    // Create the tree pixel shader
    if(FAILED(d3d_device_pointer->CreatePixelShader((DWORD*)shader_code->GetBufferPointer(), &tree_pixel_shader)))
    {
      MessageBoxA( 0, _T("Failed to create tree pixel shader."), _T("Unexpected Error"), MB_OK|MB_ICONEXCLAMATION);
    }
    // Release code buffer
    SAFE_RELEASE(shader_code);
    * /
  }

  // Free resources and objects
  void free_resources()
  {
    SAFE_RELEASE(branch_vertex_shader);
    SAFE_RELEASE(branch_vertex_shader_constant_table);
    SAFE_RELEASE(leaf_vertex_shader);
    SAFE_RELEASE(leaf_vertex_shader_constant_table);
    SAFE_RELEASE(tree_pixel_shader);
    SAFE_RELEASE(tree_pixel_shader_constant_table);
  }

  // Tree creation functions
  // ----------------------------------------------------------------

  // Loads a Woody3D tree mesh from file
  void load_tree_mesh(const wchar_t* filepath)
  {
    // Local data
    wd_tree_material_s  material;

    // Shader animation vertex element descriptors
    // The branch vertex descriptor is used on both standard branches and imposter branches
    wd_ved shader_branch_ved[]  =
    {
      wd_ved(WD_VED_POSITION, 3),    // Vertex position - 3 floats
      wd_ved(WD_VED_COLOR_ARGB, 1),  // Vertex color - 1 float
      wd_ved(WD_VED_TEXCOORD, 2),  // Vertex UV - 2 floats
      wd_ved(WD_VED_NORMAL, 3),    // Vertex Normal - 3 floats
      wd_ved(WD_VED_TANGENT, 4),     // Vertex Tangent - 4 floats
      wd_ved(WD_VED_FWOR, 4)       // Vertex FWOR vector - 4 floats (BRANCH FLEXIBILITY, BRANCH OSCILLATION WEIGHT, BRANCH OSCILLATION PHASE OFFSET, BRANCH OSCILLATION RANGE)
    };

    wd_ved shader_leaf_ved[]  =
    {
      wd_ved(WD_VED_POSITION, 3),  // Vertex position - 3 floats
      wd_ved(WD_VED_COLOR_ARGB, 1),  // Vertex color - 1 float
      wd_ved(WD_VED_TEXCOORD, 2),  // Vertex UV - 2 floats
      wd_ved(WD_VED_LLOD, 1),    // Vertex LLOD - 1 float (LEAF LOD)
      wd_ved(WD_VED_FWOR, 4),    // Vertex FWOR vector - 4 floats (BRANCH FLEXIBILITY, BRANCH OSCILLATION WEIGHT, BRANCH OSCILLATION PHASE OFFSET, BRANCH OSCILLATION RANGE)
      wd_ved(WD_VED_XYOR, 4),    // Vertex XYOR vector - 4 floats (LEAF XY OFFSET, LEAF OSCILLATION PHASE OFFSET, LEAF ROTATION OSCILLATION RANGE)
      wd_ved(WD_VED_CUVA, 4)       // Vertex CUVA vector - 4 floats (LEAF ANIMATION COLUMN COUNT, LEAF ANIMATION UV FRAME SIZE, LEAF ANIMATION FRAME COUNT)
    };

    // Free old tree
    woody_tree.free();

    // Load the mesh file for shader animation
    if(!woody_tree.load_mesh_from_file(0, (wd_uint_16*)filepath,
                                       TRUE,         // Create tree directory
                                       shader_branch_ved, 6, // Branch ved
                                       shader_branch_ved, 6, // Imposter branch ved
                                       shader_leaf_ved, 7,   // Leaf ved
                                       WD_TEXSPACE_DIRECT3D, // Texture space origin
                                       FALSE,          // Is create local animation (software animation)
                                       material))        // Tree material OUT
    {
      MessageBoxA(0, _T("Load Tree Mesh Failed"), _T("In function load_tree_mesh(): Failed to load Woody3D tree mesh from file."), 0);
    }


    // Create textures from the returned material
    if(!create_tree_material_textures(material))
    {
      MessageBoxA(0, _T("Load Tree Material Failed"), _T("In function create_tree_material_textures(): Failed to create Woody3D tree material."), 0);
    }
  }

  // Create textures from the tree material struct
  BOOL create_tree_material_textures(const wd_tree_material_s &material)
  {
    / *    // Local data
        wchar_t     texture_filepath[MAX_PATH], map_relative_path[MAX_PATH];
        const wchar_t*  filename_pointer;


        // Free all old textures
        free_tree_material_textures();

        // Load branch texture
        if( strlen( material.branch_map_path_utf_8) > 0)
        {
          // Convert map relative path from UTF-8 to UTF-16
          MultiByteToWideChar(CP_UTF8, 0, material.branch_map_path_utf_8, -1, map_relative_path, MAX_PATH);
          // Extract filename from material relative path name
          filename_pointer = wcsrchr(map_relative_path, _T('\\')) + 1;
          // Create filepath based on where texture is stored relative to application
          wsprintf( texture_filepath, _T("..\\trees\\%s"), filename_pointer);

          // Load texture
          if(FAILED(D3DXCreateTextureFromFileEx(d3d_device_pointer, texture_filepath,
            D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
            0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
            D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0,
            &branch_texture)))
          {
            MessageBoxA( 0, _T("Load Texture Failed"), _T("In function create_tree_material_textures(): Failed to load branch texture from file."));
            return FALSE;
          }

          // Load branch normal texture
          if( strlen(material.branch_normal_map_path_utf_8) > 0)
          {
            // Convert map relative path from UTF-8 to UTF-16
            MultiByteToWideChar(CP_UTF8, 0, material.branch_normal_map_path_utf_8, -1, map_relative_path, MAX_PATH);
            // Extract filename from material relative path name
            filename_pointer = wcsrchr(map_relative_path, _T('\\')) + 1;
            // Create filepath based on where texture is stored relative to application
            wsprintf(texture_filepath, _T("..\\shared\\trees\\%s"), filename_pointer);

            // Load texture
            if(FAILED(D3DXCreateTextureFromFileEx(d3d_device_pointer, texture_filepath,
              D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
              0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
              D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0,
              &branch_normal_texture)))
            {
              MessageBoxA( 0, _T("Load Texture Failed"), _T("In function create_tree_material_textures(): Failed to load branch normal texture from file."));
              return FALSE;
            }
          }
        }

        // Load imposter branch texture
        if( strlen(material.imposter_branch_map_path_utf_8) > 0)
        {
          // Convert map relative path from UTF-8 to UTF-16
          MultiByteToWideChar(CP_UTF8, 0, material.imposter_branch_map_path_utf_8, -1, map_relative_path, MAX_PATH);
          // Extract filename from material relative path name
          filename_pointer = wcsrchr(map_relative_path, _T('\\')) + 1;
          // Create filepath based on where texture is stored relative to application
          wsprintf( texture_filepath, _T("..\\%s"), filename_pointer);

          // Load texture
          if( FAILED( D3DXCreateTextureFromFileEx(d3d_device_pointer, texture_filepath,
            D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
            0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
            D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0,
            &imposter_branch_texture)))
          {
            MessageBoxA( 0, _T("Load Texture Failed"), _T("In function create_tree_material_textures(): Failed to load imposter branch texture from file."));
            return FALSE;
          }

          // Load branch normal texture
          if( strlen( material.imposter_branch_normal_map_path_utf_8) > 0)
          {
            // Convert map relative path from UTF-8 to UTF-16
            MultiByteToWideChar(CP_UTF8, 0, material.imposter_branch_normal_map_path_utf_8, -1, map_relative_path, MAX_PATH);
            // Extract filename from material relative path name
            filename_pointer = wcsrchr(map_relative_path, _T('\\')) + 1;
            // Create filepath based on where texture is stored relative to application
            wsprintf( texture_filepath, _T("..\\trees\\%s"), filename_pointer);

            // Load texture
            if( FAILED(D3DXCreateTextureFromFileEx(d3d_device_pointer, texture_filepath,
              D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
              0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
              D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0,
              &imposter_branch_normal_texture)))
            {
              MessageBoxA( 0, _T("Load Texture Failed"), _T("In function create_tree_material_textures(): Failed to load imposter branch normal texture from file."));
              return FALSE;
            }
          }
        }

        // Load leaf texture
        if( strlen( material.leaf_map_path_utf_8) > 0)
        {
          // Convert map relative path from UTF-8 to UTF-16
          MultiByteToWideChar(CP_UTF8, 0, material.leaf_map_path_utf_8, -1, map_relative_path, MAX_PATH);
          // Extract filename from material relative path name
          filename_pointer = wcsrchr(map_relative_path, _T('\\')) + 1;
          // Create filepath based on where texture is stored relative to application
          wsprintf( texture_filepath, _T("..\\trees\\%s"), filename_pointer);

          // Load texture
          if( FAILED(D3DXCreateTextureFromFileEx(d3d_device_pointer, texture_filepath,
            D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
            0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
            D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0,
            &leaf_texture)))
          {
            MessageBoxA( 0, _T("Load Texture Failed"), _T("In function create_tree_material_textures(): Failed to load leaf texture from file."));
            return FALSE;
          }

          // Load leaf normal texture
          if(strlen(material.leaf_normal_map_path_utf_8) > 0)
          {
            // Convert map relative path from UTF-8 to UTF-16
            MultiByteToWideChar(CP_UTF8, 0, material.leaf_normal_map_path_utf_8, -1, map_relative_path, MAX_PATH);
            // Extract filename from material relative path name
            filename_pointer = wcsrchr(map_relative_path, _T('\\')) + 1;
            // Create filepath based on where texture is stored relative to application
            wsprintf(texture_filepath, _T("..\\shared\\trees\\%s"), filename_pointer);

            // Load texture
            if(FAILED(D3DXCreateTextureFromFileEx(d3d_device_pointer, texture_filepath,
              D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
              0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
              D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0,
              &leaf_normal_texture)))
            {
              MessageBoxA( 0, _T("Load Texture Failed"), _T("In function create_tree_material_textures(): Failed to load leaf normal texture from file."));
              return FALSE;
            }
          }
        }
        * /
    return TRUE;
  }

  // Release tree material textures
  void free_tree_material_textures()
  {
    SAFE_RELEASE(branch_texture);
    SAFE_RELEASE(branch_normal_texture);
    SAFE_RELEASE(imposter_branch_texture);
    SAFE_RELEASE(imposter_branch_normal_texture);
    SAFE_RELEASE(leaf_texture);
    SAFE_RELEASE(leaf_normal_texture);
  }

  // Create geometry buffers for tree rendering
  BOOL create_tree_geometry_buffers()
  {
    / *    // Local data
        unsigned int          buffer_size;
        void*             data_pointer;


        // Free old geometry buffers
        free_tree_geometry_buffers();

        // Create vertex descriptors for D3D


        // Shader animation
          // Branch vertex
          D3DVERTEXELEMENT9 branch_vd[] =
          {
            //  Stream  Offset         Type                   Method                 Usage          Usage Index
            {     0,      0,    D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,     0      },
            {     0,      12,   D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0      },
            {     0,      16,   D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,     0      },
            {     0,      24,   D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0      },
            {     0,      36,   D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,   1      }, // TANGENT
            {     0,      52,   D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,   2      }, // FWOR
            D3DDECL_END()
          };
          // Leaf vertex
          D3DVERTEXELEMENT9 leaf_vd[] =
          {
            //  Stream  Offset         Type                   Method                 Usage          Usage Index
            {     0,      0,    D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,     0      },
            {     0,      12,   D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0      },
            {     0,      16,   D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,     0      },
            {     0,      24,   D3DDECLTYPE_FLOAT1,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,   1      }, // LLOD
            {     0,      28,   D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,   2      }, // FWOR
            {     0,      44,   D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,   3      }, // XYOR
            {     0,      60,   D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,   4      }, // CUVA
            D3DDECL_END()
          };
          // Create branch vertex declaration
          if( FAILED(d3d_device_pointer->CreateVertexDeclaration(branch_vd, &branch_vertex_declaration)))
          {
            MessageBoxA( 0, _T("Create Vertex Declaration Failed"), _T("In function create_tree_geometry_buffers(): Failed to create branch vertex declaration."));
            return FALSE;
          }
          // Create leaf vertex declaration
          if(FAILED(d3d_device_pointer->CreateVertexDeclaration(leaf_vd, &leaf_vertex_declaration)))
          {
            MessageBoxA( 0, _T("Create Vertex Declaration Failed"), _T("In function create_tree_geometry_buffers(): Failed to create leaf vertex declaration."));
            return FALSE;
          }


        // Set lod of tree to 1.0f to ensure buffers are large enough
        woody_tree.set_lod(1.0f);

        // Create branch geometry buffers
        buffer_size = woody_tree.get_branch_vertex_count() * (woody_tree.get_branch_vertex_stride() * sizeof(wd_real_32));
        if( FAILED(d3d_device_pointer->CreateVertexBuffer(buffer_size,
          D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT,
          &branch_vertex_buffer, 0)))
        {
          MessageBoxA( 0, _T("Create Vertex Buffer Failed"), _T("In function create_tree_geometry_buffers(): Failed to create branch vertex buffer."));
          return FALSE;
        }
        buffer_size = woody_tree.get_branch_index_count() * sizeof(wd_uint_16);
        if(FAILED(d3d_device_pointer->CreateIndexBuffer(buffer_size,
          D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT,
          &branch_index_buffer, 0)))
        {
          MessageBoxA( 0, _T("Create Index Buffer Failed"), _T("In function create_tree_geometry_buffers(): Failed to create branch index buffer."));
          return FALSE;
        }
        // Create imposter branch geometry buffers
        if( woody_tree.get_imposter_branch_vertex_count() > 0)
        {
          buffer_size = woody_tree.get_imposter_branch_vertex_count() * (woody_tree.get_imposter_branch_vertex_stride() * sizeof(wd_real_32));
          if( FAILED(d3d_device_pointer->CreateVertexBuffer(buffer_size,
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT,
            &imposter_branch_vertex_buffer, 0)))
          {
            MessageBoxA( 0, _T("Create Vertex Buffer Failed"), _T("In function create_tree_geometry_buffers(): Failed to create imposter branch vertex buffer."));
            return FALSE;
          }
          buffer_size = woody_tree.get_imposter_branch_index_count() * sizeof(wd_uint_16);
          if( FAILED(d3d_device_pointer->CreateIndexBuffer(buffer_size,
            D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT,
            &imposter_branch_index_buffer, 0)))
          {
            MessageBoxA( 0, _T("Create Index Buffer Failed"), _T("In function create_tree_geometry_buffers(): Failed to create imposter branch index buffer."));
            return FALSE;
          }
        }
        // Create leaf geometry buffers
        if( woody_tree.get_leaf_vertex_count() > 0)
        {
          buffer_size = woody_tree.get_leaf_vertex_count() * (woody_tree.get_leaf_vertex_stride() * sizeof(wd_real_32));
          if(FAILED(d3d_device_pointer->CreateVertexBuffer(buffer_size,
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT,
            &leaf_vertex_buffer, 0)))
          {
            MessageBoxA( 0, _T("Create Vertex Buffer Failed"), _T("In function create_tree_geometry_buffers(): Failed to create leaf vertex buffer."));
            return FALSE;
          }
          buffer_size = woody_tree.get_leaf_index_count() * sizeof(wd_uint_16);
          if(FAILED(d3d_device_pointer->CreateIndexBuffer(buffer_size,
            D3DUSAGE_WRITEONLY | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT,
            &leaf_index_buffer, 0)))
          {
            MessageBoxA( 0, _T("Create Index Buffer Failed"), _T("In function create_tree_geometry_buffers(): Failed to create leaf index buffer."));
            return FALSE;
          }
        }

        // Copy tbe tree geometry into the buffers
        // Branch vertex buffer
        buffer_size = woody_tree.get_branch_vertex_count() * (woody_tree.get_branch_vertex_stride() * sizeof(wd_real_32));
        if(FAILED(branch_vertex_buffer->Lock(0, buffer_size, (void**)&data_pointer, 0)))
        {
          MessageBoxA( 0, _T("Lock Buffer Failed"), _T("In function create_tree_geometry_buffers(): Failed to lock branch vertex buffer."));
          return FALSE;
        }
        memcpy(data_pointer, woody_tree.get_branch_vertex_array(), buffer_size);
        if(FAILED(branch_vertex_buffer->Unlock()))
        {
          MessageBoxA( 0, _T("Unlock Buffer Failed"), _T("In function create_tree_geometry_buffers(): Failed to unlock branch vertex buffer."));
          return FALSE;
        }
        // Branch index buffer
        buffer_size = woody_tree.get_branch_index_count() * sizeof(wd_uint_16);
        if(FAILED(branch_index_buffer->Lock(0, buffer_size, (void**)&data_pointer, 0)))
        {
          MessageBoxA( 0, _T("Lock Buffer Failed"), _T("In function create_tree_geometry_buffers(): Failed to lock branch index buffer."));
          return FALSE;
        }
        memcpy(data_pointer, woody_tree.get_branch_index_array(), buffer_size);
        if(FAILED(branch_index_buffer->Unlock()))
        {
          MessageBoxA( 0, _T("Unlock Buffer Failed"), _T("In function create_tree_geometry_buffers(): Failed to unlock branch index buffer."));
          return FALSE;
        }
        // Imposter branch vertex buffer
        buffer_size = woody_tree.get_imposter_branch_vertex_count() * (woody_tree.get_imposter_branch_vertex_stride() * sizeof(wd_real_32));
        if( buffer_size > 0)
        {
          if( FAILED(imposter_branch_vertex_buffer->Lock(0, buffer_size, (void**)&data_pointer, 0)))
          {
            MessageBoxA( 0, _T("Lock Buffer Failed"), _T("In function create_tree_geometry_buffers(): Failed to lock imposter branch vertex buffer."));
            return FALSE;
          }
          memcpy( data_pointer, woody_tree.get_imposter_branch_vertex_array(), buffer_size);
          if( FAILED( imposter_branch_vertex_buffer->Unlock()))
          {
            MessageBoxA( 0, _T("Unlock Buffer Failed"), _T("In function create_tree_geometry_buffers(): Failed to unlock imposter branch vertex buffer."));
            return FALSE;
          }
          // Imposter branch index buffer
          buffer_size = woody_tree.get_imposter_branch_index_count() * sizeof(wd_uint_16);
          if( FAILED( imposter_branch_index_buffer->Lock(0, buffer_size, (void**)&data_pointer, 0)))
          {
            MessageBoxA( 0, _T("Lock Buffer Failed"), _T("In function create_tree_geometry_buffers(): Failed to lock imposter branch index buffer."));
            return FALSE;
          }
          memcpy( data_pointer, woody_tree.get_imposter_branch_index_array(), buffer_size);
          if( FAILED( imposter_branch_index_buffer->Unlock()))
          {
            MessageBoxA( 0, _T("Unlock Buffer Failed"), _T("In function create_tree_geometry_buffers(): Failed to unlock imposter branch index buffer."));
          }
        }
        // Leaf vertex buffer
        buffer_size = woody_tree.get_leaf_vertex_count() * (woody_tree.get_leaf_vertex_stride() * sizeof(wd_real_32));
        if( buffer_size > 0)
        {
          if( FAILED(leaf_vertex_buffer->Lock(0, buffer_size, (void**)&data_pointer, 0)))
          {
            MessageBoxA( 0, _T("Lock Buffer Failed"), _T("In function create_tree_geometry_buffers(): Failed to lock leaf vertex buffer."));
            return FALSE;
          }
          memcpy( data_pointer, woody_tree.get_leaf_vertex_array(), buffer_size);
          if( FAILED( leaf_vertex_buffer->Unlock()))
          {
            MessageBoxA( 0, _T("Unlock Buffer Failed"), _T("In function create_tree_geometry_buffers(): Failed to unlock leaf vertex buffer."));
            return FALSE;
          }
          // Leaf index buffer
          buffer_size = woody_tree.get_leaf_index_count() * sizeof(wd_uint_16);
          if(FAILED(leaf_index_buffer->Lock(0, buffer_size, (void**)&data_pointer, 0)))
          {
            MessageBoxA( 0, _T("Lock Buffer Failed"), _T("In function create_tree_geometry_buffers(): Failed to lock leaf index buffer."));
            return FALSE;
          }
          memcpy(data_pointer, woody_tree.get_leaf_index_array(), buffer_size);
          if(FAILED(leaf_index_buffer->Unlock()))
          {
            MessageBoxA( 0, _T("Unlock Buffer Failed"), _T("In function create_tree_geometry_buffers(): Failed to unlock leaf index buffer."));
            return FALSE;
          }
        }

        // Set the tree lod to global lod value
        woody_tree.set_lod(tree_lod);
        * /
    return TRUE;

  }

  // Release tree geometry buffers
  void free_tree_geometry_buffers()
  {
    SAFE_RELEASE(branch_vertex_buffer);
    SAFE_RELEASE(branch_index_buffer);
    SAFE_RELEASE(imposter_branch_vertex_buffer);
    SAFE_RELEASE(imposter_branch_index_buffer);
    SAFE_RELEASE(leaf_vertex_buffer);
    SAFE_RELEASE(leaf_index_buffer);
    SAFE_RELEASE(branch_vertex_declaration);
    SAFE_RELEASE(leaf_vertex_declaration);
  }

  // Render functions
  // ----------------------------------------------------------------

  // Render tree with shader animation
  void render_tree_using_gpu_animation()
  {
    / *    // Local data
        wd_vector2      ra_vector, leaf_scale_vector;
        wd_vector3      dul_vector, center_of_leaves;
        wd_vector4      xzip_vector;
        wd_vector3      tree_position, tree_rotation, tree_scaling;
        D3DXMATRIX      world_matrix, translation_matrix, rotation_matrix, scale_matrix,
          view_matrix, projection_matrix, view_projection_matrix,
          world_view_projection_matrix, world_inverse_transpose_matrix;


        // Set orientation of tree to vectors
        tree_position = wd_vector3(0.0f, 0.0f, 0.0f);
        tree_rotation = wd_vector3(0.0f, tree_y_rotation, 0.0f);
        tree_scaling  = wd_vector3(1.0f, 1.0f, 1.0f);

        // Get the woody shader constants
        woody_tree.get_shader_constants(xzip_vector, dul_vector, ra_vector);

        // --------------

        // Create scale matrix
        D3DXMatrixScaling(&scale_matrix, tree_scaling.x, tree_scaling.y, tree_scaling.z);

        // Create rotation matrix using Y rotation only
        D3DXMatrixRotationY(&rotation_matrix, tree_y_rotation);

        // Create translation matrix
        D3DXMatrixTranslation(&translation_matrix, tree_position.x, tree_position.y, tree_position.z);

        // Create world matrix
        world_matrix = scale_matrix * rotation_matrix * translation_matrix;

        // Get view and projection matrices
        d3d_device_pointer->GetTransform( D3DTS_VIEW, &view_matrix);
        d3d_device_pointer->GetTransform( D3DTS_PROJECTION, &projection_matrix);

        // Create view projection
        view_projection_matrix      = view_matrix * projection_matrix;

        // Create world view projection
        world_view_projection_matrix  = world_matrix * view_matrix * projection_matrix;

        // Create world inverse transpose
        D3DXMatrixInverse(&world_inverse_transpose_matrix, 0, &world_matrix);
        D3DXMatrixTranspose(&world_inverse_transpose_matrix, &world_inverse_transpose_matrix);

        // --------------

        // Set branch vertex shader constants
        branch_vertex_shader_constant_table->SetFloatArray(d3d_device_pointer,  "xzip_vector", &xzip_vector.x, 4);

        branch_vertex_shader_constant_table->SetFloatArray(d3d_device_pointer,  "light_vector", &light_0_vector.x, 3);
        branch_vertex_shader_constant_table->SetMatrix(d3d_device_pointer,    "world_view_projection", &world_view_projection_matrix);
        branch_vertex_shader_constant_table->SetMatrix(d3d_device_pointer,    "world_inverse_transpose", &world_inverse_transpose_matrix);
        branch_vertex_shader_constant_table->SetFloat(d3d_device_pointer,   "normal_multiplier", 1.0f);

        // Set branch vertex shader
        d3d_device_pointer->SetVertexShader(branch_vertex_shader);

        // --------------

        // Set pixel shader constants
        tree_pixel_shader_constant_table->SetFloatArray(d3d_device_pointer,   "light_color", &light_0_color.x, 3);
        tree_pixel_shader_constant_table->SetFloatArray(d3d_device_pointer,   "ambient_color", &ambient_color.x, 3);

        // Set the pixel shader
        d3d_device_pointer->SetPixelShader(tree_pixel_shader);

        // --------------

        // If standard branches
        if( woody_tree.get_branch_index_count() > 0)
        {
          set_pixel_shader_texture("diffuse_sampler", tree_pixel_shader_constant_table, branch_texture);
          set_pixel_shader_texture("normal_sampler", tree_pixel_shader_constant_table, branch_normal_texture);
          tree_pixel_shader_constant_table->SetBool(d3d_device_pointer,   "is_translucent", FALSE);

          d3d_device_pointer->SetStreamSource(0, branch_vertex_buffer, 0,
            woody_tree.get_branch_vertex_stride() * sizeof(wd_real_32));
          d3d_device_pointer->SetIndices(branch_index_buffer);
          d3d_device_pointer->SetVertexDeclaration(branch_vertex_declaration);
          d3d_device_pointer->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, woody_tree.get_branch_vertex_count(),
            0, woody_tree.get_branch_index_count() / 3);
        }

        // Enable alpha test
        d3d_device_pointer->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000077);
        d3d_device_pointer->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
        d3d_device_pointer->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

        // If imposter branches
        if( woody_tree.get_imposter_branch_index_count() > 0)
        {
          set_pixel_shader_texture("diffuse_sampler", tree_pixel_shader_constant_table, imposter_branch_texture);
          set_pixel_shader_texture("normal_sampler", tree_pixel_shader_constant_table, imposter_branch_normal_texture);
          tree_pixel_shader_constant_table->SetBool(d3d_device_pointer,   "is_translucent", FALSE);

          d3d_device_pointer->SetStreamSource(0, imposter_branch_vertex_buffer, 0,
            woody_tree.get_imposter_branch_vertex_stride() * sizeof(wd_real_32));
          d3d_device_pointer->SetIndices(imposter_branch_index_buffer);
          d3d_device_pointer->SetVertexDeclaration(branch_vertex_declaration);
          // Render front faces
          d3d_device_pointer->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, woody_tree.get_imposter_branch_vertex_count(),
            0, woody_tree.get_imposter_branch_index_count() / 3);
          // Invert normals and render back faces
          branch_vertex_shader_constant_table->SetFloat(d3d_device_pointer, "normal_multiplier", -1.0f);
          d3d_device_pointer->SetRenderState(D3DRS_CULLMODE,          D3DCULL_CCW);
          d3d_device_pointer->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, woody_tree.get_imposter_branch_vertex_count(),
            0, woody_tree.get_imposter_branch_index_count() / 3);

          // Reset culling and branch normals
          d3d_device_pointer->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW);
        }

        // --------------

        // Get center of leaves
        woody_tree.get_center_of_leaves(center_of_leaves);

        // Create an XY scale vector for leaves
        leaf_scale_vector = wd_vector2(tree_scaling.x, tree_scaling.y);

        // Set leaf vertex shader constants
        leaf_vertex_shader_constant_table->SetFloatArray( d3d_device_pointer, "ra_vector", &ra_vector.x, 2);
        leaf_vertex_shader_constant_table->SetFloatArray( d3d_device_pointer, "dul_vector", &dul_vector.x, 3);
        leaf_vertex_shader_constant_table->SetFloatArray( d3d_device_pointer, "xzip_vector", &xzip_vector.x, 4);

        leaf_vertex_shader_constant_table->SetFloatArray( d3d_device_pointer, "scale_vector", &leaf_scale_vector.x, 2);
        leaf_vertex_shader_constant_table->SetFloatArray( d3d_device_pointer, "eye_position", &camera.get_position().x, 3);
        leaf_vertex_shader_constant_table->SetFloatArray( d3d_device_pointer, "center_of_leaves", &center_of_leaves.x, 3);
        leaf_vertex_shader_constant_table->SetFloatArray( d3d_device_pointer, "light_vector", &light_0_vector.x, 3);
        leaf_vertex_shader_constant_table->SetMatrix( d3d_device_pointer,   "world_matrix", &world_matrix);
        leaf_vertex_shader_constant_table->SetMatrix( d3d_device_pointer,   "view_projection", &view_projection_matrix);

        // Set leaf vertex shader
        d3d_device_pointer->SetVertexShader( leaf_vertex_shader);

        // --------------

        // If leaves
        if( woody_tree.get_leaf_index_count() > 0)
        {
          set_pixel_shader_texture("diffuse_sampler", tree_pixel_shader_constant_table, leaf_texture);
          set_pixel_shader_texture("normal_sampler", tree_pixel_shader_constant_table, leaf_normal_texture);
          tree_pixel_shader_constant_table->SetBool(d3d_device_pointer,   "is_translucent", TRUE);

          d3d_device_pointer->SetStreamSource(0, leaf_vertex_buffer, 0,
            woody_tree.get_leaf_vertex_stride() * sizeof(wd_real_32));
          d3d_device_pointer->SetIndices(leaf_index_buffer);
          d3d_device_pointer->SetVertexDeclaration(leaf_vertex_declaration);
          d3d_device_pointer->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, woody_tree.get_leaf_vertex_count(),
            0, woody_tree.get_leaf_index_count() / 3);
        }

        // Clear textures and shaders
        set_pixel_shader_texture( "diffuse_sampler", tree_pixel_shader_constant_table, 0);
        set_pixel_shader_texture( "normal_sampler", tree_pixel_shader_constant_table, 0);

        d3d_device_pointer->SetVertexShader( 0);
        d3d_device_pointer->SetPixelShader( 0);

        // Disable alpha test
        d3d_device_pointer->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE);
        * /
  }

};*/