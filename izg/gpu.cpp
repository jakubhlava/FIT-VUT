/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>



/// \addtogroup gpu_init
/// @{

/**
 * @brief Constructor of GPU
 */
GPU::GPU(){
  /// \todo Zde můžete alokovat/inicializovat potřebné proměnné grafické karty
}

/**
 * @brief Destructor of GPU
 */
GPU::~GPU(){
    //printf("destructor\n");
    GPU::deleteFramebuffer();
    //printf("deleted framebuffer\n");
    for (auto b : validBIDs) {   
        //printf("deleting buffer %i\n", b);
        GPU::deleteBuffer(b);
    }
    for (auto p : validPIDs) {
        //printf("deleting program %i\n", p);
        GPU::deleteProgram(p);
    }
    for (auto vp : validVPIDs) {
        //printf("deleting vertexpuller %i\n", vp);
        GPU::deleteVertexPuller(vp);
    }
  /// \todo Zde můžete dealokovat/deinicializovat grafickou kartu
}

/// @}

/** \addtogroup buffer_tasks 01. Implementace obslužných funkcí pro buffery
 * @{
 */

/**
 * @brief This function allocates buffer on GPU.
 *
 * @param size size in bytes of new buffer on GPU.
 *
 * @return unique identificator of the buffer
 */
BufferID GPU::createBuffer(uint64_t size) { 
  /// \todo Tato funkce by měla na grafické kartě vytvořit buffer dat.<br>
  /// Velikost bufferu je v parameteru size (v bajtech).<br>
  /// Funkce by měla vrátit unikátní identifikátor identifikátor bufferu.<br>
  /// Na grafické kartě by mělo být možné alkovat libovolné množství bufferů o libovolné velikosti.<br>
    ObjectID newID = 0x0;
    while (isBuffer(newID) && newID != emptyID) {
        newID++;
    }
    if (newID != emptyID) {
        byte* ptr = new byte[size];
        BufferList[newID] = ptr;
        validBIDs.push_back(newID);
        return newID;
    }
    else {
        return emptyID;
    }
}

/**
 * @brief This function frees allocated buffer on GPU.
 *
 * @param buffer buffer identificator
 */
void GPU::deleteBuffer(BufferID buffer) {
  /// \todo Tato funkce uvolní buffer na grafické kartě.
  /// Buffer pro smazání je vybrán identifikátorem v parameteru "buffer".
  /// Po uvolnění bufferu je identifikátor volný a může být znovu použit při vytvoření nového bufferu.
    if (isBuffer(buffer)) {
        delete[] BufferList[buffer];
        BufferList.erase(buffer);
        validBIDs.erase(remove(validBIDs.begin(), validBIDs.end(), buffer), validBIDs.end());
    }
}

/**
 * @brief This function uploads data to selected buffer on the GPU
 *
 * @param buffer buffer identificator
 * @param offset specifies the offset into the buffer's data
 * @param size specifies the size of buffer that will be uploaded
 * @param data specifies a pointer to new data
 */
void GPU::setBufferData(BufferID buffer, uint64_t offset, uint64_t size, void const* data) {
  /// \todo Tato funkce nakopíruje data z cpu na "gpu".<br>
  /// Data by měla být nakopírována do bufferu vybraného parametrem "buffer".<br>
  /// Parametr size určuje, kolik dat (v bajtech) se překopíruje.<br>
  /// Parametr offset určuje místo v bufferu (posun v bajtech) kam se data nakopírují.<br>
  /// Parametr data obsahuje ukazatel na data na cpu pro kopírování.<br>
    if (isBuffer(buffer)) {
        memcpy(BufferList[buffer] + offset, (byte*)data, size);
    }
}

/**
 * @brief This function downloads data from GPU.
 *
 * @param buffer specfies buffer
 * @param offset specifies the offset into the buffer from which data will be returned, measured in bytes. 
 * @param size specifies data size that will be copied
 * @param data specifies a pointer to the location where buffer data is returned. 
 */
void GPU::getBufferData(BufferID buffer,
                        uint64_t offset,
                        uint64_t size,
                        void*    data)
{
  /// \todo Tato funkce vykopíruje data z "gpu" na cpu.
  /// Data by měla být vykopírována z bufferu vybraného parametrem "buffer".<br>
  /// Parametr size určuje kolik dat (v bajtech) se překopíruje.<br>
  /// Parametr offset určuje místo v bufferu (posun v bajtech) odkud se začne kopírovat.<br>
  /// Parametr data obsahuje ukazatel, kam se data nakopírují.<br>
    if (isBuffer(buffer)) {

        //printf("Trying to read %i bytes of data from buffer %i with offset %u.\n",size,buffer,offset);
        memcpy(data, BufferList[buffer] + offset, size);
    }
}

/**
 * @brief This function tests if buffer exists
 *
 * @param buffer selected buffer id
 *
 * @return true if buffer points to existing buffer on the GPU.
 */
bool GPU::isBuffer(BufferID buffer) { 
  /// \todo Tato funkce by měla vrátit true pokud buffer je identifikátor existující bufferu.<br>
  /// Tato funkce by měla vrátit false, pokud buffer není identifikátor existujícího bufferu. (nebo bufferu, který byl smazán).<br>
  /// Pro emptyId vrací false.<br>
    if (buffer == emptyID)
        return false;
    if (BufferList.count(buffer) == 1)
        return true;
    else
        return false;
     
}

