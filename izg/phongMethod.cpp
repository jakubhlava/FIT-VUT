/*!
 * @file
 * @brief This file contains implementation of phong rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/phongMethod.hpp>
#include <student/bunny.hpp>

/** \addtogroup shader_side 06. Implementace vertex/fragment shaderu phongovy metody
 * Vašim úkolem ve vertex a fragment shaderu je transformovat trojúhelníky pomocí view a projekční matice a spočítat phongův osvětlovací model.
 * Vašim úkolem také je správně vypočítat procedurální barvu.
 * Následující obrázek zobrazuje shadery v různém stupni implementace.
 * Horní řáděk zobrazuje procedurální texturu.
 * Prostřední řádek zobrazuje model králička s nanesenou texturou, ale bez použití transformačních matic.
 * Dolní řádek zobrazuje model po použítí transformačních matic.
 *
 * \image html images/texture.svg "Vypočet procedurální textury." width=1000
 *
 * 
 *
 *
 *
 * @{
 */


/**
 * @brief This function represents vertex shader of phong method.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
void phong_VS(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms){
  /// \todo Naimplementujte vertex shader, který transformuje vstupní vrcholy do
  /// clip-space.<br>
  /// <b>Vstupy:</b><br>
  /// Vstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu ve
  /// world-space (vec3) a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).<br>
  /// <b>Výstupy:</b><br>
  /// Výstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu (vec3)
  /// ve world-space a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).
  /// Výstupní vrchol obsahuje pozici a normálu vrcholu proto, že chceme počítat
  /// osvětlení ve world-space ve fragment shaderu.<br>
  /// <b>Uniformy:</b><br>
  /// Vertex shader by měl pro transformaci využít uniformní proměnné obsahující
  /// view a projekční matici.
  /// View matici čtěte z nulté uniformní proměnné a projekční matici
  /// čtěte z první uniformní proměnné.
  /// <br>
  /// Využijte vektorové a maticové funkce.
  /// Nepředávajte si data do shaderu pomocí globálních proměnných.
  /// Vrchol v clip-space by měl být zapsán do proměnné gl_Position ve výstupní
  /// struktuře.
  /// \image html images/vertex_shader_tasks.svg "Vizualizace vstupů a výstupů vertex shaderu" width=1000
   
    glm::mat4 pvMatrix = uniforms.uniform[1].m4 * uniforms.uniform[0].m4;
    glm::vec4 position =  { inVertex.attributes[0].v3, 1.0f };
    outVertex.gl_Position = pvMatrix * position;
    outVertex.attributes[0].v3 = inVertex.attributes[0].v3;
    outVertex.attributes[1].v3 = inVertex.attributes[1].v3;
}

/**
 * @brief This function represents fragment shader of phong method.
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param uniforms uniform variables
 */
