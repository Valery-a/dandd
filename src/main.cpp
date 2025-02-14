#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <optional>
#include <string>
#include <queue>
#include <set>
#include <random>
#include <array>

static std::array<int,512> g_perm;
void initNoise(unsigned int seed) {
    std::mt19937 rng(seed);
    std::vector<int> p(256);
    for(int i=0;i<256;i++){p[i]=i;}
    for(int i=255;i>0;i--){
        std::uniform_int_distribution<int> dist(0,i);
        int target=dist(rng);
        std::swap(p[i],p[target]);
    }
    for(int i=0;i<512;i++){g_perm[i]=p[i&255];}
}
inline float grad2(int hash,float x,float y){
    int h=hash&7;
    float u=(h<4)?x:y;
    float v=(h<4)?y:x;
    return((h&1)?-u:u)+((h&2)?-2.f*v:2.f*v)*0.5f;
}
float perlin2D(float x,float y){
    int X=(int)std::floor(x)&255;
    int Y=(int)std::floor(y)&255;
    float xf=x-std::floor(x);
    float yf=y-std::floor(y);
    auto fade=[](float t){return t*t*t*(t*(t*6.f-15.f)+10.f);};
    float u=fade(xf);
    float v=fade(yf);
    int aa=g_perm[g_perm[X]+Y];
    int ab=g_perm[g_perm[X]+Y+1];
    int ba=g_perm[g_perm[X+1]+Y];
    int bb=g_perm[g_perm[X+1]+Y+1];
    float x1=grad2(aa,xf,yf);
    float x2=grad2(ba,xf-1,yf);
    float y1=(1.f-u)*x1+u*x2;
    x1=grad2(ab,xf,yf-1);
    x2=grad2(bb,xf-1,yf-1);
    float y2=(1.f-u)*x1+u*x2;
    return((1.f-v)*y1+v*y2);
}
float fbm2D(float x,float y,int octaves,float lacunarity,float gain){
    float freq=1.f;
    float amp=1.f;
    float sum=0.f;
    for(int i=0;i<octaves;i++){
        float val=perlin2D(x*freq,y*freq);
        sum+=val*amp;
        freq*=lacunarity;
        amp*=gain;
    }
    return sum;
}
float getTemperature(int wx,int wz){
    float x=wx*0.001f;
    float z=wz*0.001f;
    float val=fbm2D(x,z,4,2.f,0.5f);
    val=val*0.5f+0.5f;
    return val;
}
float getRainfall(int wx,int wz){
    float x=wx*0.001f+100;
    float z=wz*0.001f+100;
    float val=fbm2D(x,z,4,2.f,0.5f);
    val=val*0.5f+0.5f;
    return val;
}
static const unsigned int SCR_WIDTH=1280;
static const unsigned int SCR_HEIGHT=720;
static const int MAX_CHUNK_GENERATIONS_PER_FRAME=1;
static const int MAX_CHUNK_MESHES_PER_FRAME=2;
static const float CHUNK_UNLOAD_INTERVAL=2.0f;
static const int CHUNK_SIZE=16;
static const float FOG_RANGE=80.0f;
static const int CHUNK_Y_MIN=-1;
static const int CHUNK_Y_MAX=1;
static glm::vec3 cameraPos=glm::vec3(0.0f,16.0f,0.0f);
static glm::vec3 cameraFront=glm::vec3(0.0f,0.0f,-1.0f);
static glm::vec3 cameraUp=glm::vec3(0.0f,1.0f,0.0f);
static float deltaTime=0.0f;
static float lastFrame=0.0f;
static bool firstMouse=true;
static float lastX=SCR_WIDTH/2.0f;
static float lastY=SCR_HEIGHT/2.0f;
static float yaw=-90.0f;
static float pitch=0.0f;
static float cameraBaseSpeed=5.0f;
enum class BlockType: uint8_t{AIR=0,GRASS,DIRT,STONE,SAND,WOOD,LEAF};
enum class BiomeType{DESERT,PLAINS,FOREST,SNOWY,MOUNTAINS,SWAMP};
BiomeType pickBiome(float temperature,float rainfall){
    if(temperature<0.3f){
        if(rainfall>0.5f)return BiomeType::SNOWY;
        else return BiomeType::MOUNTAINS;
    }else if(temperature>0.7f){
        if(rainfall<0.3f)return BiomeType::DESERT;
        else return BiomeType::SWAMP;
    }else{
        if(rainfall<0.4f)return BiomeType::PLAINS;
        else return BiomeType::FOREST;
    }
}
std::string biomeToString(BiomeType b){
    switch(b){
        case BiomeType::DESERT:return "Desert";
        case BiomeType::PLAINS:return "Plains";
        case BiomeType::FOREST:return "Forest";
        case BiomeType::SNOWY:return "Snowy";
        case BiomeType::MOUNTAINS:return "Mountains";
        case BiomeType::SWAMP:return "Swamp";
    }
    return "Unknown";
}
float getBaseTerrainHeight(int wx,int wz){
    float x=wx*0.0015f;
    float z=wz*0.0015f;
    float val=fbm2D(x,z,4,2.0f,0.5f);
    float height=val*8.0f+20.0f;
    return height;
}
float getHeightWithBiome(int wx,int wz){
    float temperature=getTemperature(wx,wz);
    float rainfall=getRainfall(wx,wz);
    BiomeType biome=pickBiome(temperature,rainfall);
    float baseHeight=getBaseTerrainHeight(wx,wz);
    switch(biome){
        case BiomeType::DESERT:return baseHeight*0.9f;
        case BiomeType::MOUNTAINS:return baseHeight+6.0f;
        case BiomeType::SNOWY:return baseHeight+2.0f;
        case BiomeType::FOREST:return baseHeight+1.0f;
        case BiomeType::SWAMP:return baseHeight*0.8f;
        case BiomeType::PLAINS:default:return baseHeight;
    }
}
struct Block{BlockType type=BlockType::AIR;};
struct Chunk{
    Block blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
    glm::ivec3 position;
    unsigned int VAO=0,VBO=0,EBO=0;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};
