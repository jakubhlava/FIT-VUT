/*!
 * @file
 * @brief This file contains class that represents graphic card.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#pragma once

#include <student/fwd.hpp>
#include <map>
#include <algorithm>
#include <vector>
#include <memory>
#include <cstring>

using namespace std;

/**
 * @brief This class represent software GPU
 */
class GPU {
public:
    GPU();
    virtual ~GPU();

    //buffer object commands
    BufferID  createBuffer(uint64_t size);
    void      deleteBuffer(BufferID buffer);
    void      setBufferData(BufferID buffer, uint64_t offset, uint64_t size, void const* data);
    void      getBufferData(BufferID buffer, uint64_t offset, uint64_t size, void* data);
    bool      isBuffer(BufferID buffer);

    //vertex array object commands (vertex puller)
    ObjectID  createVertexPuller();
    void      deleteVertexPuller(VertexPullerID vao);
    void      setVertexPullerHead(VertexPullerID vao, uint32_t head, AttributeType type, uint64_t stride, uint64_t offset, BufferID buffer);
    void      setVertexPullerIndexing(VertexPullerID vao, IndexType type, BufferID buffer);
    void      enableVertexPullerHead(VertexPullerID vao, uint32_t head);
    void      disableVertexPullerHead(VertexPullerID vao, uint32_t head);
    void      bindVertexPuller(VertexPullerID vao);
    void      unbindVertexPuller();
    bool      isVertexPuller(VertexPullerID vao);

    //program object commands
    ProgramID createProgram();
    void      deleteProgram(ProgramID prg);
    void      attachShaders(ProgramID prg, VertexShader vs, FragmentShader fs);
    void      setVS2FSType(ProgramID prg, uint32_t attrib, AttributeType type);
    void      useProgram(ProgramID prg);
    bool      isProgram(ProgramID prg);
    void      programUniform1f(ProgramID prg, uint32_t uniformId, float     const& d);
    void      programUniform2f(ProgramID prg, uint32_t uniformId, glm::vec2 const& d);
    void      programUniform3f(ProgramID prg, uint32_t uniformId, glm::vec3 const& d);
    void      programUniform4f(ProgramID prg, uint32_t uniformId, glm::vec4 const& d);
    void      programUniformMatrix4f(ProgramID prg, uint32_t uniformId, glm::mat4 const& d);

    //framebuffer functions
    void      createFramebuffer(uint32_t width, uint32_t height);
    void      deleteFramebuffer();
    void      resizeFramebuffer(uint32_t width, uint32_t height);
    uint8_t* getFramebufferColor();
    float* getFramebufferDepth();
    uint32_t  getFramebufferWidth();
    uint32_t  getFramebufferHeight();

    //execution commands
    void      clear(float r, float g, float b, float a);
    void      drawTriangles(uint32_t  nofVertices);

    /// \addtogroup gpu_init 00. proměnné, inicializace / deinicializace grafické karty
    /// @{
    /// \todo zde si můžete vytvořit proměnné grafické karty (buffery, programy, ...)
    /// @}


    struct HeadSettings {
        BufferID bid = emptyID;
        uint64_t offset = 0;
        uint64_t stride = 0;
        AttributeType size = AttributeType::EMPTY;
        bool enabled = false;
    };

    struct Indexing {
        BufferID bid = emptyID;
        IndexType type = IndexType::UINT8;
        bool enabled = false;
    };

    struct VPSettingsTable {
        Indexing indexSettings;
        HeadSettings settings[maxAttributes];
        uint64_t counter = 0;
        vector<int> enabledHeads;
    };

    struct ProgramSettings {
        VertexShader vs;
        FragmentShader fs;
        Uniforms uniforms;
        AttributeType outVsAttrs[maxAttributes];
    };

    struct FrameBuffer {
        uint8_t* colorBuffer = nullptr;
        float* depthBuffer = nullptr;
        uint32_t width = 0;
        uint32_t height = 0;
    };

    struct Primitive {
        OutVertex vertex[3];
    };

    struct Edge {
        float dx = 0.0f; // delta x
        float dy = 0.0f; // delta y
        float sx = 0.0f; // starting x
        float sy = 0.0f; // starting y
    };

    map<BufferID, byte*> BufferList;
    map<VertexPullerID, VPSettingsTable*> VertexPullerList;
    map<ProgramID, ProgramSettings*> ProgramList;

    vector<BufferID> validBIDs;
    vector<VertexPullerID> validVPIDs;
    vector<ProgramID> validPIDs;
    
    VertexPullerID BoundPuller = emptyID;
    ProgramID ActiveProgram = emptyID;
    FrameBuffer* GPUFrameBuffer = nullptr;

    void calculateBarycentric(Primitive p, uint32_t x, uint32_t y, float* out);
};