/// @}

/**
 * \addtogroup vertexpuller_tasks 02. Implementace obslužných funkcí pro vertex puller
 * @{
 */

/**
 * @brief This function creates new vertex puller settings on the GPU,
 *
 * @return unique vertex puller identificator
 */
ObjectID GPU::createVertexPuller     (){
  /// \todo Tato funkce vytvoří novou práznou tabulku s nastavením pro vertex puller.<br>
  /// Funkce by měla vrátit identifikátor nové tabulky.
  /// Prázdná tabulka s nastavením neobsahuje indexování a všechny čtecí hlavy jsou vypnuté.
    ObjectID newID = 0x0;
    while (isVertexPuller(newID) && newID != emptyID) {
        newID++;
    }
    if (newID != emptyID) {
        VPSettingsTable *table = new VPSettingsTable;
        VertexPullerList[newID] = table;
        validVPIDs.push_back(newID);
        return newID;
    }
    else {
        return emptyID;
    }
}

/**
 * @brief This function deletes vertex puller settings
 *
 * @param vao vertex puller identificator
 */
void     GPU::deleteVertexPuller     (VertexPullerID vao){
  /// \todo Tato funkce by měla odstranit tabulku s nastavení pro vertex puller.<br>
  /// Parameter "vao" obsahuje identifikátor tabulky s nastavením.<br>
  /// Po uvolnění nastavení je identifiktátor volný a může být znovu použit.<br>
    if (isVertexPuller(vao)) {
        delete VertexPullerList[vao];
        VertexPullerList.erase(vao);
        validVPIDs.erase(remove(validVPIDs.begin(), validVPIDs.end(), vao), validVPIDs.end());
    }
}

/**
 * @brief This function sets one vertex puller reading head.
 *
 * @param vao identificator of vertex puller
 * @param head id of vertex puller head
 * @param type type of attribute
 * @param stride stride in bytes
 * @param offset offset in bytes
 * @param buffer id of buffer
 */
void     GPU::setVertexPullerHead    (VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer){
  /// \todo Tato funkce nastaví jednu čtecí hlavu vertex pulleru.<br>
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "head" vybírá čtecí hlavu vybraného vertex pulleru.<br>
  /// Parametr "type" nastaví typ atributu, který čtecí hlava čte. Tímto se vybere kolik dat v bajtech se přečte.<br>
  /// Parametr "stride" nastaví krok čtecí hlavy.<br>
  /// Parametr "offset" nastaví počáteční pozici čtecí hlavy.<br>
  /// Parametr "buffer" vybere buffer, ze kterého bude čtecí hlava číst.<br>
    if (isVertexPuller(vao)) {
        VertexPullerList[vao]->settings[head].bid = buffer;
        VertexPullerList[vao]->settings[head].offset = offset;
        VertexPullerList[vao]->settings[head].stride = stride;
        VertexPullerList[vao]->settings[head].size = type;
    }
}

/**
 * @brief This function sets vertex puller indexing.
 *
 * @param vao vertex puller id
 * @param type type of index
 * @param buffer buffer with indices
 */
void     GPU::setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer){
  /// \todo Tato funkce nastaví indexování vertex pulleru.
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "type" volí typ indexu, který je uložený v bufferu.<br>
  /// Parametr "buffer" volí buffer, ve kterém jsou uloženy indexy.<br>
    if (isVertexPuller(vao)) {
        VertexPullerList[vao]->indexSettings.type = type;
        VertexPullerList[vao]->indexSettings.bid = buffer;
        VertexPullerList[vao]->indexSettings.enabled = true;
    }
}

/**
 * @brief This function enables vertex puller's head.
 *
 * @param vao vertex puller 
 * @param head head id
 */
void     GPU::enableVertexPullerHead (VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce povolí čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava povolena, hodnoty z bufferu se budou kopírovat do atributu vrcholů vertex shaderu.<br>
  /// Parametr "vao" volí tabulku s nastavením vertex pulleru (vybírá vertex puller).<br>
  /// Parametr "head" volí čtecí hlavu.<br>
    if (isVertexPuller(vao)) {
        VertexPullerList[vao]->settings[head].enabled = true;
        VertexPullerList[vao]->enabledHeads.push_back(head);
    }
}

/**
 * @brief This function disables vertex puller's head
 *
 * @param vao vertex puller id
 * @param head head id
 */
void     GPU::disableVertexPullerHead(VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce zakáže čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava zakázána, hodnoty z bufferu se nebudou kopírovat do atributu vrcholu.<br>
  /// Parametry "vao" a "head" vybírají vertex puller a čtecí hlavu.<br>
    if (isVertexPuller(vao)) {
        VertexPullerList[vao]->settings[head].enabled = false;
    }
}

/**
 * @brief This function selects active vertex puller.
 *
 * @param vao id of vertex puller
 */
void     GPU::bindVertexPuller       (VertexPullerID vao){
  /// \todo Tato funkce aktivuje nastavení vertex pulleru.<br>
  /// Pokud je daný vertex puller aktivován, atributy z bufferů jsou vybírány na základě jeho nastavení.<br>
    if (isVertexPuller(vao)) {
        BoundPuller = vao;
        VertexPullerList[BoundPuller]->counter = 0;
    }
}

/**
 * @brief This function deactivates vertex puller.
 */