void phong_FS(OutFragment& outFragment, InFragment const& inFragment, Uniforms const& uniforms) {
    /// \todo Naimplementujte fragment shader, který počítá phongův osvětlovací
    /// model s phongovým stínováním.<br>
    /// <b>Vstup:</b><br>
    /// Vstupní fragment by měl v nultém fragment atributu obsahovat
    /// interpolovanou pozici ve world-space a v prvním
    /// fragment atributu obsahovat interpolovanou normálu ve world-space.<br>
    /// <b>Výstup:</b><br>
    /// Barvu zapište do proměnné gl_FragColor ve výstupní struktuře.<br>
    /// <b>Uniformy:</b><br>
    /// Pozici kamery přečtěte z uniformní 3 a pozici
    /// světla přečtěte z uniformní 2.
    /// <br>
    /// <br>
    /// Dejte si pozor na velikost normálového vektoru, při lineární interpolaci v
    /// rasterizaci může dojít ke zkrácení.
    /// Zapište barvu do proměnné gl_FragColor ve výstupní struktuře.
    /// Shininess faktor nastavte na 40.f
    /// <br>
    /// <br>
    /// Difuzní barva materiálu (textura) by měla být procedurálně generována.
    /// Textura je složena zde dvou částí: sinusové pruhy a bílý sněhový poprašek.
    /// Textura je zkombinována z těchto dvou částí podle sklonu normály.
    /// V případě, že normála směřuje kolmo vzhůru je textura čistě bílá.
    /// V případě, že normála směřuje vodorovně nebo dolů je textura složena ze sinusových pruhů.
    /// Bílá textura a textura sinusových pruhů je lineráně míchana pomocí interpolačního parameteru t.
    /// Interpolační parameter t spočtěte z y komponenty normály pomocí t = y*y (samozřejmě s ohledem na negativní čísla).
    /// 
    /// \image html images/texture_combination.svg "Vizualizace kombinace textury sinusových pruhů a sněhového poprašku." width=600
    /// <br>
    /// <br>
    /// Sinusové pruhy jsou naneseny v rovině xy pomocí planárního nanášení textur, viz následující obrázek:
    /// \image html images/texture_plane.svg "Vizualizace nanášení textury sinusových pruhů." width=600
    ///
    /// Sinusová textura obsahuje 10 pruhů 5 zelených (0,0.5,0) a pět žlutých (1,1,0).
    /// Pruhy mají stejnou tloušku.
    /// Textura má velikost 1x1 a opakuje se (platí i pro záporné souřadnice).
    /// Pruhy jsou zvlněny pomocí funkce sinus.
    /// Amplituda funkce sinus je 0.1.
    /// Perioda je nastavena na \f$ \frac{2 \cdot \pi}{10} \f$.
    /// Výchylku pruhu lze tedy spočítat jako \f$ \frac{sin(y \cdot 10)}{10} \f$
    /// Tvorba textury je znázorněna na následujícím obrázku:
    ///
    /// \image html images/stripes_texture.svg "Vizualizace tvorby sinusové textury" width=600
    /// 
    /// 
    /// Spekulární barvu materiálu nastavte na čistou bílou.
    /// Barvu světla nastavte na bílou.
    /// Nepoužívejte ambientní světlo.<br>
    ///
    /// \image html images/fragment_shader_tasks.svg "Vizualizace výpočtu ve fragment shaderu" width=1000

    glm::vec3 fragLight = uniforms.uniform[2].v3; // pozice světla
    glm::vec3 viewPos = uniforms.uniform[3].v3; // pozice kamery
    glm::vec3 normal = inFragment.attributes[1].v3; // normála bodu
    glm::vec3 origPos = inFragment.attributes[0].v3;
     
    // normalizace vektorů
    fragLight = glm::normalize(fragLight - origPos);
    viewPos = glm::normalize(viewPos - origPos);
    normal = glm::normalize(normal);
    float t = normal.y * normal.y;
    /*if (normal.y < 0) {
        t = -t;
    }*/

    float x = origPos[0];
    float y = origPos[1];

    // texture
    glm::vec4 textureColor = { 0.f, 0.f, 0.f, 1.f };

    float basicSin = 0.1f*glm::sin((-y) * 10.f);

    float position = x - basicSin;
    int sw = 0;
    if (position < 0) sw = 1;
    float parameter = position*10;
    if (glm::abs(fmod(parameter,2.0f)) < 1.0f) {
        if (sw) {
            textureColor = (1.f - t) * glm::vec4(1.f, 1.0f, 0.f, 1.f) + t;
        }
        else {
            textureColor = (1.f - t) * glm::vec4(0.f, 0.5f, 0.f, 1.f) + t;
        }
    }
    else {
        if (sw) {
            textureColor = (1.f - t) * glm::vec4(0.f, 0.5f, 0.f, 1.f) + t;
        }
        else
        {
            textureColor = (1.f - t) * glm::vec4(1.f, 1.0f, 0.f, 1.f) + t;
        }
    }

    float diffuse = glm::dot(fragLight, normal);

    glm::vec3 reflection = glm::normalize(2.f * glm::dot(normal, fragLight) * normal - fragLight);
    float specular;
    
    if (glm::dot(viewPos, normal) >= 0) {
        specular = glm::pow(glm::dot(reflection, viewPos), 40.f);
    }
    else {
        specular = 0;
    }


    if (diffuse <= 0.f) {
        diffuse = 0.f;
        specular = 0.f;
    }



    // glm::vec4 color = textureColor + glm::vec4(diffuse*t) + glm::vec4(specular);

     //glm::vec4 color = textureColor + glm::vec4(light * t);

     //glm::vec4 color = glm::abs((glm::dot(fragLight, normal) + glm::pow(glm::dot(reflection, viewPos), 40.f)))*textureColor + glm::vec4(light * t) + specular;
    glm::vec4 color = textureColor;
    //glm::vec4 color = specular + diffuse * textureColor;
    for (int i = 0; i < 4; i++) {
        color[i] *= diffuse;
        color[i] += specular;
        if (color[i] > 1.f) {
            color[i] = 1.f;
        }
        else if (color[i] < 0.f) {
            color[i] = 0.f; 
        }
    }
    /*if (diffuse > 0.005f) {
        color += (1 - t);
    }*/

    //color = glm::vec4(phong * t);
    //color += textureColor*phong;
    /*
    printf("InFrag defaultpos: %f, %f, %f\n", origPos[0], origPos[1], origPos[2]);
    printf("InFrag calcpos: %f, %f\n", x, y);
    printf("Calculated color: %f, %f, %f\n", color[0], color[1], color[2]);
    //*/
    

    outFragment.gl_FragColor = color;


}

/// @}

/** \addtogroup cpu_side 07. Implementace vykreslení králička s phongovým osvětlovacím modelem.
 * Vaším úkolem je využít naimplementovanou grafickou kartu a vykreslit králička s phongovým osvětlovacím modelem a stínováním a procedurální texturou.
 * @{
 */


/**
 * @brief Constructoro f phong method
 */
