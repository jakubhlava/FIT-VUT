/*!
 * @file
 * @brief This file contains phong rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <student/method.hpp>

/// \addtogroup cpu_side Úkoly v cpu části
/// @{

/**
 * @brief This class holds all variables of phong method.
 */
class PhongMethod: public Method{
  public:
    PhongMethod();
    virtual ~PhongMethod();
    virtual void onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera) override;
    
    BufferID vertices;
    BufferID indexing;
    BufferID normals;
    VertexPullerID puller;
    ProgramID prg;

    uint32_t verticesCount = 2092*3; 

    /// \todo Zde si vytvořte proměnné, které budete potřebovat (id bufferů, programu, ...)

};

/// @}