void     GPU::unbindVertexPuller     (){
  /// \todo Tato funkce deaktivuje vertex puller.
  /// To většinou znamená, že se vybere neexistující "emptyID" vertex puller.
    BoundPuller = emptyID;
}

/**
 * @brief This function tests if vertex puller exists.
 *
 * @param vao vertex puller
 *
 * @return true, if vertex puller "vao" exists
 */
bool     GPU::isVertexPuller         (VertexPullerID vao){
  /// \todo Tato funkce otestuje, zda daný vertex puller existuje.
  /// Pokud ano, funkce vrací true.
    if (vao == emptyID)
        return false;
    if (VertexPullerList.count(vao) == 1)
        return true;
    else
        return false;
  
}

/// @}

/** \addtogroup program_tasks 03. Implementace obslužných funkcí pro shader programy
 * @{
 */

/**
 * @brief This function creates new shader program.
 *
 * @return shader program id
 */
ProgramID        GPU::createProgram         (){
  /// \todo Tato funkce by měla vytvořit nový shader program.<br>
  /// Funkce vrací unikátní identifikátor nového proramu.<br>
  /// Program je seznam nastavení, které obsahuje: ukazatel na vertex a fragment shader.<br>
  /// Dále obsahuje uniformní proměnné a typ výstupních vertex attributů z vertex shaderu, které jsou použity pro interpolaci do fragment atributů.<br>
    ObjectID newID = 0x0;
    while (isProgram(newID) && newID != emptyID) {
        newID++;
    }
    if (newID != emptyID) {
        ProgramSettings* settings = new ProgramSettings;
        fill(begin(settings->outVsAttrs), end(settings->outVsAttrs), AttributeType::EMPTY);
        settings->fs = nullptr;
        settings->vs = nullptr;
        ProgramList[newID] = settings;
        validPIDs.push_back(newID);
        return newID;
    }
    else {
        return emptyID;
    }
}

/**
 * @brief This function deletes shader program
 *
 * @param prg shader program id
 */
void             GPU::deleteProgram         (ProgramID prg){
  /// \todo Tato funkce by měla smazat vybraný shader program.<br>
  /// Funkce smaže nastavení shader programu.<br>
  /// Identifikátor programu se stane volným a může být znovu využit.<br>
    if (isProgram(prg)) {
        delete ProgramList[prg];
        ProgramList.erase(prg);
        validPIDs.erase(remove(validPIDs.begin(), validPIDs.end(), prg), validPIDs.end());
    }
}

/**
 * @brief This function attaches vertex and frament shader to shader program.
 *
 * @param prg shader program
 * @param vs vertex shader 
 * @param fs fragment shader
 */
void             GPU::attachShaders         (ProgramID prg,VertexShader vs,FragmentShader fs){
  /// \todo Tato funkce by měla připojít k vybranému shader programu vertex a fragment shader.
    if (isProgram(prg)) {
        ProgramList[prg]->vs = vs;
        ProgramList[prg]->fs = fs;
    }
}

/**
 * @brief This function selects which vertex attributes should be interpolated during rasterization into fragment attributes.
 *
 * @param prg shader program
 * @param attrib id of attribute
 * @param type type of attribute
 */
void             GPU::setVS2FSType          (ProgramID prg,uint32_t attrib,AttributeType type){
  /// \todo tato funkce by měla zvolit typ vertex atributu, který je posílán z vertex shaderu do fragment shaderu.<br>
  /// V průběhu rasterizace vznikají fragment.<br>
  /// Fragment obsahují fragment atributy.<br>
  /// Tyto atributy obsahují interpolované hodnoty vertex atributů.<br>
  /// Tato funkce vybere jakého typu jsou tyto interpolované atributy.<br>
  /// Bez jakéhokoliv nastavení jsou atributy prázdne AttributeType::EMPTY<br>
    if (isProgram(prg)) {
        ProgramList[prg]->outVsAttrs[attrib] = type;
    }
}

/**
 * @brief This function actives selected shader program
 *
 * @param prg shader program id
 */
void             GPU::useProgram            (ProgramID prg){
  /// \todo tato funkce by měla vybrat aktivní shader program.
    if (isProgram(prg)) {
        ActiveProgram = prg;
    }
}

/**
 * @brief This function tests if selected shader program exists.
 *
 * @param prg shader program
 *
 * @return true, if shader program "prg" exists.
 */
bool             GPU::isProgram             (ProgramID prg){
  /// \todo tato funkce by měla zjistit, zda daný program existuje.<br>
  /// Funkce vráti true, pokud program existuje.<br>
    if (prg == emptyID)
        return false;
    if (ProgramList.count(prg) == 1)
        return true;
    else
        return false;
}