PhongMethod::PhongMethod(){
/// \todo Doprogramujte inicializační funkci.
/// Zde byste měli vytvořit buffery na GPU, nahrát data do bufferů, vytvořit
/// vertex puller a správně jej nakonfigurovat, vytvořit program, připojit k
/// němu shadery a nastavit atributy, které se posílají mezi vs a fs.
/// Do bufferů nahrajte vrcholy králička (pozice, normály) a indexy na vrcholy
/// ze souboru \ref bunny.hpp.
/// Shader program by měl odkazovat na funkce/shadery phong_VS a phong_FS.
/// V konfiguraci vertex pulleru nastavte dvě čtecí hlavy.
/// Jednu pro pozice vrcholů a druhou pro normály vrcholů.
/// Nultý vertex/fragment atribut by měl obsahovat pozici vertexu.
/// První vertex/fragment atribut by měl obsahovat normálu vertexu.
/// Nastavte, které atributy (jaký typ) se posílají z vertex shaderu do fragment shaderu.
/// <b>Seznam funkcí, které jistě využijete:</b>
///  - gpu.createBuffer()
///  - gpu.setBufferData()
///  - gpu.createVertexPuller()
///  - gpu.setVertexPullerIndexing()
///  - gpu.setVertexPullerHead()
///  - gpu.enableVertexPullerHead()
///  - gpu.createProgram()
///  - gpu.attachShaders()
///  - gpu.setVS2FSType()

    vector<float> verticesList;
    vector<float> normalsList;

    for (auto bv : bunnyVertices) {
        for (int i = 0; i < 3; i++) {
            verticesList.push_back(bv.position[i]);
            normalsList.push_back(bv.normal[i]);
        }
    }

    size_t verticesBufSize = sizeof(decltype(verticesList)::value_type) * verticesList.size(); 
    vertices = gpu.createBuffer(verticesBufSize);
    size_t normalsBufSize = sizeof(decltype(normalsList)::value_type) * normalsList.size();
    normals = gpu.createBuffer(normalsBufSize);
    size_t indicesBufSize = sizeof(bunnyIndices);
    indexing = gpu.createBuffer(indicesBufSize);
    
    
    gpu.setBufferData(vertices, 0, verticesBufSize, verticesList.data());
    gpu.setBufferData(normals, 0, normalsBufSize, normalsList.data());
    gpu.setBufferData(indexing, 0, indicesBufSize, bunnyIndices);

    puller = gpu.createVertexPuller();
    gpu.setVertexPullerHead(puller, 0, AttributeType::VEC3, sizeof(glm::vec3), 0, vertices);
    gpu.enableVertexPullerHead(puller, 0);

    gpu.setVertexPullerHead(puller, 1, AttributeType::VEC3, sizeof(glm::vec3), 0, normals);
    gpu.enableVertexPullerHead(puller, 1);

    gpu.setVertexPullerIndexing(puller, IndexType::UINT32, indexing);

    prg = gpu.createProgram();
    gpu.attachShaders(prg, phong_VS, phong_FS);
    gpu.setVS2FSType(prg, 0, AttributeType::VEC3);
    gpu.setVS2FSType(prg, 1, AttributeType::VEC3);

}


/**
 * @brief This function draws phong method.
 *
 * @param proj projection matrix
 * @param view view matrix
 * @param light light position
 * @param camera camera position
 */
void PhongMethod::onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){

/// \todo Doprogramujte kreslící funkci.
/// Zde byste měli aktivovat shader program, aktivovat vertex puller, nahrát
/// data do uniformních proměnných a
/// vykreslit trojúhelníky pomocí funkce \ref GPU::drawTriangles.
/// Data pro uniformní proměnné naleznete v parametrech této funkce.
/// <b>Seznam funkcí, které jistě využijete:</b>
///  - gpu.bindVertexPuller()
///  - gpu.useProgram()
///  - gpu.programUniformMatrix4f()
///  - gpu.programUniform3f      ()
///  - gpu.drawTriangles()
///  - gpu.unbindVertexPuller()

  gpu.clear(.5f,.5f,.5f,1.f);
  gpu.bindVertexPuller(puller);
  gpu.useProgram(prg);
  gpu.programUniformMatrix4f(prg, 0, view);
  gpu.programUniformMatrix4f(prg, 1, proj);
  gpu.programUniform3f(prg, 2, light);
  gpu.programUniform3f(prg, 3, camera);

  gpu.drawTriangles(verticesCount);

  gpu.unbindVertexPuller();
  
}

/**
 * @brief Destructor of phong method.
 */
PhongMethod::~PhongMethod(){
  ///\todo Zde uvolněte alokované zdroje
  /// <b>Seznam funkcí</b>
  ///  - gpu.deleteProgram()
  ///  - gpu.deleteVertexPuller()
  ///  - gpu.deleteBuffer()
    
    gpu.deleteProgram(prg);
    gpu.deleteVertexPuller(puller);
    gpu.deleteBuffer(vertices);
    gpu.deleteBuffer(normals);
    gpu.deleteBuffer(indexing);
    

}

/// @}