static std::unordered_map<std::string,Chunk> chunks;
float noiseFunc(int x,int,int z){
    float n=sinf(x*0.1f)+cosf(z*0.1f);
    return n*0.5f;
}
BiomeType getBiome(int wx,int wz){
    float val=noiseFunc(wx*4,0,wz*4)+0.5f;
    if(val<0.33f)return BiomeType::DESERT;
    else if(val<0.66f)return BiomeType::PLAINS;
    else return BiomeType::MOUNTAINS;
}
float getHeightForBiome(BiomeType biome,int wx,int wz){
    float base=noiseFunc(wx,0,wz);
    switch(biome){
        case BiomeType::PLAINS:return base*5.f+10.f;
        case BiomeType::DESERT:return base*3.f+8.f;
        case BiomeType::MOUNTAINS:default:return base*30.f+40.f;
    }
}
void placeSimpleTree(Chunk& c,int x,int z,int trunkBaseY,int trunkHeight){
    for(int h=0;h<trunkHeight;h++){
        int yy=trunkBaseY+h;
        if(yy<0||yy>=CHUNK_SIZE)break;
        c.blocks[x][yy][z].type=BlockType::WOOD;
    }
    int leafY=trunkBaseY+trunkHeight;
    if(leafY>=0&&leafY<CHUNK_SIZE){
        c.blocks[x][leafY][z].type=BlockType::LEAF;
        if(x-1>=0)c.blocks[x-1][leafY][z].type=BlockType::LEAF;
        if(x+1<CHUNK_SIZE)c.blocks[x+1][leafY][z].type=BlockType::LEAF;
        if(z-1>=0)c.blocks[x][leafY][z-1].type=BlockType::LEAF;
        if(z+1<CHUNK_SIZE)c.blocks[x][leafY][z+1].type=BlockType::LEAF;
    }
}
#include <cstdlib>
glm::vec3 getBlockColor(BlockType type){
    switch(type){
        case BlockType::GRASS:return glm::vec3(0.2f,0.7f,0.2f);
        case BlockType::DIRT:return glm::vec3(0.45f,0.36f,0.22f);
        case BlockType::STONE:return glm::vec3(0.5f,0.5f,0.5f);
        case BlockType::SAND:return glm::vec3(0.9f,0.85f,0.5f);
        case BlockType::WOOD:return glm::vec3(0.55f,0.27f,0.07f);
        case BlockType::LEAF:return glm::vec3(0.1f,0.6f,0.1f);
        default:return glm::vec3(0.5f,0.5f,0.5f);
    }
}
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window,double xpos,double ypos);
void loadVisibleChunks();
void generateChunk(Chunk& c);
void generateMesh(Chunk& c);
void renderChunks(unsigned int fillShader,unsigned int outlineShader,const glm::mat4& proj,const glm::mat4& view);
void renderHighlight(glm::ivec3 pos,unsigned int highlightShader,const glm::mat4& proj,const glm::mat4& view);
std::optional<glm::ivec3> raycast();
void breakBlock(const glm::ivec3& blockPos);
bool isBlockActive(int wx,int wy,int wz);
std::string chunkKey(int cx,int cy,int cz);
Chunk* getChunk(int cx,int cy,int cz);
static void checkShaderCompileErrors(GLuint shader,const std::string& type);
static void checkProgramLinkErrors(GLuint program);
bool isChunkInView(const Chunk& c,const glm::mat4& projView);
static const glm::ivec3 faceOffsets[6]={
    glm::ivec3(1,0,0),
    glm::ivec3(-1,0,0),
    glm::ivec3(0,1,0),
    glm::ivec3(0,-1,0),
    glm::ivec3(0,0,1),
    glm::ivec3(0,0,-1),
};
static const float faceCorners[6][4][3]={
    {{1,0,0},{1,0,1},{1,1,1},{1,1,0}},
    {{0,0,0},{0,1,0},{0,1,1},{0,0,1}},
    {{0,1,0},{0,1,1},{1,1,1},{1,1,0}},
    {{0,0,0},{1,0,0},{1,0,1},{0,0,1}},
    {{0,0,1},{0,1,1},{1,1,1},{1,0,1}},
    {{0,0,0},{1,0,0},{1,1,0},{0,1,0}}
};
static const unsigned int FACE_INDICES[6]={0,1,2,2,3,0};
static std::queue<glm::ivec3> chunkGenQueue;
static std::set<std::string> pendingChunks;
static std::queue<Chunk*> chunkMeshQueue;
static float lastUnloadCheck=0.0f;
static bool showDebugWindow=false;
static bool tKeyPressedLast=false;
int main(){
    if(!glfwInit()){return -1;}
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window=glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT,"Voxel Demo w/ Biomes",nullptr,nullptr);
    if(!window){glfwTerminate();return -1;}
    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){glfwTerminate();return -1;}
    glfwSetCursorPosCallback(window,mouse_callback);
    glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io=ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init("#version 330");
    static const char* coloredVertexShaderSource=R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    out vec3 vColor;
    uniform mat4 projection;
    uniform mat4 view;
    uniform mat4 model;
    void main(){
        vColor=aColor;
        gl_Position=projection*view*model*vec4(aPos,1.0);
    }
    )";
    static const char* coloredFragmentShaderSource=R"(
    #version 330 core
    in vec3 vColor;
    out vec4 FragColor;
    void main(){
        FragColor=vec4(vColor,1.0);
    }
    )";
    unsigned int blockShader=glCreateProgram();
    {
        unsigned int vs=glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs,1,&coloredVertexShaderSource,nullptr);
        glCompileShader(vs);
        checkShaderCompileErrors(vs,"BLOCK_VS");
        unsigned int fs=glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs,1,&coloredFragmentShaderSource,nullptr);
        glCompileShader(fs);
        checkShaderCompileErrors(fs,"BLOCK_FS");
        glAttachShader(blockShader,vs);
        glAttachShader(blockShader,fs);
        glLinkProgram(blockShader);
        checkProgramLinkErrors(blockShader);
        glDeleteShader(vs);
        glDeleteShader(fs);
    }
    static const char* outlineVertexShaderSource=R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 projection;
    uniform mat4 view;
    uniform mat4 model;
    void main(){
        gl_Position=projection*view*model*vec4(aPos,1.0);
    }
    )";
    static const char* outlineFragmentShaderSource=R"(
    #version 330 core
    out vec4 FragColor;
    void main(){
        FragColor=vec4(0,0,0,1);
    }
    )";
    unsigned int outlineShader=glCreateProgram();
    {
        unsigned int vs=glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs,1,&outlineVertexShaderSource,nullptr);
        glCompileShader(vs);
        checkShaderCompileErrors(vs,"OUTLINE_VS");
        unsigned int fs=glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs,1,&outlineFragmentShaderSource,nullptr);
        glCompileShader(fs);
        checkShaderCompileErrors(fs,"OUTLINE_FS");
        glAttachShader(outlineShader,vs);
        glAttachShader(outlineShader,fs);
        glLinkProgram(outlineShader);
        checkProgramLinkErrors(outlineShader);
        glDeleteShader(vs);
        glDeleteShader(fs);
    }
    static const char* highlightVertexShaderSource=R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 projection;
    uniform mat4 view;
    uniform mat4 model;
    void main(){
        gl_Position=projection*view*model*vec4(aPos,1.0);
    }
    )";
    static const char* highlightFragmentShaderSource=R"(
    #version 330 core
    out vec4 FragColor;
    void main(){
        FragColor=vec4(1,0,0,1);
    }
    )";
    unsigned int highlightShader=glCreateProgram();
    {
        unsigned int vs=glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs,1,&highlightVertexShaderSource,nullptr);
        glCompileShader(vs);
        checkShaderCompileErrors(vs,"HIGHLIGHT_VS");
        unsigned int fs=glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs,1,&highlightFragmentShaderSource,nullptr);
        glCompileShader(fs);
        checkShaderCompileErrors(fs,"HIGHLIGHT_FS");
        glAttachShader(highlightShader,vs);
        glAttachShader(highlightShader,fs);
        glLinkProgram(highlightShader);
        checkProgramLinkErrors(highlightShader);
        glDeleteShader(vs);
        glDeleteShader(fs);
    }
    glEnable(GL_DEPTH_TEST);
    bool leftMousePressedLastFrame=false;
    initNoise((unsigned int)time(nullptr));
    while(!glfwWindowShouldClose(window)){
        float now=(float)glfwGetTime();
        deltaTime=now-lastFrame;
        lastFrame=now;
        processInput(window);
        loadVisibleChunks();
        glClearColor(0.5f,0.8f,1.0f,1.f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glm::mat4 proj=glm::perspective(glm::radians(45.f),(float)SCR_WIDTH/(float)SCR_HEIGHT,0.1f,200.f);
        glm::mat4 view=glm::lookAt(cameraPos,cameraPos+cameraFront,cameraUp);
        renderChunks(blockShader,outlineShader,proj,view);
        auto selBlock=raycast();
        if(selBlock){
            renderHighlight(*selBlock,highlightShader,proj,view);
            bool leftNow=(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS);
            if(leftNow&&!leftMousePressedLastFrame){
                breakBlock(*selBlock);
            }
            leftMousePressedLastFrame=leftNow;
        }else{
            leftMousePressedLastFrame=false;
        }
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        if(showDebugWindow){
            ImGui::SetNextWindowPos(ImVec2(10,10),ImGuiCond_Always);
            ImGui::Begin("Debug Info",nullptr,ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoSavedSettings);
            float fps=1.0f/(deltaTime>0?deltaTime:1.f);
            ImGui::Text("FPS: %.1f",fps);
            size_t totalBytes=0;
            for(const auto& kv:chunks){
                const Chunk& c=kv.second;
                totalBytes+=c.vertices.capacity()*sizeof(float);
                totalBytes+=c.indices.capacity()*sizeof(unsigned int);
            }
            float memMB=(float)totalBytes/(1024.0f*1024.0f);
            ImGui::Text("Memory (approx): %.2f MB",memMB);
            BiomeType playerBiomeDebug=pickBiome(getTemperature((int)cameraPos.x,(int)cameraPos.z),getRainfall((int)cameraPos.x,(int)cameraPos.z));
            ImGui::Text("Biome: %s",biomeToString(playerBiomeDebug).c_str());
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
void loadVisibleChunks(){
    float minX=cameraPos.x-FOG_RANGE;
    float maxX=cameraPos.x+FOG_RANGE;
    float minZ=cameraPos.z-FOG_RANGE;
    float maxZ=cameraPos.z+FOG_RANGE;
    int cMinX=(int)std::floor(minX/(float)CHUNK_SIZE);
    int cMaxX=(int)std::floor(maxX/(float)CHUNK_SIZE);
    int cMinZ=(int)std::floor(minZ/(float)CHUNK_SIZE);
    int cMaxZ=(int)std::floor(maxZ/(float)CHUNK_SIZE);
    std::vector<glm::ivec3> neededPositions;
    neededPositions.reserve((cMaxX-cMinX+1)*(cMaxZ-cMinZ+1));
    for(int cx=cMinX;cx<=cMaxX;cx++){
        for(int cz=cMinZ;cz<=cMaxZ;cz++){
            for(int cy=CHUNK_Y_MIN;cy<=CHUNK_Y_MAX;cy++){
                float centerX=(cx+0.5f)*CHUNK_SIZE;
                float centerY=(cy+0.5f)*CHUNK_SIZE;
                float centerZ=(cz+0.5f)*CHUNK_SIZE;
                float dx=centerX-cameraPos.x;
                float dy=centerY-cameraPos.y;
                float dz=centerZ-cameraPos.z;
                float distSq=dx*dx+dy*dy+dz*dz;
                if(distSq<=(FOG_RANGE*FOG_RANGE)){
                    std::string key=chunkKey(cx,cy,cz);
                    if(chunks.find(key)==chunks.end()&&pendingChunks.find(key)==pendingChunks.end()){
                        neededPositions.push_back(glm::ivec3(cx,cy,cz));
                    }
                }
            }
        }
    }
    std::sort(neededPositions.begin(),neededPositions.end(),[](const glm::ivec3 &a,const glm::ivec3 &b){
        float ax=(a.x+0.5f)*CHUNK_SIZE-cameraPos.x;
        float ay=(a.y+0.5f)*CHUNK_SIZE-cameraPos.y;
        float az=(a.z+0.5f)*CHUNK_SIZE-cameraPos.z;
        float bx=(b.x+0.5f)*CHUNK_SIZE-cameraPos.x;
        float by=(b.y+0.5f)*CHUNK_SIZE-cameraPos.y;
        float bz=(b.z+0.5f)*CHUNK_SIZE-cameraPos.z;
        float distA=ax*ax+ay*ay+az*az;
        float distB=bx*bx+by*by+bz*bz;
        return distA<distB;
    });
    for(auto &pos:neededPositions){
        std::string key=chunkKey(pos.x,pos.y,pos.z);
        chunkGenQueue.push(pos);
        pendingChunks.insert(key);
        if(chunkGenQueue.size()>100)break;
    }
    for(int i=0;i<MAX_CHUNK_GENERATIONS_PER_FRAME&&!chunkGenQueue.empty();i++){
        glm::ivec3 pos=chunkGenQueue.front();
        chunkGenQueue.pop();
        Chunk newChunk;
        newChunk.position=pos;
        generateChunk(newChunk);
        Chunk* ptr=new Chunk(newChunk);
        chunkMeshQueue.push(ptr);
        pendingChunks.erase(chunkKey(pos.x,pos.y,pos.z));
    }
    static bool cameraSpawned=false;
    for(int i=0;i<MAX_CHUNK_MESHES_PER_FRAME&&!chunkMeshQueue.empty();i++){
        Chunk* cptr=chunkMeshQueue.front();
        chunkMeshQueue.pop();
        generateMesh(*cptr);
        std::string key=chunkKey(cptr->position.x,cptr->position.y,cptr->position.z);
        chunks[key]=*cptr;
        if(!cameraSpawned&&cptr->position.x==0&&cptr->position.y==0&&cptr->position.z==0){
            int highestGlobalY=-9999;
            for(int lx=0;lx<CHUNK_SIZE;lx++){
                for(int lz=0;lz<CHUNK_SIZE;lz++){
                    for(int ly=CHUNK_SIZE-1;ly>=0;ly--){
                        if(cptr->blocks[lx][ly][lz].type!=BlockType::AIR){
                            int worldY=ly+cptr->position.y*CHUNK_SIZE;
                            if(worldY>highestGlobalY){highestGlobalY=worldY;}
                            break;
                        }
                    }
                }
            }
            if(highestGlobalY>-9999){
                cameraPos=glm::vec3(0.0f,(float)highestGlobalY+2.0f,0.0f);
                cameraSpawned=true;
            }
        }
        delete cptr;
    }
    float now=(float)glfwGetTime();
    if(now-lastUnloadCheck>CHUNK_UNLOAD_INTERVAL){
        lastUnloadCheck=now;
        std::vector<std::string> toUnload;
        toUnload.reserve(chunks.size());
        for(auto &kv:chunks){
            const std::string &key=kv.first;
            const Chunk &chunk=kv.second;
            float cx=(chunk.position.x+0.5f)*CHUNK_SIZE;
            float cy=(chunk.position.y+0.5f)*CHUNK_SIZE;
            float cz=(chunk.position.z+0.5f)*CHUNK_SIZE;
            float dx=cx-cameraPos.x;
            float dy=cy-cameraPos.y;
            float dz=cz-cameraPos.z;
            float distSq=dx*dx+dy*dy+dz*dz;
            if(distSq>(FOG_RANGE*1.2f)*(FOG_RANGE*1.2f)){
                toUnload.push_back(key);
            }
        }
        for(const auto &k:toUnload){
            auto it=chunks.find(k);
            if(it==chunks.end())continue;
            Chunk &c=it->second;
            if(c.VAO){
                glDeleteVertexArrays(1,&c.VAO);
                glDeleteBuffers(1,&c.VBO);
                glDeleteBuffers(1,&c.EBO);
            }
            chunks.erase(k);
        }
    }
}
void generateChunk(Chunk& c){
    float heightMap[CHUNK_SIZE][CHUNK_SIZE];
    BiomeType biomeMap[CHUNK_SIZE][CHUNK_SIZE];
    for(int x=0;x<CHUNK_SIZE;x++){
        for(int z=0;z<CHUNK_SIZE;z++){
            int wx=x+c.position.x*CHUNK_SIZE;
            int wz=z+c.position.z*CHUNK_SIZE;
            float temperature=getTemperature(wx,wz);
            float rainfall=getRainfall(wx,wz);
            BiomeType biome=pickBiome(temperature,rainfall);
            biomeMap[x][z]=biome;
            float hVal=getHeightWithBiome(wx,wz);
            heightMap[x][z]=hVal;
        }
    }
    for(int x=0;x<CHUNK_SIZE;x++){
        for(int z=0;z<CHUNK_SIZE;z++){
            BiomeType biome=biomeMap[x][z];
            int topInt=(int)std::floor(heightMap[x][z]);
            for(int y=0;y<CHUNK_SIZE;y++){
                int wy=y+c.position.y*CHUNK_SIZE;
                if(wy>topInt){
                    c.blocks[x][y][z].type=BlockType::AIR;
                }else{
                    bool isTop=(wy==topInt);
                    if(isTop){
                        switch(biome){
                            case BiomeType::DESERT:c.blocks[x][y][z].type=BlockType::SAND;break;
                            case BiomeType::MOUNTAINS:
                            case BiomeType::SNOWY:c.blocks[x][y][z].type=BlockType::STONE;break;
                            case BiomeType::FOREST:
                            case BiomeType::SWAMP:
                            case BiomeType::PLAINS:
                            default:c.blocks[x][y][z].type=BlockType::GRASS;break;
                        }
                    }else{
                        int depth=topInt-wy;
                        if(depth<=3){
                            switch(biome){
                                case BiomeType::DESERT:c.blocks[x][y][z].type=BlockType::SAND;break;
                                default:c.blocks[x][y][z].type=BlockType::DIRT;break;
                            }
                        }else{
                            c.blocks[x][y][z].type=BlockType::STONE;
                        }
                    }
                }
            }
        }
    }
    for(int x=1;x<CHUNK_SIZE-1;x++){
        for(int z=1;z<CHUNK_SIZE-1;z++){
            BiomeType biome=biomeMap[x][z];
            int topInt=(int)std::floor(heightMap[x][z]);
            int localTop=topInt-c.position.y*CHUNK_SIZE;
            if(localTop>=0&&localTop<CHUNK_SIZE){
                BlockType topBlock=c.blocks[x][localTop][z].type;
                if(biome==BiomeType::FOREST&&topBlock==BlockType::GRASS){
                    float r=(float)rand()/RAND_MAX;
                    if(r<0.02f){
                        placeSimpleTree(c,x,z,localTop+1,4);
                    }
                }
            }
        }
    }
}
void generateMesh(Chunk& c){
    c.vertices.clear();
    c.indices.clear();
    for(int x=0;x<CHUNK_SIZE;x++){
        for(int y=0;y<CHUNK_SIZE;y++){
            for(int z=0;z<CHUNK_SIZE;z++){
                BlockType bt=c.blocks[x][y][z].type;
                if(bt==BlockType::AIR)continue;
                int wx=x+c.position.x*CHUNK_SIZE;
                int wy=y+c.position.y*CHUNK_SIZE;
                int wz=z+c.position.z*CHUNK_SIZE;
                glm::vec3 blockColor=getBlockColor(bt);
                for(int faceID=0;faceID<6;faceID++){
                    glm::ivec3 off=faceOffsets[faceID];
                    int nx=wx+off.x;
                    int ny=wy+off.y;
                    int nz=wz+off.z;
                    if(!isBlockActive(nx,ny,nz)){
                        unsigned int startIndex=(unsigned int)c.vertices.size()/6;
                        for(int corner=0;corner<4;corner++){
                            float vx=(float)wx+faceCorners[faceID][corner][0];
                            float vy=(float)wy+faceCorners[faceID][corner][1];
                            float vz=(float)wz+faceCorners[faceID][corner][2];
                            c.vertices.push_back(vx);
                            c.vertices.push_back(vy);
                            c.vertices.push_back(vz);
                            c.vertices.push_back(blockColor.r);
                            c.vertices.push_back(blockColor.g);
                            c.vertices.push_back(blockColor.b);
                        }
                        for(int i=0;i<6;i++){
                            c.indices.push_back(startIndex+FACE_INDICES[i]);
                        }
                    }
                }
            }
        }
    }
    if(!c.vertices.empty()&&!c.indices.empty()){
        if(c.VAO){
            glDeleteVertexArrays(1,&c.VAO);
            glDeleteBuffers(1,&c.VBO);
            glDeleteBuffers(1,&c.EBO);
        }
        glGenVertexArrays(1,&c.VAO);
        glGenBuffers(1,&c.VBO);
        glGenBuffers(1,&c.EBO);
        glBindVertexArray(c.VAO);
        glBindBuffer(GL_ARRAY_BUFFER,c.VBO);
        glBufferData(GL_ARRAY_BUFFER,c.vertices.size()*sizeof(float),c.vertices.data(),GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,c.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,c.indices.size()*sizeof(unsigned int),c.indices.data(),GL_STATIC_DRAW);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }else{
        if(c.VAO){
            glDeleteVertexArrays(1,&c.VAO);
            glDeleteBuffers(1,&c.VBO);
            glDeleteBuffers(1,&c.EBO);
        }
        c.VAO=c.VBO=c.EBO=0;
    }
}
void renderChunks(unsigned int fillShader,unsigned int outlineShader,const glm::mat4& proj,const glm::mat4& view){
    glm::mat4 projView=proj*view;
    glUseProgram(fillShader);
    {
        GLint pLoc=glGetUniformLocation(fillShader,"projection");
        GLint vLoc=glGetUniformLocation(fillShader,"view");
        GLint mLoc=glGetUniformLocation(fillShader,"model");
        glUniformMatrix4fv(pLoc,1,GL_FALSE,glm::value_ptr(proj));
        glUniformMatrix4fv(vLoc,1,GL_FALSE,glm::value_ptr(view));
        if(mLoc!=-1){
            glm::mat4 I(1.0f);
            glUniformMatrix4fv(mLoc,1,GL_FALSE,glm::value_ptr(I));
        }
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        for(auto &kv:chunks){
            Chunk &c=kv.second;
            if(c.VAO==0||c.indices.empty())continue;
            if(!isChunkInView(c,projView))continue;
            glBindVertexArray(c.VAO);
            glDrawElements(GL_TRIANGLES,(GLsizei)c.indices.size(),GL_UNSIGNED_INT,0);
        }
    }
    glUseProgram(outlineShader);
    {
        GLint pLoc=glGetUniformLocation(outlineShader,"projection");
        GLint vLoc=glGetUniformLocation(outlineShader,"view");
        GLint mLoc=glGetUniformLocation(outlineShader,"model");
        glUniformMatrix4fv(pLoc,1,GL_FALSE,glm::value_ptr(proj));
        glUniformMatrix4fv(vLoc,1,GL_FALSE,glm::value_ptr(view));
        if(mLoc!=-1){
            glm::mat4 I(1.0f);
            glUniformMatrix4fv(mLoc,1,GL_FALSE,glm::value_ptr(I));
        }
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1.f,-1.f);
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        for(auto &kv:chunks){
            Chunk &c=kv.second;
            if(c.VAO==0||c.indices.empty())continue;
            if(!isChunkInView(c,projView))continue;
            glBindVertexArray(c.VAO);
            glDrawElements(GL_TRIANGLES,(GLsizei)c.indices.size(),GL_UNSIGNED_INT,0);
        }
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        glDisable(GL_POLYGON_OFFSET_LINE);
    }
}
void renderHighlight(glm::ivec3 pos,unsigned int highlightShader,const glm::mat4& proj,const glm::mat4& view){
    static unsigned int VAO=0,VBO=0,EBO=0;
    static std::vector<float> vtx;
    static std::vector<unsigned int> idx;
    if(VAO==0){
        float corners[24]={
            0,0,0,1,0,0,1,1,0,0,1,0,
            0,0,1,1,0,1,1,1,1,0,1,1
        };
        unsigned int faces[36]={
            0,1,2,2,3,0,4,5,6,6,7,4,
            0,4,7,7,3,0,1,5,6,6,2,1,
            0,1,5,5,4,0,3,2,6,6,7,3
        };
        vtx.assign(corners,corners+24);
        idx.assign(faces,faces+36);
        glGenVertexArrays(1,&VAO);
        glGenBuffers(1,&VBO);
        glGenBuffers(1,&EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferData(GL_ARRAY_BUFFER,vtx.size()*sizeof(float),vtx.data(),GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,idx.size()*sizeof(unsigned int),idx.data(),GL_STATIC_DRAW);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }
    glUseProgram(highlightShader);
    GLint pLoc=glGetUniformLocation(highlightShader,"projection");
    GLint vLoc=glGetUniformLocation(highlightShader,"view");
    GLint mLoc=glGetUniformLocation(highlightShader,"model");
    glUniformMatrix4fv(pLoc,1,GL_FALSE,glm::value_ptr(proj));
    glUniformMatrix4fv(vLoc,1,GL_FALSE,glm::value_ptr(view));
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-2.f,-2.f);
    glm::mat4 model(1.0f);
    model=glm::translate(model,glm::vec3(pos.x,pos.y,pos.z));
    glUniformMatrix4fv(mLoc,1,GL_FALSE,glm::value_ptr(model));
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,(GLsizei)idx.size(),GL_UNSIGNED_INT,0);
    glBindVertexArray(0);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
}
std::optional<glm::ivec3> raycast(){
    glm::vec3 rayPos=cameraPos;
    glm::vec3 rayDir=glm::normalize(cameraFront);
    for(float t=0;t<10.f;t+=0.1f){
        glm::ivec3 bpos((int)std::floor(rayPos.x),(int)std::floor(rayPos.y),(int)std::floor(rayPos.z));
        int cx=(int)std::floor(bpos.x/(float)CHUNK_SIZE);
        int cy=(int)std::floor(bpos.y/(float)CHUNK_SIZE);
        int cz=(int)std::floor(bpos.z/(float)CHUNK_SIZE);
        Chunk* c=getChunk(cx,cy,cz);
        if(c){
            int lx=bpos.x-cx*CHUNK_SIZE;
            int ly=bpos.y-cy*CHUNK_SIZE;
            int lz=bpos.z-cz*CHUNK_SIZE;
            if(lx>=0&&lx<CHUNK_SIZE&&ly>=0&&ly<CHUNK_SIZE&&lz>=0&&lz<CHUNK_SIZE){
                if(c->blocks[lx][ly][lz].type!=BlockType::AIR){
                    return bpos;
                }
            }
        }
        rayPos+=rayDir*0.1f;
    }
    return std::nullopt;
}
void breakBlock(const glm::ivec3& blockPos){
    int cx=(int)std::floor(blockPos.x/(float)CHUNK_SIZE);
    int cy=(int)std::floor(blockPos.y/(float)CHUNK_SIZE);
    int cz=(int)std::floor(blockPos.z/(float)CHUNK_SIZE);
    Chunk* c=getChunk(cx,cy,cz);
    if(!c)return;
    int lx=blockPos.x-cx*CHUNK_SIZE;
    int ly=blockPos.y-cy*CHUNK_SIZE;
    int lz=blockPos.z-cz*CHUNK_SIZE;
    if(lx<0||lx>=CHUNK_SIZE||ly<0||ly>=CHUNK_SIZE||lz<0||lz>=CHUNK_SIZE)return;
    c->blocks[lx][ly][lz].type=BlockType::AIR;
    generateMesh(*c);
    auto rebuildNeighbor=[&](int ox,int oy,int oz){
        Chunk* nc=getChunk(cx+ox,cy+oy,cz+oz);
        if(nc)generateMesh(*nc);
    };
    if(lx==0)rebuildNeighbor(-1,0,0);
    else if(lx==CHUNK_SIZE-1)rebuildNeighbor(1,0,0);
    if(ly==0)rebuildNeighbor(0,-1,0);
    else if(ly==CHUNK_SIZE-1)rebuildNeighbor(0,1,0);
    if(lz==0)rebuildNeighbor(0,0,-1);
    else if(lz==CHUNK_SIZE-1)rebuildNeighbor(0,0,1);
}
bool isBlockActive(int wx,int wy,int wz){
    int cx=(int)std::floor(wx/(float)CHUNK_SIZE);
    int cy=(int)std::floor(wy/(float)CHUNK_SIZE);
    int cz=(int)std::floor(wz/(float)CHUNK_SIZE);
    Chunk* c=getChunk(cx,cy,cz);
    if(!c)return false;
    int lx=wx-cx*CHUNK_SIZE;
    int ly=wy-cy*CHUNK_SIZE;
    int lz=wz-cz*CHUNK_SIZE;
    if(lx<0||lx>=CHUNK_SIZE||ly<0||ly>=CHUNK_SIZE||lz<0||lz>=CHUNK_SIZE)return false;
    return(c->blocks[lx][ly][lz].type!=BlockType::AIR);
}
std::string chunkKey(int cx,int cy,int cz){
    return std::to_string(cx)+","+std::to_string(cy)+","+std::to_string(cz);
}
Chunk* getChunk(int cx,int cy,int cz){
    auto k=chunkKey(cx,cy,cz);
    auto it=chunks.find(k);
    if(it!=chunks.end()){
        return &it->second;
    }
    return nullptr;
}
void processInput(GLFWwindow* window){
    if(glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS){
        glfwSetWindowShouldClose(window,true);
    }
    bool tKeyNow=(glfwGetKey(window,GLFW_KEY_T)==GLFW_PRESS);
    if(tKeyNow&&!tKeyPressedLast){
        showDebugWindow=!showDebugWindow;
    }
    tKeyPressedLast=tKeyNow;
    if(glfwGetKey(window,GLFW_KEY_PERIOD)==GLFW_PRESS){
        cameraBaseSpeed+=0.1f;
    }
    if(glfwGetKey(window,GLFW_KEY_COMMA)==GLFW_PRESS){
        cameraBaseSpeed-=0.1f;
        if(cameraBaseSpeed<0.f)cameraBaseSpeed=0.f;
    }
    float speed=cameraBaseSpeed*deltaTime;
    if(glfwGetKey(window,GLFW_KEY_W)==GLFW_PRESS){
        cameraPos+=cameraFront*speed;
    }
    if(glfwGetKey(window,GLFW_KEY_S)==GLFW_PRESS){
        cameraPos-=cameraFront*speed;
    }
    if(glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS){
        cameraPos-=glm::normalize(glm::cross(cameraFront,cameraUp))*speed;
    }
    if(glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS){
        cameraPos+=glm::normalize(glm::cross(cameraFront,cameraUp))*speed;
    }
    if(glfwGetKey(window,GLFW_KEY_SPACE)==GLFW_PRESS){
        cameraPos+=cameraUp*speed;
    }
    if(glfwGetKey(window,GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS){
        cameraPos-=cameraUp*speed;
    }
}
void mouse_callback(GLFWwindow*,double xpos,double ypos){
    if(firstMouse){
        lastX=(float)xpos;
        lastY=(float)ypos;
        firstMouse=false;
    }
    float xoffset=(float)xpos-lastX;
    float yoffset=lastY-(float)ypos;
    lastX=(float)xpos;
    lastY=(float)ypos;
    float sensitivity=0.1f;
    xoffset*=sensitivity;
    yoffset*=sensitivity;
    yaw+=xoffset;
    pitch+=yoffset;
    if(pitch>89.f)pitch=89.f;
    if(pitch<-89.f)pitch=-89.f;
    glm::vec3 f;
    f.x=cos(glm::radians(yaw))*cos(glm::radians(pitch));
    f.y=sin(glm::radians(pitch));
    f.z=sin(glm::radians(yaw))*cos(glm::radians(pitch));
    cameraFront=glm::normalize(f);
}
static void checkShaderCompileErrors(GLuint shader,const std::string& type){
    GLint success;
    glGetShaderiv(shader,GL_COMPILE_STATUS,&success);
    if(!success){
        GLint len;
        glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&len);
        std::vector<GLchar> log(len);
        glGetShaderInfoLog(shader,len,nullptr,log.data());
        std::cerr<<log.data()<<std::endl;
    }
}
static void checkProgramLinkErrors(GLuint program){
    GLint success;
    glGetProgramiv(program,GL_LINK_STATUS,&success);
    if(!success){
        GLint len;
        glGetProgramiv(program,GL_INFO_LOG_LENGTH,&len);
        std::vector<GLchar> log(len);
        glGetProgramInfoLog(program,len,nullptr,log.data());
        std::cerr<<log.data()<<std::endl;
    }
}
bool isChunkInView(const Chunk& c,const glm::mat4& projView){
    glm::vec3 center((c.position.x+0.5f)*CHUNK_SIZE,(c.position.y+0.5f)*CHUNK_SIZE,(c.position.z+0.5f)*CHUNK_SIZE);
    float radius=(float)CHUNK_SIZE*0.5f*1.73205f;
    glm::vec4 clipPos=projView*glm::vec4(center,1.0f);
    float x=clipPos.x;
    float y=clipPos.y;
    float z=clipPos.z;
    float w=clipPos.w;
    if(x<-w-radius)return false;
    if(x>w+radius)return false;
    if(y<-w-radius)return false;
    if(y>w+radius)return false;
    if(z<0-radius)return false;
    if(z>w+radius)return false;
    return true;
}