/**
 * @brief This function sets uniform value (1 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform1f      (ProgramID prg,uint32_t uniformId,float     const&d){
  /// \todo tato funkce by měla nastavit uniformní proměnnou shader programu.<br>
  /// Parametr "prg" vybírá shader program.<br>
  /// Parametr "uniformId" vybírá uniformní proměnnou. Maximální počet uniformních proměnných je uložen v programné \link maxUniforms \endlink.<br>
  /// Parametr "d" obsahuje data (1 float).<br>
    if (isProgram(prg)) {
        ProgramList[prg]->uniforms.uniform[uniformId].v1 = d;
    }
}

/**
 * @brief This function sets uniform value (2 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform2f      (ProgramID prg,uint32_t uniformId,glm::vec2 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 2 floaty.
    if (isProgram(prg)) {
        ProgramList[prg]->uniforms.uniform[uniformId].v2 = d;
    }
}

/**
 * @brief This function sets uniform value (3 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform3f      (ProgramID prg,uint32_t uniformId,glm::vec3 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 3 floaty.
    if (isProgram(prg)) {
        ProgramList[prg]->uniforms.uniform[uniformId].v3 = d;
    }
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform4f      (ProgramID prg,uint32_t uniformId,glm::vec4 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 4 floaty.
    if (isProgram(prg)) {
        ProgramList[prg]->uniforms.uniform[uniformId].v4 = d;
    }
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniformMatrix4f(ProgramID prg,uint32_t uniformId,glm::mat4 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává matici 4x4 (16 floatů).
    if (isProgram(prg)) {
        ProgramList[prg]->uniforms.uniform[uniformId].m4 = d;
    }
}

/// @}





/** \addtogroup framebuffer_tasks 04. Implementace obslužných funkcí pro framebuffer
 * @{
 */

/**
 * @brief This function creates framebuffer on GPU.
 *
 * @param width width of framebuffer
 * @param height height of framebuffer
 */
void GPU::createFramebuffer      (uint32_t width,uint32_t height){
  /// \todo Tato funkce by měla alokovat framebuffer od daném rozlišení.<br>
  /// Framebuffer se skládá z barevného a hloukového bufferu.<br>
  /// Buffery obsahují width x height pixelů.<br>
  /// Barevný pixel je složen z 4 x uint8_t hodnot - to reprezentuje RGBA barvu.<br>
  /// Hloubkový pixel obsahuje 1 x float - to reprezentuje hloubku.<br>
  /// Nultý pixel framebufferu je vlevo dole.<br>
    uint8_t* colorBuffer = new uint8_t[4 * (uint64_t)width * (uint64_t)height];
    float* depthBuffer = new float[(uint64_t)width * (uint64_t)height];
    GPUFrameBuffer = new FrameBuffer;
    GPUFrameBuffer->colorBuffer = colorBuffer;
    GPUFrameBuffer->depthBuffer = depthBuffer;
    GPUFrameBuffer->width = width;
    GPUFrameBuffer->height = height;
    clear(0, 0, 0, 0);
}

/**
 * @brief This function deletes framebuffer.
 */
void GPU::deleteFramebuffer      (){
  /// \todo tato funkce by měla dealokovat framebuffer.
    if(GPUFrameBuffer != nullptr) {
        delete[] GPUFrameBuffer->colorBuffer;
        delete[] GPUFrameBuffer->depthBuffer;
        delete GPUFrameBuffer;
        GPUFrameBuffer = nullptr;
    }
}

/**
 * @brief This function resizes framebuffer.
 *
 * @param width new width of framebuffer
 * @param height new heght of framebuffer
 */
void     GPU::resizeFramebuffer(uint32_t width,uint32_t height){
  /// \todo Tato funkce by měla změnit velikost framebuffer.
    deleteFramebuffer();
    createFramebuffer(width, height);
}

/**
 * @brief This function returns pointer to color buffer.
 *
 * @return pointer to color buffer
 */
uint8_t* GPU::getFramebufferColor  (){
  /// \todo Tato funkce by měla vrátit ukazatel na začátek barevného bufferu.<br>
    if (GPUFrameBuffer != nullptr) {
        return GPUFrameBuffer->colorBuffer;
    }
    else
    {
        return nullptr;
    }

}

/**
 * @brief This function returns pointer to depth buffer.
 *
 * @return pointer to dept buffer.
 */
float* GPU::getFramebufferDepth    (){
  /// \todo tato funkce by mla vrátit ukazatel na začátek hloubkového bufferu.<br>
    if (GPUFrameBuffer != nullptr) {
        return GPUFrameBuffer->depthBuffer;
    }
    else
    {
        return nullptr;
    }
}

/**
 * @brief This function returns width of framebuffer
 *
 * @return width of framebuffer
 */
uint32_t GPU::getFramebufferWidth (){
  /// \todo Tato funkce by měla vrátit šířku framebufferu.
    if (GPUFrameBuffer != nullptr) {
        return GPUFrameBuffer->width;
    }
    else 
    {
        return 0;
    }
  
}

/**
 * @brief This function returns height of framebuffer.
 *
 * @return height of framebuffer
 */
uint32_t GPU::getFramebufferHeight(){
  /// \todo Tato funkce by měla vrátit výšku framebufferu.
    if (GPUFrameBuffer != nullptr) {
        return GPUFrameBuffer->height;
    }
    else
    {
        return 0;
    }
}

/// @}

/** \addtogroup draw_tasks 05. Implementace vykreslovacích funkcí
 * Bližší informace jsou uvedeny na hlavní stránce dokumentace.
 * @{
 */

/**
 * @brief This functino clears framebuffer.
 *
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
void            GPU::clear                 (float r,float g,float b,float a){
  /// \todo Tato funkce by měla vyčistit framebuffer.<br>
  /// Barevný buffer vyčistí na barvu podle parametrů r g b a (0 - nulová intenzita, 1 a větší - maximální intenzita).<br>
  /// (0,0,0) - černá barva, (1,1,1) - bílá barva.<br>
  /// Hloubkový buffer nastaví na takovou hodnotu, která umožní rasterizaci trojúhelníka, který leží v rámci pohledového tělesa.<br>
  /// Hloubka by měla být tedy větší než maximální hloubka v NDC (normalized device coordinates).<br>
    uint32_t red, green, blue, alpha;
    if (r > 1)
        red = 255;
    else
        red = 255*r;
    if (g > 1)
        green = 255;
    else
        green = 255*g;
    if (b > 1)
        blue = 255;
    else
        blue = 255*b;
    if (a > 1)
        alpha = 255;
    else
        alpha = 255*a;

    for (uint32_t i = 0; i < getFramebufferHeight() * getFramebufferWidth(); i++) {
        uint32_t pos = i * 4;
        GPUFrameBuffer->colorBuffer[pos] = red;
        GPUFrameBuffer->colorBuffer[pos+1] = green;
        GPUFrameBuffer->colorBuffer[pos+2] = blue;
        GPUFrameBuffer->colorBuffer[pos+3] = alpha;
        GPUFrameBuffer->depthBuffer[i] = 2.0f;
    }

}



void            GPU::drawTriangles(uint32_t  nofVertices) {
    /// \todo Tato funkce vykreslí trojúhelníky podle daného nastavení.<br>
    /// Vrcholy se budou vybírat podle nastavení z aktivního vertex pulleru (pomocí bindVertexPuller).<br>
    /// Vertex shader a fragment shader se zvolí podle aktivního shader programu (pomocí useProgram).<br>
    /// Parametr "nofVertices" obsahuje počet vrcholů, který by se měl vykreslit (3 pro jeden trojúhelník).<br>


    if (!isVertexPuller(BoundPuller)) {
        printf("error: puller not bound\n");
        return;
    }
    if (!isProgram(ActiveProgram)) {
        printf("error: active program is not program\n");
        return;
    }
    if (ProgramList[ActiveProgram]->vs == nullptr || ProgramList[ActiveProgram]->fs == nullptr) {
        printf("error: shaders not attached\n");
        return;
    }
    auto puller = VertexPullerList[BoundPuller];
    uint32_t triangleCount = nofVertices / 3;
    vector<shared_ptr<Primitive>> primitivesList;
    uint8_t indexSize;
    if (puller->indexSettings.enabled) {
        switch (puller->indexSettings.type) {
        case IndexType::UINT8:
            indexSize = 1;
            break;
        case IndexType::UINT16:
            indexSize = 2;
            break;
        case IndexType::UINT32:
            indexSize = 4;
            break;
        }
    }
    for (uint32_t p = 0; p < triangleCount; p++) { // Primitive Assembly 1
        shared_ptr<Primitive> workingPrimitive = make_shared<Primitive>();
        InVertex* inVertexList = new InVertex[3];
        for (uint32_t i = 0; i < 3; i++) { // Vertex Puller   
            if (puller->indexSettings.enabled) {
                inVertexList[i].gl_VertexID = 0;
                uint8_t id8 = 0;
                uint16_t id16 = 0;
                switch (indexSize)
                {
                case 1:
                    getBufferData(puller->indexSettings.bid, puller->counter * indexSize, indexSize, &id8);
                    inVertexList[i].gl_VertexID += id8;
                    break;
                case 2:
                    getBufferData(puller->indexSettings.bid, puller->counter * indexSize, indexSize, &id16);
                    inVertexList[i].gl_VertexID += id16;
                    break;
                case 4:
                    getBufferData(puller->indexSettings.bid, puller->counter * indexSize, indexSize, &inVertexList[i].gl_VertexID);
                    break;
                }

                if (inVertexList[i].gl_VertexID > 4000000000) {
                    printf("Possibly bad ID loaded. %i\n", inVertexList[i].gl_VertexID);
                    printf("Parameters: %i, %i, %i\n", puller->indexSettings.bid, puller->counter * indexSize, indexSize);
                }
            }
            else
            {
                inVertexList[i].gl_VertexID = puller->counter;
            }
            for (auto head : puller->enabledHeads) { // načítání dat z bufferů do atributů vrcholu
                uint64_t offset = puller->settings[head].offset + puller->settings[head].stride * inVertexList[i].gl_VertexID;
                switch (puller->settings[head].size) {
                case AttributeType::FLOAT:
                    getBufferData(puller->settings[head].bid, offset, 4, &inVertexList[i].attributes[head].v1);
                    break;
                case AttributeType::VEC2:
                    getBufferData(puller->settings[head].bid, offset, 8, &inVertexList[i].attributes[head].v2);
                    break;
                case AttributeType::VEC3:
                    getBufferData(puller->settings[head].bid, offset, 12, &inVertexList[i].attributes[head].v3);
                    break;
                case AttributeType::VEC4:
                    getBufferData(puller->settings[head].bid, offset, 16, &inVertexList[i].attributes[head].v4);
                    break;
                }
            }
            puller->counter++;
        }
        OutVertex* outVertexList = new OutVertex[3];
        for (uint32_t i = 0; i < 3; i++) { // Vertex Processor
            ProgramList[ActiveProgram]->vs(outVertexList[i], inVertexList[i], ProgramList[ActiveProgram]->uniforms);
        }
        for (uint8_t i = 0; i < 3; i++) {
            workingPrimitive->vertex[i] = outVertexList[i];
        }
        primitivesList.push_back(workingPrimitive);
        delete[] inVertexList;
        delete[] outVertexList;
    }

    // Clipping

    vector<shared_ptr<Primitive>> newPrimitivesList;

    while (!primitivesList.empty()) {
        Primitive primitive = *(primitivesList.back());
        primitivesList.pop_back();
        vector<OutVertex> newVertices;
        vector<pair<OutVertex, int>> clipOut;
        for (int i = 0; i < 3; i++) {
            if (-(primitive.vertex[i].gl_Position[3]) > primitive.vertex[i].gl_Position[2]) {
                clipOut.push_back({ primitive.vertex[i],i });
            }
        }
        switch (clipOut.size()) {
        case 0:
        {
            shared_ptr<Primitive> copyP = make_shared<Primitive>();
            for (int i = 0; i < 3; i++) {
                copyP->vertex[i] = primitive.vertex[i];
            }
            newPrimitivesList.push_back(copyP);
            break;
        }
        case 1:
        {
            int clipped;
            for (int i = 0; i < 3; i++) {
                if (i != clipOut[0].second) {
                    newVertices.push_back(primitive.vertex[i]); // vrchol, který se neořezává, pushnu
                    // vypočítám si parametr
                    float t = (-clipOut[0].first.gl_Position[3] - clipOut[0].first.gl_Position[2]) / (primitive.vertex[i].gl_Position[3] - clipOut[0].first.gl_Position[3] + primitive.vertex[i].gl_Position[2] - clipOut[0].first.gl_Position[2]);                    //printf("t: %f\n", t);
                    OutVertex newVert;
                    // vypočítám si pozici 
                    newVert.gl_Position = clipOut[0].first.gl_Position + t * (primitive.vertex[i].gl_Position - clipOut[0].first.gl_Position);
                    // interpoluji atributy
                    for (int j = 0; j < maxAttributes; j++) {
                        switch (ProgramList[ActiveProgram]->outVsAttrs[j]) {
                        case AttributeType::EMPTY:
                            break;
                        case AttributeType::FLOAT:
                            newVert.attributes[j].v1 = clipOut[0].first.attributes[j].v1 + t * (primitive.vertex[i].attributes[j].v1 - clipOut[0].first.attributes[j].v1);
                            break;
                        case AttributeType::VEC2:
                            newVert.attributes[j].v2 = clipOut[0].first.attributes[j].v2 + t * (primitive.vertex[i].attributes[j].v2 - clipOut[0].first.attributes[j].v2);
                            break;
                        case AttributeType::VEC3:
                            newVert.attributes[j].v3 = clipOut[0].first.attributes[j].v3 + t * (primitive.vertex[i].attributes[j].v3 - clipOut[0].first.attributes[j].v3);
                            break;
                        case AttributeType::VEC4:
                            newVert.attributes[j].v4 = clipOut[0].first.attributes[j].v4 + t * (primitive.vertex[i].attributes[j].v4 - clipOut[0].first.attributes[j].v4);
                            break;
                        }
                    }
                    newVertices.push_back(newVert);
                }
                else {
                    clipped = i;
                }
            }
            newVertices.push_back(newVertices[1]);
            newVertices.push_back(newVertices[2]);
            shared_ptr<Primitive> newP1 = make_shared<Primitive>();
            shared_ptr<Primitive> newP2 = make_shared<Primitive>();

            newP1->vertex[0] = newVertices[0];
            newP1->vertex[1] = newVertices[2];
            newP1->vertex[2] = newVertices[1];

            newP2->vertex[0] = newVertices[3];
            newP2->vertex[1] = newVertices[4];
            newP2->vertex[2] = newVertices[5];

            newPrimitivesList.push_back(newP1);
            newPrimitivesList.push_back(newP2);
            break;
        }
        case 2:
        {
            for (int k = 0; k < 2; k++)
            {
                for (int i = 0; i < 3; i++) {
                    if (i != clipOut[0].second && i != clipOut[1].second) {
                        if (k == 0) {
                            newVertices.push_back(primitive.vertex[i]); // vrchol, který se neořezává, pushnu
                        }
                        // vypočítám si parametr
                        float t = (-clipOut[k].first.gl_Position[3] - clipOut[k].first.gl_Position[2]) / (primitive.vertex[i].gl_Position[3] - clipOut[k].first.gl_Position[3] + primitive.vertex[i].gl_Position[2] - clipOut[k].first.gl_Position[2]);
                        //printf("%f\n", t);
                        OutVertex newVert;
                        // vypočítám si pozici
                        newVert.gl_Position = clipOut[k].first.gl_Position + t * (primitive.vertex[i].gl_Position - clipOut[k].first.gl_Position);
                        // interpoluji atributy
                        for (int j = 0; j < maxAttributes; j++) {
                            switch (ProgramList[ActiveProgram]->outVsAttrs[j]) {
                            case AttributeType::EMPTY:
                                break;
                            case AttributeType::FLOAT:
                                newVert.attributes[j].v1 = clipOut[k].first.attributes[j].v1 + t * (primitive.vertex[i].attributes[j].v1 - clipOut[k].first.attributes[j].v1);
                                break;
                            case AttributeType::VEC2:
                                newVert.attributes[j].v2 = clipOut[k].first.attributes[j].v2 + t * (primitive.vertex[i].attributes[j].v2 - clipOut[k].first.attributes[j].v2);
                                break;
                            case AttributeType::VEC3:
                                newVert.attributes[j].v3 = clipOut[k].first.attributes[j].v3 + t * (primitive.vertex[i].attributes[j].v3 - clipOut[k].first.attributes[j].v3);
                                break;
                            case AttributeType::VEC4:
                                newVert.attributes[j].v4 = clipOut[k].first.attributes[j].v4 + t * (primitive.vertex[i].attributes[j].v4 - clipOut[k].first.attributes[j].v4);
                                break;
                            }
                        }
                        newVertices.push_back(newVert);
                    }
                }


            }
            shared_ptr<Primitive> newP = make_shared<Primitive>();
            for (int i = 0; i < 3; i++) {
                newP->vertex[i] = newVertices[i];
            }
            newPrimitivesList.push_back(newP);
            break;
        }
        case 3:
            break;
        }
    }
 
    // viewport transformace
    // perspektivní dělení
    for (auto primitive : newPrimitivesList) {
        for (int i = 0; i < 3; i++) {
            float w = primitive->vertex[i].gl_Position[3];
            primitive->vertex[i].gl_Position = (primitive->vertex[i].gl_Position / w); // perspective division
            primitive->vertex[i].gl_Position[3] = w;
            primitive->vertex[i].gl_Position[0] = ((float)(primitive->vertex[i].gl_Position[0] + 1) / 2) * GPUFrameBuffer->width;
            primitive->vertex[i].gl_Position[1] = ((float)(primitive->vertex[i].gl_Position[1] + 1) / 2) * GPUFrameBuffer->height;
        }
    }

    // Rasterizace
    vector<shared_ptr<InFragment>> fragmentList;
    while(!newPrimitivesList.empty()) {
        Primitive p = *newPrimitivesList.back();
        newPrimitivesList.pop_back();
        vector<pair<glm::vec4, glm::vec2>> edgeVects; // vektory hran
        for (int i = 0; i < 3; i++) {
            p.vertex[i].gl_Position[0] += 0.5f;
            p.vertex[i].gl_Position[1] += 0.5f;
        }
        for (int i = 0; i < 3; i++) {
            int j;
            if (i == 2)
                j = 0;
            else
                j = i + 1;
            //printf("Edge %i-%i\n", i, j);
            glm::vec2 hi = { p.vertex[i].gl_Position[0] - p.vertex[j].gl_Position[0],p.vertex[i].gl_Position[1] - p.vertex[j].gl_Position[1] };
            edgeVects.push_back({ p.vertex[i].gl_Position, hi });
        }
        uint32_t minX = GPUFrameBuffer->width, minY = GPUFrameBuffer->height, maxX = 0, maxY = 0; // minimální obdélník
        for (int i = 0; i < 3; i++) {
            if (p.vertex[i].gl_Position[0] < minX) minX = p.vertex[i].gl_Position[0];
            if (p.vertex[i].gl_Position[1] < minY) minY = p.vertex[i].gl_Position[1];
            if (p.vertex[i].gl_Position[0] > maxX) maxX = p.vertex[i].gl_Position[0];
            if (p.vertex[i].gl_Position[1] > maxY) maxY = p.vertex[i].gl_Position[1];
        }
        for (uint32_t x = minX; x <= maxX; x++) { // průchod pixelů v minimálním obdélníku
            for (uint32_t y = minY; y <= maxY; y++) {
                float products[3];
                for (int i = 0; i < 3; i++) { // výpočet hranové funkce pinedova algoritmu
                    glm::vec2 sideVect = { x - edgeVects[i].first[0], y - edgeVects[i].first[1] };
                    products[i] = sideVect[0] * edgeVects[i].second[1] - sideVect[1] * edgeVects[i].second[0];
                }
                if (products[0] >= 0 && products[1] >= 0 && products[2] >= 0) { // tvorba infragmentů
                    shared_ptr<InFragment> newFrag = make_shared<InFragment>();
                    float lambda[3];
                    calculateBarycentric(p, x, y, lambda);
                    float z = ((lambda[0] * p.vertex[0].gl_Position[2]) / p.vertex[0].gl_Position[3] + (lambda[1] * p.vertex[0].gl_Position[2]) / p.vertex[1].gl_Position[3] + (lambda[2] * p.vertex[0].gl_Position[2]) / p.vertex[2].gl_Position[3]) / (lambda[0] / p.vertex[0].gl_Position[3] + lambda[1] / p.vertex[1].gl_Position[3] + lambda[2] / p.vertex[2].gl_Position[3]);
                    newFrag->gl_FragCoord = { x-0.5f, y-0.5f, z, 0 };
                    for (int i = 0; i < maxAttributes; i++) {
                        switch (ProgramList[ActiveProgram]->outVsAttrs[i]) {
                            case AttributeType::EMPTY:
                                break;
                            case AttributeType::FLOAT:
                                newFrag->attributes[i].v1 = ((lambda[0] * p.vertex[0].attributes[i].v1) / p.vertex[0].gl_Position[3] + (lambda[1] * p.vertex[1].attributes[i].v1) / p.vertex[1].gl_Position[3] + (lambda[2] * p.vertex[2].attributes[i].v1) / p.vertex[2].gl_Position[3]) / (lambda[0]/ p.vertex[0].gl_Position[3] + lambda[1] / p.vertex[1].gl_Position[3] + lambda[2] / p.vertex[2].gl_Position[3]);
                                break;
                            case AttributeType::VEC2:
                                newFrag->attributes[i].v2 = ((lambda[0] * p.vertex[0].attributes[i].v2) / p.vertex[0].gl_Position[3] + (lambda[1] * p.vertex[1].attributes[i].v2) / p.vertex[1].gl_Position[3] + (lambda[2] * p.vertex[2].attributes[i].v2) / p.vertex[2].gl_Position[3]) / (lambda[0] / p.vertex[0].gl_Position[3] + lambda[1] / p.vertex[1].gl_Position[3] + lambda[2] / p.vertex[2].gl_Position[3]);
                                break;
                            case AttributeType::VEC3:
                                newFrag->attributes[i].v3 = ((lambda[0] * p.vertex[0].attributes[i].v3) / p.vertex[0].gl_Position[3] + (lambda[1] * p.vertex[1].attributes[i].v3) / p.vertex[1].gl_Position[3] + (lambda[2] * p.vertex[2].attributes[i].v3) / p.vertex[2].gl_Position[3]) / (lambda[0] / p.vertex[0].gl_Position[3] + lambda[1] / p.vertex[1].gl_Position[3] + lambda[2] / p.vertex[2].gl_Position[3]);
                                break;
                            case AttributeType::VEC4:
                                newFrag->attributes[i].v4 = ((lambda[0] * p.vertex[0].attributes[i].v4) / p.vertex[0].gl_Position[3] + (lambda[1] * p.vertex[1].attributes[i].v4) / p.vertex[1].gl_Position[3] + (lambda[2] * p.vertex[2].attributes[i].v4) / p.vertex[2].gl_Position[3]) / (lambda[0] / p.vertex[0].gl_Position[3] + lambda[1] / p.vertex[1].gl_Position[3] + lambda[2] / p.vertex[2].gl_Position[3]);
                                break;
                        }
                    }
                    fragmentList.push_back(newFrag);
                }
            }
        }
    }
    
    
    // fragment processor
    vector<pair<shared_ptr<InFragment>, shared_ptr<OutFragment>>> ioFragList;
    while(!fragmentList.empty()) {
        shared_ptr<InFragment> f = make_shared<InFragment>(*fragmentList.back());
        fragmentList.pop_back();
        if (f->gl_FragCoord[2] < -1.f) {
            continue;
        }
        if (f->gl_FragCoord[0] > 0 && f->gl_FragCoord[0] < GPUFrameBuffer->width) {
            if (f->gl_FragCoord[1] > 0 && f->gl_FragCoord[1] < GPUFrameBuffer->height) {
                shared_ptr<OutFragment> outF = make_shared<OutFragment>();
                ProgramList[ActiveProgram]->fs(*outF, *f, ProgramList[ActiveProgram]->uniforms);
                ioFragList.push_back({ f, outF });
            }
        }
    }
    // per-fragment & vykreslení
    while (!ioFragList.empty()) {
        InFragment infrag = *ioFragList.back().first;
        OutFragment outfrag = *ioFragList.back().second;
        ioFragList.pop_back();
        infrag.gl_FragCoord[0] -= 0.5f;
        infrag.gl_FragCoord[1] -= 0.5f;
        if (infrag.gl_FragCoord[0] >= 0 && infrag.gl_FragCoord[0] <= GPUFrameBuffer->width) {
            if (infrag.gl_FragCoord[1] >= 0 && infrag.gl_FragCoord[1] <= GPUFrameBuffer->height) {
                int cpos = 4 * infrag.gl_FragCoord[0] + 4 * GPUFrameBuffer->width * infrag.gl_FragCoord[1];
                int dpos = infrag.gl_FragCoord[0] + GPUFrameBuffer->width * infrag.gl_FragCoord[1];
                if (dpos >= GPUFrameBuffer->width * GPUFrameBuffer->height) {
                    continue;
                }
                if (GPUFrameBuffer->depthBuffer[dpos] > infrag.gl_FragCoord[2]) {
                    for (int i = 0; i < 4; i++) {
                        if (outfrag.gl_FragColor[i] < 0) {
                            outfrag.gl_FragColor[i] = 0.0f;
                        }
                        else if (outfrag.gl_FragColor[i] > 1) {
                            outfrag.gl_FragColor[i] = 1.0f;
                        }
                        GPUFrameBuffer->colorBuffer[cpos + i] = outfrag.gl_FragColor[i] * 255;
                        GPUFrameBuffer->depthBuffer[dpos] = infrag.gl_FragCoord[2];
                    }
                }
            }
        }
        
    }



}

    void GPU::calculateBarycentric(Primitive p, uint32_t x, uint32_t y, float* out) {
        float x1 = p.vertex[0].gl_Position[0];
        float x2 = p.vertex[1].gl_Position[0];
        float x3 = p.vertex[2].gl_Position[0];
        float y1 = p.vertex[0].gl_Position[1];
        float y2 = p.vertex[1].gl_Position[1];
        float y3 = p.vertex[2].gl_Position[1];
        float lambda1 = ((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) / ((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3));
        float lambda2 = ((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) / ((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3));
        float lambda3 = 1 - lambda1 - lambda2;
        out[0] = lambda1;
        out[1] = lambda2;
        out[2] = lambda3;
    }




/// @}



     